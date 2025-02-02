#define UNICODE 1
#define _UNICODE 1
#include <Windows.h>
#include "tool.h"
#include "tool.cpp"
#include <ranges>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Shell32.lib")
#if !defined( _HAS_CXX20 ) || !_HAS_CXX20
#error CXX20 is required to compile the file (std::basic_string::starts_with)
#endif

using namespace std;

int main() {
	CmdLineW cl(GetCommandLineW());
	if (cl.getopt(L"help")) {
		cout <<
			"UUID Generate Tool (mkuuid) Help\n"
			"                @github/chcs1013\n"
			"================================\n"
			"Usage:\n\tmkuuid[ (-v|--version)][ --help][ --support][ -cCOUNT][ (--nd|--no-delims)]\n[ (-u|--uppercase)][ (-s|--starts-with=)START[ (--nc|--no-count)]][ --ui]\n"
			"COUNT\t\t\tThe number of UUIDs to generate. Default is 1.\n"
			"START\t\t\tThe start of the output string. Default is empty.\n"
			"--help\t\t\tShow the help documentation.\n"
			"-v, --version\t\tShow the version information.\n"
			"--support\t\tShow the support webpage.\n"
			"--nd, --no-delims\tDo not add \"-\" to the UUID.\n"
			"--starts-with\t\tKeep making UUIDs until the result matches the given pattern.\n"
			"--nc, --no-count\tWhen using --starts-with, we will show the total UUIDs that we\n\t\t\tgenerated. To suppress this behavior, you can use --no-count.\n"
			"-u, --uppercase\t\tOpen a simple user interface to make UUIDs.\n"
			"--ui\t\t\t*NOT APPLICABLE* Open a simple user interface to make UUIDs.\n"
			"\nExample:\n"
			"mkuuid\n\t9ad26153-72c0-498f-9998-d04c165b61a0\n"
			"mkuuid -c3\n\t22f687cd-8114-45ae-99f4-4f6a7ff66a7c\n\t1d5d1ac0-ef96-4022-8beb-d4e5e8411a12\n\t47a8c6ac-d4f0-4ab3-a91f-f49c29ce4bc6\n"
			"mkuuid --no-delims\n\ta8c27da5062e4a38914b9d391928d8bb\n"
			"mkuuid --starts-with=1234\n\t1234560e-bf8e-4bfe-a46e-217e91be34d5\n\n\tTotal generated: 116393\n"
			"mkuuid --starts-with=ff -c3 --no-delims\n\tff30d16c0deb4dbebfff9c432910e4b9\n\tffa3228002044f68a342360269c0c9e9\n\tffd943cea3b9492a841407e70671cb9b\n\n\tTotal generated: 392\n"
			"cmd.exe /c mkuuid|clip\n\tCreate an UUID and copy it to clipboard.\n"
			"\nFor version informations, use -v or --version.\n"
			<< endl;
		return 0;
	}
	if (cl.getopt(L"v") || cl.getopt(L"version")) {
		cout <<
			"UUID Generate Tool (mkuuid)\n"
			"Version: 1.4.0.1\n"
			"Build Date: " << __TIMESTAMP__ << "\n"
			"File Path: " << __FILE__ << "#" << __LINE__ << "\n"
			"Build Command:  cl mkuuid.cpp /std:c++20 /EHsc\n"
			"For help, use --help\n"
			<< endl;
		return 0;
	}
	if (cl.getopt(L"support")) {
		ShellExecuteW(NULL, L"open", L"https://github.com/shc0743/MyLearn/tree/main/resource/mkuuid.cpp", NULL, NULL, SW_NORMAL);
		return 0;
	}
	wstring scount, starts_with;
	signed long long count = 1;
	std::string(*fn)() = GenerateUUID;
	if (1 == cl.getopt(L"c", scount)) {
		count = atoll(ws2c(scount));
		if (count < 1) count = 1;
	}
	if (cl.getopt(L"no-delims") || cl.getopt(L"nd")) {
		fn = GenerateUUIDWithoutDelim;
	}
	if (!cl.getopt(L"starts-with", starts_with))
		cl.getopt(L"s", starts_with);
	bool uppercase = cl.getopt(L"uppercase") || cl.getopt(L"u");
	unsigned long long total_generated = 0;
	for (signed long long i = 0; i < count; ++i, ++total_generated) {
		auto result = fn();
		if (starts_with.empty() == false) {
			if (!s2ws(result).starts_with(starts_with)) {
				--i;
				continue;
			}
		}
		if (uppercase) {
			std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return std::toupper(c); });
		}
		puts(result.c_str());
	}
	if (starts_with.empty() == false && cl.getopt(L"nc") == 0 && cl.getopt(L"no-count") == 0) {
		std::cout << endl << "Total generated: " << total_generated << endl;
	}
	return 0;
}

