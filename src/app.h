#pragma once

#if defined(HXF_WIN32)
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#endif

#include "window.h"
#include "input.h"
#include "camera.h"
#include "engine/vulkan.h"

/**
 * @struct HxfAppData
 *
 * @brief The data of the application.
 */
typedef struct HxfAppData {
    /**
     * @brief The main window of the application.
     */
    HxfWindow mainWindow;

    /**
     * @brief The engine of the application that do all the computing.
     *
     * It includes the Vulkan engine, that draw the images.
     */
    HxfEngine engine;

    /**
     * @brief The state of the keyboard (if a key is down or not).
     */
    HxfKeyboardState keyboardState;

    /**
     * @brief The camera where the scene is viewed by the player.
     */
    HxfCamera camera;

    /**
     * @brief If set 0, it indicates that the app should stop.
     */
    int run;

    /**
     * @brief The duration (in seconds) of the last frame.
     */
    float frameDuration;
} HxfAppData;

/**
 * @struct HxfAppParam
 * @brief The parameter that are given to start the application.
 */
typedef struct HxfAppParam {
#if defined(HXF_WIN32)
    /**
     * @brief The HINSTANCE parameter that is given in the main function.
     */
    HINSTANCE hInstance;

    /**
     * @brief The nCmdShow parameter that is given in the main function.
     */
    int nCmdShow;

    int windowWidth;
    int windowHeight;
#endif
} HxfAppParam;

/**
 * @brief Launch the application.
 *
 * @param param The paramater given to the application.
 */
void hxfStartApp(const HxfAppParam* restrict param);