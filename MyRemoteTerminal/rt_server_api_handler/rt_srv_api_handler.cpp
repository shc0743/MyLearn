#include "../lib/mongoose/mongoose.h"
#include "rt_functions_entry.h"
#include <string>
#include "../MyRemoteTerminal/mycryp.h"
#include "../../resource/tool.h"

using namespace std;


PVOID rt_functions_entry[MAX_RT_FUNCTIONS_COUNT];

void __stdcall InitialFunctionsEntry(PVOID fs, size_t count) {
	memcpy_s(rt_functions_entry, sizeof(rt_functions_entry), fs, count);
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
		mg_http_reply(c, 200, NULL, "0");

		return;
	}

#if 0
	if (mg_http_match_uri(hm, "/api/crypto/sha256")) {
		struct mg_str params = hm->query;
		if (params.len < 3 /* Min: a=b */) {
			mg_http_reply(c, 400, NULL, "");
			return;
		}

		string str = params.ptr;
		str = str.substr(0, params.len);
		str += ("\r\n"s + ((char*)((unsigned char*)str.c_str())) +
			"\r\n"s + rt_sha256(str));

		mg_printf(c, ("HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: "s + to_string(str.length())
			+ "\r\n\r\n" + str).c_str());
		return;
	}
#endif

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

	if (mg_http_match_uri(hm, "/api/test")) {
		// Attempt to fetch parameters from the body, hm->body
		struct mg_str params = hm->body;

		mg_http_reply(c, 200, NULL, "This is a test!");
		return;
	}


	my_http_serve_file(c, hm, "html/error/not_found.html",
		&opts, 404, 0);
}



