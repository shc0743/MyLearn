#include <Windows.h>
#include "GlobalRef.h"



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    CmdLineW cl(GetCommandLineW());

    if (cl.getopt(L"help") || cl.argc() < 2) {
        return AppHelp();
    }



    return 0;
}
