#include <iostream>
#include <string>
#include <drogon/drogon.h>
#include "../../resource/tool.h"

using namespace std;

#pragma comment(linker, "/subsystem:windows /entry:wmainCRTStartup")

string version() {
    return "5.6.0 Paralogism";
}

int wmain(int argc, const wchar_t* argv[]) {
	wstring web_root, portstr, ssl_cert, ssl_key, host;
	CmdLineW cl(GetCommandLineW());

	if (!cl.getopt(L"webroot", web_root))
		cl.getopt(L"web", web_root);
    if (!cl.getopt(L"port", portstr))
		cl.getopt(L"p", portstr);
	cl.getopt(L"ssl-cert", ssl_cert);
    cl.getopt(L"ssl-key", ssl_key);
    cl.getopt(L"host", host);

	if (cl.getopt(L"help") || web_root.empty() || portstr.empty()) {
        cout << "Usage: HttpServer.exe [options]" << endl
            << "Options:" << endl
            << "      --help                   Show this help message and exit" << endl
            << "  --web|--webroot=<web_root>  Set the web root directory" << endl
            << "  -p|--port=<port>             Set the port number" << endl
            << "  --ssl-cert=[cert_file]        Set the SSL certificate file" << endl
            << "  --ssl-key=[key_file]          Set the SSL key file" << endl
            << "  --host=[host]                 Set the host name" << endl;

        return 0;
	}

    if (host.empty()) host = L"127.0.0.1";
	int port = atoi(ws2c(portstr));
	if (!port) {
		cerr << "Error: Port not specified" << endl;
        return 87;
	}

    cout << "HttpServer " << version() << endl <<
        "Server will run on http" << (ssl_cert.empty() ? "" : "s") << "://" << ws2s(host) << ":" << ws2s(portstr) << endl;

    SetCurrentDirectoryW(web_root.c_str());
    auto& app = drogon::app();
	app .setLogLevel(trantor::Logger::kWarn)
		//.setDefaultHandler(handler)
		.setDocumentRoot(ConvertUTF16ToUTF8(web_root))
		.setThreadNum(0)
		.setClientMaxBodySize(2147483648)
		.setClientMaxMemoryBodySize(67108864)
		.addListener(ConvertUTF16ToUTF8(host), port, (!ssl_cert.empty() && !ssl_key.empty()));

	if (!ssl_cert.empty() && !ssl_key.empty()) {
        app.setSSLFiles(ConvertUTF16ToUTF8(ssl_cert), ConvertUTF16ToUTF8(ssl_key));
	}

	app.run();
	cout << "Server stopped." << endl;
	return 0;
}


