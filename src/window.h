/**
 * @file window.h
 * @brief Window and keyboard/mouse operations
 *
 * Functions to create a window, to handle keyboard input, to handle events.
 */
#pragma once

#include <vulkan/vulkan.h>
#include "hxf.h"
#include "input.h"
#include <stdint.h>

/**
 * @brief The number of keydown and keyup callback of a window.
 */
#define HXF_WINDOW_KEY_CALLBACK_COUNT 256

/**
 * @def struct HxfWindowParam
 * @brief Parameters given to @ref hxfCreateMainWindow to create a window.
 */
typedef struct HxfWindowParam HxfWindowParam;

/**
 * @def struct HxfWindow
 * @brief A window.
 */
typedef struct HxfWindow HxfWindow;

/**
 * @brief Create a window that is the main window.
 *
 * @param param The arguments that are required to create the window.
 * @param window The window that will be created.
 *
 * @return HXF_WINDOW_CREATION_ERROR if the creation fails, HXF_SUCCESS otherwise.
 */
HxfResult hxfCreateMainWindow(const HxfWindowParam* restrict param, HxfWindow* restrict window);

/**
 * @brief Destroy the window.
 *
 * @param window The window to destroy
 *
 * @return HXF_ERROR if it fails, HXF_SUCCESS otherwise.
 */
HxfResult hxfDestroyMainWindow(HxfWindow* restrict window);

/**
 * @brief Read and handle all pending messages of the window.
 *
 * @param window A pointer to the window.
 */
void hxfReadWindowMessages(HxfWindow* restrict window);

/**
 * @brief Create a VkSurfaceKHR from the window, the result is returned in surface.
 *
 * @param window A pointer to the window from which the surface is created.
 * @param instance The vulkan instance that will own the surface.
 * @param surface A pointer to the surface that will be created.
 */
void hxfCreateWindowSurface(HxfWindow* restrict window, VkInstance instance, VkSurfaceKHR* restrict surface);

/**
 * @brief Return the vulkan extensions that are required.
 *
 * The return value is allocated and must be freed.
 *
 * @param extensions A pointer to an array of char string that will be allocated and that will
 * contains the required extensions.
 * @param count A pointer to an int that will contains the number of extensions.
 */
void hxfGetRequiredWindowExtensions(char*** extensions, uint32_t* count);

/**
 * @brief Set the callback function that will be called when the key is pressed down.
 *
 * When the window get a message that the key is pressed down, the function is executed.
 *
 * @param window A pointer to the window that generate the event.
 * @param scancode The scancode of the key.
 * @param function A pointer to the function that will be called. It must be a valid
 * pointer to a valid function.
 * @param param A pointer to the parameter given to that callback function.
 */
void hxfSetKeyDownCallback(HxfWindow* window, HxfKeyCode scancode, void (*function)(void*), void* param);

/**
 * @brief Set the callback function that will be called when the key is released.
 *
 * When the window get a message that the key is released, the function is executed.
 *
 * @param window A pointer to the window that generate the event.
 * @param scancode The scancode of the key.
 * @param function A pointer to the function that will be called. It must be a valid
 * pointer to a valid function.
 * @param param A pointer to the parameter given to that callback function.
 */
void hxfSetKeyUpCallback(HxfWindow* window, HxfKeyCode scancode, void (*function)(void*), void* param);

// The declaration is needed first, before including the rest
#if defined(HXF_WIN32)
#include "win32/window.h"
#endif