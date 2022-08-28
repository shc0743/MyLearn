#include "srv.h"
#include "../lib/mongoose/mongoose.h"
#include "../../resource/tool.h"
#include "str_table.h"
#include "mycryp.h"
#include "rt_user_auth.h"

static rt_srv_config* rt_config;
static std::map<std::string, std::string> http_file_mapping;
static std::map<std::string, std::string> http_file_mapping_authreq;
static void rt_srv_load_file_mapping();
static bool rt_srv_validate_config(rt_srv_config* config);
static void rt_srv_handler(struct mg_connection*, int, void*, void*);
static void rt_srv_apihandler(struct mg_connection*, int, void*, void*);

#if 0
#define MY_RT_SRV_DEBUG_INFO(x) (x)
#else
#define MY_RT_SRV_DEBUG_INFO(x)
#endif

DWORD __stdcall rt_srv_main(PVOID config) {
	using namespace std;
	if (!config) return ERROR_INVALID_PARAMETER;
	rt_srv_config* conf = (rt_srv_config*)config;
	::rt_config = conf;

	if (!rt_srv_validate_config(conf)) return ERROR_INVALID_DATA;

	rt_srv_load_file_mapping();

	MY_RT_SRV_DEBUG_INFO(system("echo [%date%_%time%] Server start! >> srv.log"));

	struct mg_mgr mgr;
	mg_mgr_init(&mgr); // Init manager
	mg_http_listen(&mgr, ("http:""//0.0.0.0:" +
		std::to_string(conf->lpConfig->server_port)).c_str(),
		rt_srv_handler, &mgr);  // Setup listener
	while (conf->serviceStatus != SERVICE_STOPPED)
		mg_mgr_poll(&mgr, 1000); // Event loop
	mg_mgr_free(&mgr); // Cleanup

	MY_RT_SRV_DEBUG_INFO(system("echo [%date%_%time%] Server stop! >> srv.log"));

	return 0;
}

bool rt_srv_validate_config(rt_srv_config* config) {
	if (config->cbSize == 0) return false;
	//if (config->lpConfig == 0) return false;
	return true;
}

void rt_srv_load_file_mapping() {
	using namespace std;

	fstream fp;
	char buffer[2048]{};
	string s;

	auto _lfm = [&](string file, map<string, string>& var) {
		fp.open(file, ios::in);
		if (fp) {
			while (fp.getline(buffer, 2048)) {
				s = buffer;
				if (s.find("=") == string::npos) continue;
				pair<string, string> p;
				p.second = s.substr(s.find_first_of("=") + 1);
				p.first = s.erase(s.find_first_of("="));
				var.insert(p);
			}
			fp.close();
		}
	};

	_lfm("resources/http_file_mapping.txt", http_file_mapping);
	_lfm("resources/http_file_mapping-auth_required.txt", http_file_mapping_authreq);

}

void rt_srv_handler(mg_connection* c, int ev, void* ev_data, void* fn_data) {
	if (ev == MG_EV_HTTP_MSG) {
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;
		struct mg_http_serve_opts opts = {};

		if (rt_config->serviceStatus != SERVICE_RUNNING) {
			if (mg_http_match_uri(hm, "/api/server/run")) {
				// Attempt to fetch parameters from the body, hm->body
				struct mg_str params = hm->body;

				mg_http_reply(c, 403, NULL, "");
			}
			else {
				my_http_serve_file(c, hm, "html/error/srv_not_running.html",
					&opts, 503, "Service Unavailable");
			}
			return;
		}

		if (std::string(hm->uri.ptr).substr(0, 5) == "/api/") {
			return rt_srv_apihandler(c, ev, ev_data, fn_data);
		}

		for (auto& i : http_file_mapping) {
			if (mg_http_match_uri(hm, i.first.c_str())) {
				if (i.second.substr(0, 9) == "redirect:" && i.second.length() > 9) {
					MY_RT_SRV_DEBUG_INFO(system(("echo [%date%_%time%] Redirect: " +
						i.first + " -^> " + i.second+" >> srv.log").c_str()));
					mg_printf(c, ("HTTP/1.1 301 Moved Permanently\r\n"
						"Content-Length: 0\r\n"
						"Location: " + i.second.substr(9) +"\r\n\r\n").c_str());
				}
				else if (i.second.substr(0, 5) == "goto:" && i.second.length() > 5) {
					MY_RT_SRV_DEBUG_INFO(system(("echo [%date%_%time%] Redirect (302): "
						+ i.first + " -^> " + i.second+" >> srv.log").c_str()));
					mg_printf(c, ("HTTP/1.1 302 Found\r\n"
						"Content-Length: 0\r\n"
						"Location: " + i.second.substr(5) + "\r\n\r\n").c_str());
				}
				else {
					mg_http_serve_file(c, hm, i.second.c_str(), &opts);
				}
				return;
			}
		}

		my_http_serve_file(c, hm, "html/error/not_found.html",
			&opts, 404, 0);
	}
}

void rt_srv_apihandler(mg_connection* c, int ev, void* ev_data, void* fn_data) {
	HMODULE m = GetModuleHandle(TEXT("rsah.dll"));
	if (m) {
		typedef void(__stdcall* t)(mg_connection*, int, void*, void*);
		t f = (t)GetProcAddress(m, "rt_srv_api_handler");
		if (f) {
			f(c, ev, ev_data, fn_data);
		}

		return;
	}
	mg_http_reply(c, 500, NULL, "");
}




