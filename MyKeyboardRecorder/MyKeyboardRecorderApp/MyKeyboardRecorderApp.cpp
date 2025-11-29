#include <Windows.h>
#include "../../../w32oop/w32use.hpp"
#include "arg.h"
#include "start.hpp"
#include "select.hpp"
#include "record.hpp"
#include "replay.hpp"
#include <iostream>
using namespace std;

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "Comctl32.lib")

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argv == NULL) {
		return GetLastError();
	}
	w32oop::util::RAIIHelper FreeArgs([] {LocalFree(argv); });

	if (argc < 2) {
		return start(L"app select", app_path().c_str());
	}

	wstring verb = argv[1];

	if (verb == L"select") {
		return app::Select().runner()->run();
	}

	if (verb == L"record") {
		return app::Record().runner()->run();
	}

	if (verb == L"replay") {
		return app::Replay().runner()->run();
	}
	
	cout << "Application Help" << endl <<
		"Usage:\n\tapp VERB [options...]" << endl << endl <<
		"Verb:\n" <<
		"\tselect\tOpen mode selector\n"
		"\trecord\tOpen recorder\n"
		"\treplay\tOpen replayer\n"
		<< endl;
	return (verb == L"help") ? 0 : 87;
}

