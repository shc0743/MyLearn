#include "srv.h"
#include "../lib/mongoose/mongoose.h"
#include "../../resource/tool.h"
#include "str_table.h"

static rt_srv_config* rt_config;
static std::map<std::string, std::string> http_file_mapping;
static std::map<std::string, std::string> http_file_mapping_authreq;
static void rt_srv_load_file_mapping();
static bool rt_srv_validate_config(rt_srv_config* config);
static void rt_srv_handler(struct mg_connection*, int, void*, void*);
static void rt_srv_apihandler(struct mg_connection*, int, void*, void*);
static rt_userinfo rt_LogonUser(std::wstring username, std::wstring password_enc);

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
	if (config->lpConfig == 0) return false;
	return true;
}

void rt_srv_load_file_mapping() {
	using namespace std;

	fstream fp;
	char buffer[2048]{};
	string s;

	fp.open("resources/http_file_mapping.txt", ios::in);
	if (fp) {
		while (fp.getline(buffer, 2048)) {
			s = buffer;
			if (s.find("=") == string::npos) continue;
			pair<string, string> p;
			p.second = s.substr(s.find_first_of("=") + 1);
			p.first = s.erase(s.find_first_of("="));
			http_file_mapping.insert(p);
		}
		fp.close();
	}

	fp.open("resources/http_file_mapping-auth_required.txt", ios::in);
	if (fp) {
		while (fp.getline(buffer, 2048)) {
			s = buffer;
			if (s.find("=") == string::npos) continue;
			pair<string, string> p;
			p.second = s.substr(s.find_first_of("=") + 1);
			p.first = s.erase(s.find_first_of("="));
			http_file_mapping_authreq.insert(p);
		}
		fp.close();
	}

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
				MY_RT_SRV_DEBUG_INFO(system(("echo [%date%_%time%] Serve file:"
					+ i.first + " >> srv.log").c_str()));
				mg_http_serve_file(c, hm, i.second.c_str(), &opts);
				return;
			}
		}

		MY_RT_SRV_DEBUG_INFO(system((std::string("echo [%date%_%time%] 404 file:")
			+ hm->uri.ptr + " >> srv.log").c_str()));

		my_http_serve_file(c, hm, "html/error/not_found.html",
			&opts, 404, 0);
	}
}

void rt_srv_apihandler(mg_connection* c, int ev, void* ev_data, void* fn_data) {
	struct mg_http_message* hm = (struct mg_http_message*)ev_data;
	struct mg_http_serve_opts opts = {};
	if (mg_http_match_uri(hm, "/api/test")) {
		// Attempt to fetch parameters from the body, hm->body
		struct mg_str params = hm->body;

		mg_http_reply(c, 200, NULL, "This is a test!");
		return;
	}


	my_http_serve_file(c, hm, "html/error/not_found.html",
		&opts, 404, 0);
}




