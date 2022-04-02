#ifdef HXF_WIN32
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "app.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    HxfAppParam param = {
        hInstance, nCmdShow
    };
    
    hxfStartApp(&param);
    
    return 0;
}

#endif