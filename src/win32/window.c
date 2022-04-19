#include "window.h"

/**
 * @brief The window procedure of the main window.
 */
static LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
    case WM_DESTROY:
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

HxfResult hxfCreateMainWindow(const HxfWindowParam* restrict param, HxfWindow* window) {
    const wchar_t CLASS_NAME[] = L"Window Class";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = mainWindowProc;
    wc.hInstance = param->hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    window->width = param->width;
    window->height = param->height;
    window->hInstance = param->hInstance;
    window->hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Main Window",
        WS_OVERLAPPED,
        CW_USEDEFAULT, CW_USEDEFAULT, param->width, param->height,
        NULL,
        NULL,
        param->hInstance,
        NULL
    );

    if (window->hwnd == NULL) {
        return HXF_WINDOW_CREATION_ERROR;
    }

    ShowWindow(window->hwnd, param->nCmdShow);

    return HXF_SUCCESS;
}

HxfResult hxfDestroyMainWindow(HxfWindow* restrict window) {
    if (DestroyWindow(window->hwnd) == TRUE) {
        return HXF_SUCCESS;
    }
    else {
        return HXF_ERROR;
    }
}

void hxfReadWindowMessages(HxfWindow* restrict window) {
    MSG msg = { 0 };
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        // Make the window procedure handle the message
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Additional handling
        switch (msg.message) {
        case WM_DESTROY:
            window->shouldDestroyed = 1;
            break;
        case WM_KEYDOWN:
            // Do nothing if the key was already down the last time
            if ((msg.lParam & 0x40000000) == 0x40000000) { break; }

            int scancode = (msg.lParam & 0x00FF0000) >> 16;
            window->keyDownCallback[scancode](window->keyDownCallbackParameter[scancode]);
        case WM_KEYUP:
            // Do nothing if the key was already up the last time
            if ((msg.lParam & 0x40000000) != 0x40000000) { break; }

            scancode = (msg.lParam & 0x00FF0000) >> 16;
            window->keyUpCallback[scancode](window->keyUpCallbackParameter[scancode]);
            break;
        }
    }
}

void hxfCreateWindowSurface(HxfWindow* restrict window, VkInstance instance, VkSurfaceKHR* restrict surface) {
    VkWin32SurfaceCreateInfoKHR surfaceInfo = { 0 };
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = window->hInstance;
    surfaceInfo.hwnd = window->hwnd;

    if (vkCreateWin32SurfaceKHR(instance, &surfaceInfo, NULL, surface)) {
        HXF_MSG_ERROR("Could not create a win32 surface");
        exit(EXIT_FAILURE);
    }
}

void hxfGetRequiredWindowExtensions(char*** extensions, uint32_t* count) {
    *extensions = hxfMalloc(2 * sizeof(char*));
    (*extensions)[0] = "VK_KHR_surface";
    (*extensions)[1] = "VK_KHR_win32_surface";

    *count = 2;
}

void hxfSetKeyDownCallback(HxfWindow* window, HxfKeyCode scancode, void (*function)(void*), void* param) {
    window->keyDownCallback[scancode] = function;
    window->keyDownCallbackParameter[scancode] = param;
}

void hxfSetKeyUpCallback(HxfWindow* window, HxfKeyCode scancode, void (*function)(void*), void* param) {
    window->keyUpCallback[scancode] = function;
    window->keyUpCallbackParameter[scancode] = param;
}