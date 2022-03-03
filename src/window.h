/**
 * \file window.h
 * \brief Plateform independant code for windows.
 */
#pragma once

#ifdef __linux
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
 * \struct HxfWindow
 * \brief A window that can be used to draw things on.
 */
typedef struct HxfWindow {
#ifdef __linux
    Display * xdisplay;
    Window xwindow;
    int xscreenNumber;
#endif
} HxfWindow;

/**
 * \brief Initializes and creates the window.
 * \param window The window to initialize.
 * \return HXF_WINDOW_CREATION_ERROR if the window could not be created.\n
 * HXF_SUCCESS otherwise
 */
HxfResult hxfCreateWindow(HxfWindow * window);

/**
 * \brief Destroys the window.
 * \param window The window to destroy.
 */
void hxfDestroyWindow(HxfWindow * window);

/**
 * \brief Return the number of pending events of the window.
 * \param window The window from which events are counted.
 * \return The number of pending events.
 */
int hxfPendingEvents(HxfWindow * window);

/**
 * \brief Reads the next event.
 * \param window The window from which the next event is read.
 * \param event The event that is read.
 */
void hxfReadNextEvent(HxfWindow * window, HxfEvent * event);

/**
 * \brief Gets the extensions needed by vulkan to work with windows.
 * \param extensions A pointer to an array of char strings containing the required extensions.
 * \param count The size of the array.
 * \return HXF_SUCCESS.
 *
 * Note: extensions must be freed when it is not needed anymore.
 */
HxfResult hxfGetRequiredWindowVulkanExtension(char *** extensions, u_int32_t * count);

/**
 * \brief Create a Vulkan surface from the window.
 * \param window The window from which we want to create the surface.
 * \param instance The Vulkan instance that hold the surface.
 * \param surface The created surface.
 * 
 * \return HXF_ERROR if an error occurs during the creation.\n
 * HXF_SUCCESS if nothing went wrong.
 */
HxfResult hxfCreateVulkanSurface(HxfWindow * window, VkInstance instance, VkSurfaceKHR * surface);