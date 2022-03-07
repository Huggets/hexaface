/**
 * \file window.h
 * \brief Plateform independant code for window and input.
 */
#pragma once

#if defined(HXF_WINDOW_XLIB)
#include "X11/Xlib.h"
#endif

#include "event.h"
#include "hxf.h"
#include <vulkan/vulkan.h>

/**
 * \def HXF_WINDOW_WIDTH
 * \brief The width of the window.
 */
#define HXF_WINDOW_WIDTH 800

/**
 * \def HXF_WINDOW_HEIGHT
 * \brief The height of the window.
 */
#define HXF_WINDOW_HEIGHT 600

/**
 * \struct HxfKeysState
 * \brief Information about the state of keys.
 * 
 * The member variable is set to 0 when the key is released and to 1 when the key is pressed.
 */
typedef struct HxfKeysState {
    int escape; ///< Escape keys
    int space; ///< Spacebar
    int arrowUp; ///< Up arrow
    int arrowDown; ///< Down arrow
    int arrowLeft;
    int arrowRight;
} HxfKeysState;

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
 * The members must not be modified.
 * 
 * Only info and keysState can be accessed.
 */
typedef struct HxfWindow {
#if defined(HXF_WINDOW_XLIB)
    Display * xdisplay;
    Window xwindow;
    int xscreenNumber;
    Atom wm_protocols;
    Atom wm_delete_window;
#endif
    HxfWindowInformation info; ///< You need to call hxfUpdateWindowInformation to get the latest information

    HxfKeysState keysState; ///< The state of the keys
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
void hxfUpdateWindowInformation(HxfWindow * window);