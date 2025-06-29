#include <iostream>
#include <w32use.hpp>
#include "../../resource/tool.h"

#pragma comment(linker, "/entry:mainCRTStartup")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class ApplicationForwarderWindow :public Window {
public:
    ApplicationForwarderWindow() : Window(L"Application Forwarder", 640, 480, 0, 0, WS_OVERLAPPEDWINDOW) {};

protected:
    Edit appName, target;
    Button createBtn, deleteBtn;
    Edit logBox;

    void onCreated() {
        appName.set_parent(this);
        appName.create(L"Original App Name", 600, 25, 10, 10);

        target.set_parent(this);
        target.create(L"Target App Cmd-Line (Use %1 to refer arguments)", 600, 25, 10, 40);

        createBtn.set_parent(this);
        createBtn.create(L"Create", 100, 25, 10, 70, Button::STYLE | BS_DEFPUSHBUTTON);

        deleteBtn.set_parent(this);
        deleteBtn.create(L"Delete", 100, 25, 120, 70);

        logBox.set_parent(this);
        logBox.create(L"", 600, 240, 10, 110, Edit::STYLE | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL);
        logBox.readonly(true);
        logBox.text(L"Logs will show here");

        createBtn.onClick([this](EventData& ev) { 
            if (target.text().empty()) {
                MessageBoxW(hwnd, L"Please enter a target command line", L"Error", MB_OK | MB_ICONERROR);
                return;
            }
            if (appName.text().empty()) {
                MessageBoxW(hwnd, L"Please enter original app", L"Error", MB_OK | MB_ICONERROR);
                return;
            }

        });
    }

protected:
    void setup_event_handlers() override {

    }
};

int main() {
    CmdLineW cl(GetCommandLineW());

    if (cl.getopt(L"x")) {

    }

    // GUI
    if (!IsRunAsAdmin()) {
        ShellExecuteW(NULL, L"runas", GetProgramDirW().c_str(), L"", L"", SW_SHOW);
        return 0;
    }

    ApplicationForwarderWindow w;
    w.create();
    w.set_main_window();
    w.show();
    w.center();
    return w.run();
}
