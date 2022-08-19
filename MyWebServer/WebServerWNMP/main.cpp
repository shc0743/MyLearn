#include <Windows.h>
#include "../../resource/tool.h"
#include "service.h"

#undef echo


#pragma comment(linker, "/entry:mainCRTStartup")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int main(int argc, char* argv[]) {
	if (argc < 2) {
		if(!AttachConsole(ATTACH_PARENT_PROCESS))
			if (!AllocConsole()) {
				MessageBoxTimeoutW(0, L"Error creating and attaching console!",
					NULL, MB_ICONHAND, 0, 2000);
				return ERROR|1;
			}
		HANDLE
			i = GetStdHandle(STD_INPUT_HANDLE),
			o = GetStdHandle(STD_OUTPUT_HANDLE);
		char buffer[4096]{};
		auto echo = [o](std::string str) {
			DWORD temp = 0;
			return WriteConsoleA(o, str.c_str(), (DWORD)str.length(), &temp, 0);
		};
		auto read = [i, &buffer](){
			memset(buffer, 0, sizeof(buffer));
			DWORD temp = 0;
			ReadConsoleA(i, buffer, DWORD(sizeof(buffer)/sizeof(buffer[0])), &temp, NULL);
			return buffer;
		};

		echo("Web Server (WNMP) Setup\n"
			"=======================\n"
			"0. Exit\n"
			"1. Create new server service\n"
			"2. Delete an existing service\n"
			"\nYour choice: "
		);

		while ((buffer[0] = _getch())) {
			if (
				buffer[0] == '0' ||
				buffer[0] == '1' ||
				buffer[0] == '2'
			) break;
			echo("\a");
		}

		buffer[1] = '\n';
		echo(buffer);
		echo("=======================\n");

		using namespace std;
		switch (buffer[0]) {
		case '0':
			return 0;

		case '1': {
			echo("Windows service name: ");
			string svc = read();
			if (svc.length() < 3) return ERROR_CANCELLED;
			echo("nginx command line: ");
			string nginx = read();
			if (nginx.length() < 3) return ERROR_CANCELLED;
			echo("MySQL service name: ");
			string mysql = read();
			if (mysql.length() < 3) return ERROR_CANCELLED;
			echo("php command line: ");
			string php = read();
			if (php.length() < 3) return ERROR_CANCELLED;
			echo("Service startup type ([A]uto|[D]emand,default:D): ");
			while ((buffer[0] = _getch())) {
				if (buffer[0] == 'A' || buffer[0] == 'D') break;
				if (buffer[0] == 'a' || buffer[0] == 'd') break;
				if (buffer[0] == '\r' || buffer[0] == '\n')	break;
				echo("\a");
			}
			string startup_type =
				(buffer[0] == 'A' || buffer[0] == 'a') ? "a" : "d";

			for (auto& i : string("\r\n")) {
				str_replace(svc, string(1, i), "");
				str_replace(nginx, string(1, i), "");
				str_replace(mysql, string(1, i), "");
				str_replace(php, string(1, i), "");
			}

			echo(startup_type + "\n");
			echo("=======================\n");

			string cmdl = "-N -W\"" + svc + "\" -n\"" + nginx +
				"\" -m\"" + mysql + "\" -p\"" + php + "\" -S" + startup_type;

			if (IsRunAsAdmin()) {
				cmdl = "\"" + GetProgramDir() + "\" " + cmdl;
				DWORD code = 1;
				auto pi = Process.Start_HiddenWindow(to__str(cmdl));
				if (!pi.hProcess) return GetLastError();
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hThread);
				GetExitCodeProcess(pi.hProcess, &code);
				CloseHandle(pi.hProcess);

				if (code == 0) echo("Successfully.\n");
				else echo("Operation failed.\n");

				echo("Press any key to continue...\n");
				pause;
				return code;
			}
			else {
				SHELLEXECUTEINFO execinfo{ 0 };
				std::wstring pd = s2ws(GetProgramDir());
				std::wstring cl = s2ws(cmdl);
				execinfo.lpFile = pd.c_str();
				execinfo.cbSize = sizeof(execinfo);
				execinfo.lpVerb = L"runas";
				execinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				execinfo.nShow = SW_SHOWDEFAULT;
				execinfo.lpParameters = cl.c_str();
				execinfo.hwnd = GetConsoleWindow();

				if (!ShellExecuteExW(&execinfo)) {
					echo(ErrorCodeToStringA(ERROR_CANCELLED) + "\n");
					return ERROR_CANCELLED;
				}

				DWORD code = 1;
				if (execinfo.hProcess) {
					WaitForSingleObject(execinfo.hProcess, INFINITE);
					GetExitCodeProcess(execinfo.hProcess, &code);
					CloseHandle(execinfo.hProcess);
				}

				if (code == 0) echo("Successfully.\n");
				else echo("Operation failed.\n");

				echo("Press any key to continue...\n");
				pause;
				return code;
			}

			break;
		}

		case '2': {
			echo("Windows service to delete: ");
			string svc = read();
			if (svc.length() < 3) return ERROR_CANCELLED;

			str_replace(svc, "\r", "");
			str_replace(svc, "\n", "");

			if (ServiceManager.Remove(svc) == 0) {
				echo("Success.\n");
			}
			else {
				echo("Fail.\n");
			}

			echo("Press any key to continue...\n");
			pause
			break;
		}

		default:
			return -1;
		}

		return 0;
	}

	CmdLineA cl(GetCommandLineA());

	using namespace std;
	if (cl.getopt("N")) {
		string svc, n, m, p, startup_type;
		cl.getopt("W", svc);
		cl.getopt("n", n);
		cl.getopt("m", m);
		cl.getopt("p", p);
		cl.getopt("S", startup_type);
		if (svc.empty() || n.empty() || m.empty() || p.empty())
			return ERROR_INVALID_PARAMETER;
		if (startup_type != "a") startup_type = "d";

		string svccmd = "\"" + GetProgramDir() + "\" --service -W\"" + svc +
			"\" -n\"" + n + "\" -m\"" + m + "\" -p\"" + p + "\"";
		//MessageBoxA(0, svccmd.c_str(), 0, 0);
		auto result = ServiceManager.New(s2ws(svc), s2ws(svccmd),
			(startup_type == "d") ?	ServiceManager.Manual : ServiceManager.Auto,
			s2ws(svc), L"Web Server Service", SERVICE_WIN32_OWN_PROCESS);
		return result;
	}
	else if (cl.getopt("d")) {
		string svc;
		cl.getopt("W", svc);
		if (svc.empty()) return ERROR_INVALID_PARAMETER;

		return ServiceManager.Remove(svc);
	}

	if (cl.getopt("service")) {
		string svc, n, m, p;
		cl.getopt("W", svc);
		cl.getopt("n", n);
		cl.getopt("m", m);
		cl.getopt("p", p);

		global_SvcObj = new ServiceWorker_t;
		wcscpy_s(global_SvcObj->ServiceName, s2wc(svc));
		global_SvcObj->szNginx = s2ws(n);
		global_SvcObj->szPhp = s2ws(p);

		return MyStartAsServiceW(s2ws(svc), global_SvcObj->ServiceLaunch);
	}

	return ERROR_INVALID_PARAMETER;
	return 0;
}


