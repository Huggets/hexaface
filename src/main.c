#ifdef HXF_WIN32
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <shlwapi.h>
#include "app.h"

/**
 * @brief Handle the parameters given to the command line to convert them to HxfAppParam.
 * 
 * @param pCmdLine The window command line arguments.
 * @param appParam A pointer to the HxfAppParam that will receive the arguments.
 */
void handleParamaters(HxfAppParam* restrict appParam) {
    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc == 2) {
        HXF_MSG_ERROR("Window width specified but not the window height");
        exit(EXIT_FAILURE);
    }
    if (argc > 2) {
        appParam->windowWidth = StrToInt(argv[1]);
        appParam->windowHeight = StrToInt(argv[2]);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    HxfAppParam param = {
        hInstance, nCmdShow, 800, 600
    };

    handleParamaters(&param);

    hxfStartApp(&param);
    
    return 0;
}

#endif