#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include "hxf.h"

#ifdef __linux
#include <vulkan/vulkan_xlib.h>
#endif

HxfResult hxfCreateWindow(HxfWindow * window) {
#ifdef __linux
    // Get the default display
    window->xdisplay = XOpenDisplay(NULL);
    if (window->xdisplay == NULL) {
        return HXF_WINDOW_CREATION_ERROR;
    }

    // Create the X11 window
    window->xscreenNumber = DefaultScreen(window->xdisplay);
    window->xwindow = XCreateWindow(
        window->xdisplay, RootWindow(window->xdisplay, window->xscreenNumber), 0, 0,
        HXF_WINDOW_WIDTH, HXF_WINDOW_HEIGHT, 1, CopyFromParent, InputOutput, CopyFromParent, 0, NULL);

    XSelectInput(window->xdisplay, window->xwindow, KeyPressMask | KeyReleaseMask);
    XMapWindow(window->xdisplay, window->xwindow);
    XClearWindow(window->xdisplay, window->xwindow);
#endif
    return HXF_SUCCESS;
}

void hxfDestroyWindow(HxfWindow * window) {
#ifdef __linux
    XDestroyWindow(window->xdisplay, window->xwindow);
    XCloseDisplay(window->xdisplay);
    window->xdisplay = NULL;
    window->xscreenNumber = -1;
#endif
}

int hxfPendingEvents(HxfWindow * window) {
#ifdef __linux
    XFlush(window->xdisplay);
    return XQLength(window->xdisplay);
#else
    return 0;
#endif
}

void hxfReadNextEvent(HxfWindow * window, HxfEvent * event) {
#ifdef __linux
    XEvent xevent;
    XNextEvent(window->xdisplay, &xevent);

    if (xevent.type == KeyRelease || xevent.type == KeyPress) {
        if (xevent.type == KeyRelease) {
            event->type = HXF_EVENT_TYPE_KEYRELEASE;
        } else {
            event->type = HXF_EVENT_TYPE_KEYPRESS;
        }

        switch (xevent.xkey.keycode) {
        case 9:
            event->key = HXF_EVENT_KEY_ESCAPE;
            break;
        case 65:
            event->key = HXF_EVENT_KEY_SPACE;
            break;
        default:
            event->key = HXF_EVENT_KEY_UNKNOWN;
        }
    }
#endif
}

HxfResult hxfGetRequiredWindowVulkanExtension(char *** extensions, u_int32_t * count) {
#ifdef __linux
    *count = 2;
    *extensions = hxfMalloc(sizeof(char *) * (*count));

    (*extensions)[0] = "VK_KHR_surface";
    (*extensions)[1] = "VK_KHR_xlib_surface";
#else
    (*extensions) = NULL;
    count = 0;
#endif

    return HXF_SUCCESS;
}

HxfResult hxfCreateVulkanSurface(HxfWindow * window, VkInstance instance, VkSurfaceKHR * surface) {
#ifdef __linux
    VkXlibSurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.dpy = window->xdisplay;
    surfaceCreateInfo.window = window->xwindow;

    if (vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, NULL, surface)) {
        return HXF_ERROR;
    }

    return HXF_SUCCESS;
#endif
}