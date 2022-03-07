#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include "hxf.h"

#if defined(HXF_WINDOW_XLIB)
#include <vulkan/vulkan_xlib.h>
#endif

HxfResult hxfCreateWindow(HxfWindow * window) {
#if defined(HXF_WINDOW_XLIB)
    // Get the default display
    window->xdisplay = XOpenDisplay(NULL);
    if (window->xdisplay == NULL) {
        return HXF_WINDOW_CREATION_ERROR;
    }

    // Create the X11 window
    window->xscreenNumber = DefaultScreen(window->xdisplay);
    window->xwindow = XCreateWindow(
        window->xdisplay, RootWindow(window->xdisplay, window->xscreenNumber), 0, 0,
        HXF_WINDOW_WIDTH, HXF_WINDOW_HEIGHT, 50, CopyFromParent, InputOutput, CopyFromParent, 0, NULL);

    // Allow to handle the close button
    window->wm_protocols = XInternAtom(window->xdisplay, "WM_PROTOCOLS", False);
    window->wm_delete_window = XInternAtom(window->xdisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(window->xdisplay, window->xwindow, &window->wm_delete_window, 1);

    XAutoRepeatOff(window->xdisplay); // Disable auto repetition off keyboard keys
    XSelectInput(window->xdisplay, window->xwindow, KeyPressMask | KeyReleaseMask);
    XMapWindow(window->xdisplay, window->xwindow);

    window->info.height = HXF_WINDOW_HEIGHT;
    window->info.width = HXF_WINDOW_WIDTH;

    window->keysState = (HxfKeysState){0};
#endif
    return HXF_SUCCESS;
}

void hxfDestroyWindow(HxfWindow * window) {
#if defined(HXF_WINDOW_XLIB)
    XDestroyWindow(window->xdisplay, window->xwindow);
    XCloseDisplay(window->xdisplay);
    window->xdisplay = NULL;
    window->xscreenNumber = -1;
#endif
}

int hxfHasPendingEvents(HxfWindow * window) {
#if defined(HXF_WINDOW_XLIB)
    // For xlib the return value is the number of pending events
    XFlush(window->xdisplay);
    return XQLength(window->xdisplay);
#else
    return 0;
#endif
}

/**
 * \brief Set the event data to keycode.
 */
static void setEventKey(HxfEvent * event, unsigned int keycode) {
    switch (keycode) {
    case 9:
        event->data = HXF_EVENT_KEY_ESCAPE;
        break;
    case 65:
        event->data = HXF_EVENT_KEY_SPACE;
        break;
    case 111:
        event->data = HXF_EVENT_KEY_ARROW_UP;
        break;
    case 116:
        event->data = HXF_EVENT_KEY_ARROW_DOWN;
        break;
    case 113:
        event->data = HXF_EVENT_KEY_ARROW_LEFT;
        break;
    case 114:
        event->data = HXF_EVENT_KEY_ARROW_RIGHT;
        break;
    default:
        event->data = HXF_EVENT_KEY_UNKNOWN;
    }
}

static void setKeyState(HxfKeysState * keyState, HxfEvent * event) {
    const unsigned int state = event->type == HXF_EVENT_TYPE_KEYPRESS ? 1 : 0;

    switch (event->data)
    {
    case HXF_EVENT_KEY_ESCAPE:
        keyState->escape = state;
        break;
    case HXF_EVENT_KEY_SPACE:
        keyState->space = state;
        break;
    case HXF_EVENT_KEY_ARROW_UP:
        keyState->arrowUp = state;
        break;
    case HXF_EVENT_KEY_ARROW_DOWN:
        keyState->arrowDown = state;
        break;
    case HXF_EVENT_KEY_ARROW_LEFT:
        keyState->arrowLeft = state;
        break;
    case HXF_EVENT_KEY_ARROW_RIGHT:
        keyState->arrowRight = state;
        break;
    }
}

void hxfGetNextEvent(HxfWindow * window, HxfEvent * event) {
#if defined(HXF_WINDOW_XLIB)
    XEvent xevent;
    XNextEvent(window->xdisplay, &xevent);

    switch (xevent.type) {
    case KeyPress:
        event->type = HXF_EVENT_TYPE_KEYPRESS;
        setEventKey(event, xevent.xkey.keycode);
        setKeyState(&window->keysState, event);
        break;
    case KeyRelease:
        event->type = HXF_EVENT_TYPE_KEYRELEASE;
        setEventKey(event, xevent.xkey.keycode);
        setKeyState(&window->keysState, event);
        break;
    case ClientMessage:
        // This can happen when the window manager ask to close the window (because of alt-f4 or the close
        // button for example)
        if (xevent.xclient.message_type == window->wm_protocols && xevent.xclient.data.l[0] == window->wm_delete_window) {
            event->type = HXF_EVENT_TYPE_WINDOW_SHOULD_CLOSE;
        }
    }
#endif
}

HxfResult hxfGetRequiredWindowVulkanExtension(char *** extensions, u_int32_t * count) {
#if defined(HXF_WINDOW_XLIB)
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
#if defined(HXF_WINDOW_XLIB)
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

void hxfGetWindowSize(HxfWindow * window, unsigned int * width, unsigned int * height) {
#if defined(HXF_WINDOW_XLIB)
    *width = HXF_WINDOW_WIDTH;
    *height = HXF_WINDOW_HEIGHT;
#endif
}

HxfWindowInformation * hxfGetWindowInformation(HxfWindow * window) {
    XWindowAttributes * attributes = NULL;
    XGetWindowAttributes(window->xdisplay, window->xwindow, attributes);
    window->info.height = attributes->height;
    window->info.width = attributes->width;

    return &window->info;
}