#include "../MyRemoteTool/sciter/include/sciter-x.h"
#include "../MyRemoteTool/sciter/include/sciter-x-window.hpp"
#include "../../resource/tool.h"

class frame : public sciter::window {
public:
    frame() : window(SW_MAIN | SW_ENABLE_DEBUG) { }
};
class frame_nomain : public sciter::window {
public:
    frame_nomain() : window(SW_ENABLE_DEBUG) { }
};

int uimain(std::function<int()> run) {
    CmdLineW cl(GetCommandLineW());

    if (cl.argc() < 2) return ERROR_INVALID_PARAMETER;

    sciter::om::hasset<frame> pmain = new frame();
    pmain->load((cl[1]).c_str());
    pmain->expand();

    for (size_t i = 2; i < cl.argc(); ++i) {
        sciter::om::hasset<frame_nomain> pwin = new frame_nomain();
        pwin->load((cl[i]).c_str());
        pwin->expand();
    }

    return run();
}


