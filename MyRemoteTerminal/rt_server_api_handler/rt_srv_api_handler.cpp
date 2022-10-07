#include "../lib/mongoose/mongoose.h"
#include "rt_functions_entry.h"
#include <string>
#include "../MyRemoteTerminal/mycryp.h"
#include "../../resource/tool.h"
#include "../MyRemoteTerminal/str_table.h"

using namespace std;


PVOID rt_functions_entry[MAX_RT_FUNCTIONS_COUNT];
static WCHAR pszServiceName[1024]{};
constexpr time_t session_max_avail_time = 600;


//bool(*__stdcall rt_generate_rsakey2)(FILE* , FILE* , UINT );
unsigned long long(__stdcall* rt_GetUserCount)();

void __stdcall InitialFunctionsEntry(PVOID fs, size_t count) {
	memcpy_s(rt_functions_entry, sizeof(rt_functions_entry), fs, count);

}

void __stdcall dllinit() {
	WCHAR hSahName[256]{}, hAuthName[256]{};
	HINSTANCE hInst = GetModuleHandle(NULL); // Get EXE module
	if (!(LoadStringW(hInst, IDS_STRING_SRV_DLL_RSAH, hSahName, 256) &&
		LoadStringW(hInst, IDS_STRING_SRV_DLL_RAUTH, hAuthName, 256))) {
		throw std::exception("RAUTH string is missing");
	}

	HMODULE hauth = LoadLibraryW(hAuthName);
	if (hauth) {
		rt_GetUserCount = (decltype(rt_GetUserCount))
			GetProcAddress(hauth, "rt_GetUserCount");
		if (!rt_GetUserCount) throw std::exception("rauth.dll is corrupted");

#if 0
		rt_generate_rsakey2 = (decltype(rt_generate_rsakey2))
			GetProcAddress(hauth, "rt_generate_rsakey");
		if (!rt_generate_rsakey2) throw std::exception("rauth.dll is corrupted");
#endif
	}
	else throw std::exception("rauth.dll is missing");

	CmdLineW cl(GetCommandLineW());
	wstring svc_name;
	cl.getopt(L"service-name", svc_name);
	wcscpy_s(pszServiceName, svc_name.c_str());


#if 0
	FILE* fp1 = NULL, *fp2 = NULL;
	fopen_s(&fp1, "srv_pub.tmp", "wb");
	fopen_s(&fp2, "srv_priv.tmp", "wb");
	if (fp1 && fp2) {
		rt_generate_rsakey(fp1, fp2, 4096);
		fclose(fp1); fclose(fp2);
	}
	else {
		if (fp1) fclose(fp1);
		if (fp2) fclose(fp2);
	}
#endif


}




static string print_ip6num_to_str(uint8_t ip6[16]) {
	string o;
	char buffer[2 + 1]{};
	for (uint32_t i = 0; i < 16; ++i) {
		sprintf_s(buffer, "%02x", ip6[i]);
		o += buffer;
	}
	return o;
}


static void __stdcall rt_srv_api_handler__path_api_config(
	mg_connection* c, int ev, void* ev_data, void* fn_data
) {
	struct mg_http_message* hm = (struct mg_http_message*)ev_data;
	struct mg_http_serve_opts opts = {};

	if (mg_http_match_uri(hm, "/api/config/")) {
		auto user_count = rt_GetUserCount();
		if (user_count == 0) {
			mg_http_serve_file(c, hm,
				"html/html/main/config/initial-config.html", &opts);
		}
		else {
			mg_http_reply(c, 500, NULL, "TODO: User already exists");
		}

		return;
	}

	if (0 == rt_GetUserCount()) {
		struct sess_info {
			string token;
			time_t start_time;
			time_t last_time;
		};
		static std::map<string, sess_info> sess;

		if (mg_http_match_uri(hm, "/api/config/get_step")) {
			string bd;
			if (hm->body.len) {
				bd = hm->body.ptr;
				bd = bd.substr(0, hm->body.len);
			}

			try {
				auto& s = sess.at(bd);

				if (time(0) - s.last_time > session_max_avail_time) {
					sess.erase(bd);
					throw 1;
				}
				s.last_time = time(0);

				mg_http_reply(c, 200, "X-Server-Result: 0x0\r\n"
					"My-Authorization: YES\r\n",
					"df9dfb9ec41e4e2fb04c4380aff8fa9a=Welcome|"
					"822767c3f45e447b9640137379b49430=Configuration|"
					"cb7fcfa9279f4a1a855ac123c5fafc76=Set Password|"
					"33c93cb0c5ac48ab85109f3bfc314af0=Continue|"
					"7488ce326aea4807944d23613b094b28=Done");
				return;
			}
			catch (...) {}

			if (1) {
				mg_http_reply(c, 200, "X-Server-Result: 0x0\r\n"
					"My-Authorization: NO\r\n",
					"fd9c894b943a404c9eac4700b51b0265=Begin|"
					"7c7e2af3e2d1412e8f500e557b4eb6ff=License|"
					"1a9f3d4c37754d9a8855edf34cf6f88f=Access token|"
					"4bd130a7898a4e089df4783e98bd5b78=Welcome|"
					"d2c07e80d2f245e4b24341f79f61ae5b=Continue");
			} // TODO

			return;
		}
		if (mg_http_match_uri(hm, "/api/config/get_step_content")) {
			string bd;
			if (hm->body.len) {
				bd = hm->body.ptr;
				bd = bd.substr(0, hm->body.len);
			}
			else {
				mg_http_reply(c, 400, "X-Server-Result: 87\r\n", "");
				return;
			}

			if (bd == "fd9c894b943a404c9eac4700b51b0265") {
				mg_http_reply(c, 200, "X-Server-Result: 0\r\n",
					"step?fd9c894b943a404c9eac4700b51b0265");
				return;
			}
			else if (bd == "7c7e2af3e2d1412e8f500e557b4eb6ff") {
				mg_http_reply(c, 200, "X-Server-Result: 0\r\n",
					"step?7c7e2af3e2d1412e8f500e557b4eb6ff");
				return;
			}
			else if (bd == "1a9f3d4c37754d9a8855edf34cf6f88f") {
				mg_http_reply(c, 200, "X-Server-Result: 0\r\n",
					"step?1a9f3d4c37754d9a8855edf34cf6f88f");
				return;
			}
			else if (bd == "4bd130a7898a4e089df4783e98bd5b78") {
				mg_http_reply(c, 200, "X-Server-Result: 0\r\n",
					"step?4bd130a7898a4e089df4783e98bd5b78");
				return;
			}
			else if (bd == "d2c07e80d2f245e4b24341f79f61ae5b") {
				mg_http_reply(c, 200, "X-Server-Result: 0\r\n",
					"step?d2c07e80d2f245e4b24341f79f61ae5b");
				return;
			}
			else if (bd == "df9dfb9ec41e4e2fb04c4380aff8fa9a") {
				mg_http_reply(c, 200, "X-Server-Result: 0\r\n",
					"step?df9dfb9ec41e4e2fb04c4380aff8fa9a");
				return;
			}


			else mg_http_reply(c, 400, "X-Server-Result: 87\r\n", "%s", bd.c_str());

			return;
		}
		if (mg_http_match_uri(hm, "/api/config/step")) {
			string hd, bd;
			if (hm->query.len) {
				hd = hm->query.ptr;
				hd = hd.substr(0, hm->query.len);
			}
			else {
				mg_http_reply(c, 400, "X-Server-Result: 87\r\n", "");
				return;
			}
			if (hm->body.len) {
				bd = hm->body.ptr;
				bd = bd.substr(0, hm->body.len);
			}

			opts.extra_headers = "X-Server-Result: 0\r\n";

			if (hd == "fd9c894b943a404c9eac4700b51b0265") {
				mg_http_serve_file(c, hm, "html/html/main/config/initial-config/"
					"begin.fd9c894b943a404c9eac4700b51b0265.html", &opts);
				return;
			}
			else if (hd == "7c7e2af3e2d1412e8f500e557b4eb6ff") {
				mg_http_serve_file(c, hm, "html/html/main/config/initial-config/"
					"license.7c7e2af3e2d1412e8f500e557b4eb6ff.html", &opts);
				return;
			}
			else if (hd == "1a9f3d4c37754d9a8855edf34cf6f88f") {
				mg_http_serve_file(c, hm, "html/html/main/config/initial-config/"
					"access.1a9f3d4c37754d9a8855edf34cf6f88f.html", &opts);
				return;
			}
			else if (hd == "4bd130a7898a4e089df4783e98bd5b78") {
				mg_http_serve_file(c, hm, "html/html/main/config/initial-config/"
					"welcome.4bd130a7898a4e089df4783e98bd5b78.html", &opts);
				return;
			}
			else if (hd == "d2c07e80d2f245e4b24341f79f61ae5b") {
				mg_http_reply(c, 200, "X-Server-Result: 0\r\n",
					"Redirect...<form target=_parent id=a><input id=b type=hidden name"
					"=session></form><script>b.value=sessionStorage.getItem"
					"('initial_config-sessionid');a.submit()</script>");
				return;
			}
			else if (hd == "df9dfb9ec41e4e2fb04c4380aff8fa9a") {
				try {
					auto& s = sess.at(bd);

					if (time(0) - s.last_time > session_max_avail_time) {
						sess.erase(bd);
						throw 1;
					}
					s.last_time = time(0);

					mg_http_serve_file(c, hm, "html/html/main/config/initial-config/"
						"begin.df9dfb9ec41e4e2fb04c4380aff8fa9a.html", &opts);
				}
				catch (...) { mg_http_reply(c, 403, NULL, ""); }
				return;
			}


			else mg_http_reply(c, 400, "X-Server-Result: 87\r\n", "");

			return;
		}
		if (mg_http_match_uri(hm, "/api/config/create_session_by_token")) {
			string bd;
			if (hm->body.len) {
				bd = hm->body.ptr;
				bd = bd.substr(0, hm->body.len);
			}
			else {
				mg_http_reply(c, 400, "X-Server-Result: 87\r\n", "");
				return;
			}

			WCHAR pp[256]{};
			LoadStringW(NULL, IDS_STRING_SRV_AUTHSRV_NAME, pp, 256);
			wstring pn = pp;
			LoadStringW(NULL, IDS_STRING_APP_UUID, pp, 256);
			str_replace(pn, L"%uuid%", pp);
			str_replace(pn, L"%ServiceName%", pszServiceName);
			HANDLE hFile = CreateFileW(pn.c_str(), GENERIC_READ | GENERIC_WRITE,
				0, 0, OPEN_EXISTING, 0, 0);
			if (!hFile || hFile == INVALID_HANDLE_VALUE) {
				mg_http_reply(c, 500, ("X-Server-Result: " + to_string(
					GetLastError()) + "\r\n").c_str(), "");
				return;
			}

			DWORD p0 = DWORD((bd.length()) + 1 * sizeof(char)), p1 = p0;
			size_t buf_size = p0;
			PSTR pBuffer = (NULL);
			WriteFile(hFile, &buf_size, sizeof(buf_size), &p1, 0);
			WriteFile(hFile, bd.c_str(), p0, &p1, 0);

			(void)(ReadFile(hFile, &buf_size, sizeof(buf_size), &p1, 0) + 1);
			pBuffer = (PSTR)calloc(1, buf_size);
			if (!pBuffer) {
				mg_http_reply(c, 500, "X-Server-Result: 14\r\n", "");
				return;
			}
			memset(pBuffer, 0, buf_size);
			(void)(ReadFile(hFile, pBuffer, (DWORD)buf_size, &p1, 0) + 1);

			CloseHandle(hFile);

			string str = pBuffer;
			free(pBuffer);
			if (str.empty() || str[0] == '\0') {
				mg_printf(c, "HTTP/1.1 403 Invalid Access Token\r\n"
					"Content-Length: 0\r\n"
					"X-Server-Result: 5\r\n\r\n");
				return;
			}
			sess.insert(std::make_pair(str, sess_info{
				.token = bd,
				.start_time = time(0),
				.last_time = time(0),
				}));
			mg_http_reply(c, 200, "X-Server-Result: 0\r\n", str.c_str());

			return;
		}


	}


	mg_http_reply(c, 404, "X-Server-Result:87", "");
	return;
}
static void __stdcall rt_srv_api_handler__path_api_static_licenses_show(
	mg_connection* c, int ev, void* ev_data, void* fn_data
) {
	struct mg_http_message* hm = (struct mg_http_message*)ev_data;
	struct mg_http_serve_opts opts = {};

	static std::map<string, string> mapping;
	if (mapping.empty()) {
		mapping["lib=this"] = "html/static/gpl-3.0.html";
		mapping["lib=openssl"] = "html/static/licenses/openssl.txt";
		mapping["lib=mongoose"] = "html/static/licenses/mg.txt";
	}

	string query;
	if (hm->query.ptr) {
		query = hm->query.ptr;
		query = query.substr(0, hm->query.len);
	}

	try {
		auto u = mapping.at(query);
		mg_http_serve_file(c, hm, u.c_str(), &opts);
	}
	catch (...) {
		mg_http_reply(c, 404, "My-Text: 404 Not Found", "");
	}


}


void __stdcall rt_srv_api_handler(mg_connection* c, int ev, void* ev_data, void* fn_data) {
	struct mg_http_message* hm = (struct mg_http_message*)ev_data;
	struct mg_http_serve_opts opts = {};

	if (mg_http_match_uri(hm, "/api/auth/login")) {
		mg_printf(c, "HTTP/1.1 302 Found\r\n"
			"Content-Length: 0\r\n"
			"Location: %s\r\n\r\n", "/auth/oauth2/login");

		return;
	}
	if (mg_http_match_uri(hm, "/api/auth/api/GetUserCount")) {
		mg_http_reply(c, 200, NULL, to_string(rt_GetUserCount()).c_str());
		//FreeLibrary(hauth);

		return;
	}

	if (string(hm->uri.ptr).substr(0, 12) == "/api/config/") {
		return rt_srv_api_handler__path_api_config(c, ev, ev_data, fn_data);
	}

	if (mg_http_match_uri(hm, "/api/secure/secure.js")) {
		string str = hm->query.ptr;
		str = str.substr(0, hm->query.len);

		if (str == "") {
			mg_http_reply(c, 404, NULL, "");
		}
		else if (str == "config/initial-config") {
			if (rt_GetUserCount() == 0) {
				mg_http_serve_file(c, hm, "html/html/main/config/initial-config.js", &opts);
			}
			else {
				mg_printf(c, "HTTP/1.1 418 I'm a teapot\r\nContent-Length: 0\r\n\r\n");
			}
		}
		else {
			mg_printf(c, "HTTP/1.1 418 I'm a teapot\r\nContent-Length: 0\r\n\r\n");
		}

		return;
	}

	if (mg_http_match_uri(hm, "/api/secure/server.pub")) {
		opts.mime_types = "application/binary";
		my_http_serve_file_noEtag(c, hm, "svc_pub.tmp", &opts, 200, "OK");
		return;
	}

	if (mg_http_match_uri(hm, "/api/faq/helpful-report/report")) {
		struct mg_str params = hm->body;
		std::string data = params.ptr;
		data = data.substr(0, params.len);
		
		if (!file_exists(L"./data/faq-helpful-report")) {
			CreateDirectoryW(L"data", NULL);
			CreateDirectoryW(L"data\\faq-helpful-report", NULL);
		}

		string ip = (c->rem.is_ip6 ? 
			print_ip6num_to_str(c->rem.ip6) :
			to_string(c->rem.ip));
		fstream fp("./data/faq-helpful-report/time=" + to_string(time(0)) +
			",ip=" + ip + ",random=" + to_string(rand()) + ".dat", ios::out);
		if (fp) {
			fp << "Helpful Report File\n========\n"
				"IP Address=" << ip << endl;
			fp << data;
			fp.close();
		}


		mg_http_reply(c, 204, NULL, "");
		return;
	}

	if (mg_http_match_uri(hm, "/api/auth/passwordrecovery/faq")) {
		mg_http_serve_file(c, hm, "html/LogonAuth/passwdrecovery.htm", &opts);

		return;
	}

	if (mg_http_match_uri(hm, "/api/static/licenses/show")) {
		return rt_srv_api_handler__path_api_static_licenses_show(c, ev, ev_data, fn_data);
	}

	if (mg_http_match_uri(hm, "/api/favicon.ico")) {
#if 0
		if (!file_exists("_favicon.ico")) {
			HMODULE cmd = LoadLibraryW(L"cmd.exe");
			if (!cmd) mg_http_reply(c, 500, ("X-Server-Result: " +
				to_string(GetLastError())).c_str(), "");
			FreeResFile(32512, "ICON", "_favicon.ico", cmd);
		}

		mg_http_serve_file(c, hm, "_favicon.ico", &opts);
#else
		mg_http_serve_file(c, hm, "resources/cmd.ico", &opts);
#endif

		return;
	}

	if (mg_http_match_uri(hm, "/api/test")) {
		// Attempt to fetch parameters from the body, hm->body
		struct mg_str params = hm->body;

		mg_http_reply(c, 200, NULL, "This is a test!");
		return;
	}


	mg_http_reply(c, 404, "X-Server-Result: 2\r\nServer-Api-Response"
		"-Text: 404 Not Found\r\n", "{\"code\":404}");
}



