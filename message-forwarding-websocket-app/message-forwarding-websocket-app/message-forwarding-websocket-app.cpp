#include <drogon/drogon.h>
#include <drogon/WebSocketController.h>
#include <unordered_map>
#include <set>
using namespace std;

using namespace drogon;

#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "crypt32.lib")

std::string ConvertUTF16ToUTF8(PCWSTR utf16Str) {
	if (utf16Str == nullptr) return "";
	int utf16Len = static_cast<int>(wcslen(utf16Str));
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, utf16Str,
		utf16Len, nullptr, 0, nullptr, nullptr);
	std::string utf8Str(utf8Len, 0);  // 创建一个足够大的字符串来容纳UTF-8字符串
	WideCharToMultiByte(CP_UTF8, 0, utf16Str, utf16Len, &utf8Str[0],
		utf8Len, nullptr, nullptr);
	return utf8Str;
}

class WebSocketServer : public drogon::WebSocketController<WebSocketServer, false>
{
public:
	void handleNewMessage(const WebSocketConnectionPtr& conn,
		std::string&& message,
		const WebSocketMessageType& type) override
	{
		std::lock_guard<std::mutex> lock(mutex_);

		try {
			auto clientId = conn->getContext<std::string>();
			if (!clientId || !clientMap_.contains(*clientId)) {
				throw exception("Invalid client_id");
			}
			auto& clients = clientMap_[*clientId];

			if (clients.size() == 2)
			{
				for (auto& client : clients)
				{
					if (client != conn)
					{
						client->send(message);
					}
				}
			}
		} catch (...) {}
	}

	void handleNewConnection(const HttpRequestPtr& req,
		const WebSocketConnectionPtr& conn) override
	{
		// 添加token验证
		if (!token_.empty()) {
			if ((!req->parameters().contains("token")) || req->getParameter("token") != token_) {
				conn->shutdown(CloseCode::kViolation, "Not authorized");
				return;
			}
		}
		string clientId = req->getParameter("client_id");
		if (clientId.empty()) {
			conn->shutdown(CloseCode::kInvalidMessage, "Invalid client id");
			return;
		}

		std::lock_guard<std::mutex> lock(mutex_);
		if (clientMap_.contains(clientId) && clientMap_.at(clientId).size() >= 2) {
			conn->shutdown(CloseCode::kInvalidMessage, "Too many connections");
			return;
		}

		conn->setContext(std::make_shared<std::string>(clientId));
		
		if (clientMap_.contains(clientId)) {
			auto& clients = clientMap_.at(clientId);
			clients.insert(conn);
			for (auto& client : clients) {
				client->send("connected");
			}
		}
		else {
			std::set<WebSocketConnectionPtr> clients;
			clients.insert(conn);
			clientMap_.emplace(clientId, std::move(clients));
		}
	}

	void handleConnectionClosed(const WebSocketConnectionPtr& conn) override
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto clientId = conn->getContext<std::string>();
		if (!clientId) return;
		auto it = clientMap_.find(*clientId);

		if (it != clientMap_.end())
		{
			it->second.erase(conn);
			if (it->second.empty())
			{
				clientMap_.erase(it);
			}
			else
			{
				// 如果还剩一个客户端，也关闭它
				for (auto& remainingConn : it->second)
				{
					remainingConn->shutdown();
				}
				clientMap_.erase(it);
			}
		}
	}

	WS_PATH_LIST_BEGIN
		WS_PATH_ADD("/fd/echo");
	WS_PATH_LIST_END

	std::string token_;
private:
	std::mutex mutex_;
	std::unordered_map<std::string, std::set<WebSocketConnectionPtr>> clientMap_;
};

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: message-forwarding-websocket-app.exe <port : Integer> [token : String]\n\tor <ipWithPort>" << std::endl;
		return 1;
	}

	string ipp = argv[1];
	string portstr = (ipp.find(":") == ipp.npos) ? ipp : ipp.substr(ipp.find_last_of(":") + 1);
	int port;
	try {
		port = std::stoi(portstr);
	}
	catch (...) {
		cerr << "Invalid port: " << ipp << endl;
		return 87;
	}
	string srv_addr = "[::]";
	if (ipp.find(":") != ipp.npos) srv_addr = ipp.substr(0, ipp.find_last_of(":"));
	cout << "Will run server on " << srv_addr << ":" << port << endl;

	auto server = std::make_shared<WebSocketServer>();

	if (argc >= 3) {
		server->token_ = argv[2];
	}

	WCHAR tmpPath[MAX_PATH]{};
	GetTempPathW(MAX_PATH, tmpPath);
	string tmp_dir = ConvertUTF16ToUTF8((tmpPath + L"/tmp_uploads_"s + to_wstring(GetCurrentProcessId())).c_str());

	app().setUploadPath(tmp_dir);
	app().registerController(server);
	bool usessl = false;
	if (
		GetFileAttributesW(L"ssl.cer") != INVALID_FILE_ATTRIBUTES &&
		GetFileAttributesW(L"ssl.key") != INVALID_FILE_ATTRIBUTES
	) {
		cout << "SSL files found, enabling SSL..." << endl;
		usessl = true;
		app().setSSLFiles("ssl.cer", "ssl.key");
	}
	app().addListener(srv_addr, port, usessl);

	cout << "Running server on " << srv_addr << ":" << port << endl;

	app().run();

	cout << "Quitting..." << endl;
	return 0;
}