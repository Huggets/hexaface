/**
 * \file window.h
 * \brief Plateform independant code for window.
 */
#pragma once

#if defined(HXF_WINDOW_XLIB)
#include "X11/Xlib.h"
#endif

#include "event.h"
#include "hxf.h"
#include <vulkan/vulkan.h>

/**
 * \def HXF_WINDOW_HEIGHT
 * \brief The height of the window.
 */
#define HXF_WINDOW_HEIGHT 600

/**
 * \def HXF_WINDOW_WIDTH
 * \brief The width of the window.
 */
#define HXF_WINDOW_WIDTH 800

/**
 * Information about a window.
 */
typedef struct HxfWindowInformation {
    int width;
    int height;
} HxfWindowInformation;

/**
 * \struct HxfWindow
 * \brief A window that can be used to draw things on.
 * 
 * The members must not be accessed or modified.
 */
typedef struct HxfWindow {
#if defined(HXF_WINDOW_XLIB)
    Display * xdisplay;
    Window xwindow;
    int xscreenNumber;
    Atom wm_protocols;
    Atom wm_delete_window;
    HxfWindowInformation info;
#endif
} HxfWindow;

/**
 * \brief Initialize and create the window.
 * \param window The window to initialize.
 * \return HXF_WINDOW_CREATION_ERROR if the window could not be created.\n
 * HXF_SUCCESS otherwise
 */
HxfResult hxfCreateWindow(HxfWindow * window);

/**
 * \brief Destroy the window.
 * \param window The window to destroy.
 */
void hxfDestroyWindow(HxfWindow * window);

/**
 * \brief Indicate if there are pending events.
 * \param window The window from which events are counted.
 * \return 0 if there are no pending events, a number different than zero if there are pending events.
 */
int hxfHasPendingEvents(HxfWindow * window);

/**
 * \brief Get the next event available.
 * \param window The window from which the next event is read.
 * \param event The event that is read.
 */
void hxfGetNextEvent(HxfWindow * window, HxfEvent * event);

/**
 * \brief Get the extensions needed by vulkan to work with windows.
 * \param extensions A pointer to an array of char strings containing the required extensions.
 * \param count The size of the array.
 * \return HXF_SUCCESS.
 *
 * Note: extensions must be freed when it is not needed anymore.
 */
HxfResult hxfGetRequiredWindowVulkanExtension(char *** extensions, u_int32_t * count);

/**
 * \brief Create a Vulkan surface from the window.
 * \param window The window.
 * \param instance The Vulkan instance that hold the surface.
 * \param surface The created surface.
 * 
 * \return HXF_ERROR if an error occurs during the creation.\n
 * HXF_SUCCESS if nothing went wrong.
 */
HxfResult hxfCreateVulkanSurface(HxfWindow * window, VkInstance instance, VkSurfaceKHR * surface);

/**
 * \brief Get the current size of the window.
 * \param window The window.
 * \param width A pointer to an unsigned that will be set to the window width.
 * \param height A pointer to an unsigned that will be set to the window eight.
 */
void hxfGetWindowSize(HxfWindow * window, unsigned int * width, unsigned int * height);

/**
 * \brief Get information about a window such as the width and height.
 * \param window The window.
 * \return A HxfWindowInformation pointer that gives information about the window.
 */
HxfWindowInformation * hxfGetWindowInformation(HxfWindow * window);