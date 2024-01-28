#define UNICODE 1
#define _UNICODE 1
#include <Windows.h>
#include "tool.h"
#include "tool.cpp"
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "ole32.lib")

using namespace std;

int main() {
	CmdLineW cl(GetCommandLineW());
	if (cl.getopt(L"help")) {
		cout <<
			"UUID Generate Tool (mkuuid) Help\n"
			"================================\n"
			"Usage:\n\tmkuuid[ (-v|--version)][ --help][ -cCOUNT][ --no-delims]\n"
			"COUNT\t\tThe number of UUIDs to generate. Default is 1.\n"
			"--no-delims\tDo not add \"-\" to the UUID.\n"
			"\nExample:\n"
			"mkuuid\n\t9ad26153-72c0-498f-9998-d04c165b61a0\n"
			"mkuuid -c3\n\t22f687cd-8114-45ae-99f4-4f6a7ff66a7c\n\t1d5d1ac0-ef96-4022-8beb-d4e5e8411a12\n\t47a8c6ac-d4f0-4ab3-a91f-f49c29ce4bc6\n"
			"mkuuid --no-delims\n\ta8c27da5062e4a38914b9d391928d8bb\n"
			"cmd.exe /c mkuuid|clip\n\tCreate an UUID and copy it to clipboard.\n"
			"\nFor version informations, use -v or --version.\n"
			<< endl;
		return 0;
	}
	if (cl.getopt(L"v") || cl.getopt(L"version")) {
		cout <<
			"UUID Generate Tool (mkuuid)\n"
			"Version: 1.3.0.2\n"
			"Build Date: " << __TIMESTAMP__ << "\n"
			"File Path: " << __FILE__ << "#" << __LINE__ << "\n"
			"Build Command:  cl mkuuid.cpp /std:c++20 /EHsc\n"
			"For help, use --help\n"
			<< endl;
		return 0;
	}
	wstring scount;
	signed long long count = 1;
	std::string(*fn)() = GenerateUUID;
	if (1 == cl.getopt(L"c", scount)) {
		count = atoll(ws2c(scount));
		if (count < 1) count = 1;
	}
	if (cl.getopt(L"no-delims")) {
		fn = GenerateUUIDWithoutDelim;
	}
	for (signed long long i = 0; i < count; ++i) {
		puts(fn().c_str());
	}
	return 0;
}

