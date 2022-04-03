#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>
#include <vulkan/vulkan.h>
#include "../hxf.h"
#include "../window.h"

struct HxfWindowParam {
    HINSTANCE hInstance;
    int nCmdShow;
    int width;
    int height;
};

struct HxfWindow {
    HWND hwnd;
    HINSTANCE hInstance;
    int width;
    int height;
    /**
     * @brief An array of callback function that are called when a key is pressed.
     *
     * The index corresponds to the scancode.
     */
    void (*keyDownCallback[HXF_WINDOW_KEY_CALLBACK_COUNT])(void*);
    void* keyDownCallbackParameter[HXF_WINDOW_KEY_CALLBACK_COUNT];
    /**
     * @brief An array of callback function that are called when a key is released.
     *
     * The index corresponds to the scancode.
     */
    void (*keyUpCallback[HXF_WINDOW_KEY_CALLBACK_COUNT])(void*);
    void* keyUpCallbackParameter[HXF_WINDOW_KEY_CALLBACK_COUNT];
    
    int shouldDestroyed; ///< Indicates that the window should be destroyed
};