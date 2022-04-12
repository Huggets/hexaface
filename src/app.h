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
#include "world.h"
#include "engine/vulkan.h"
#include "engine/game-handler.h"

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
     * @brief Contains the game objects.
     */
    HxfGameData game;

    /**
     * @brief If set to 0, it indicates that the app should stop.
     */
    int run;

    /**
     * @brief The duration (in seconds) of the last frame.
     */
    float frameDuration;

    /**
     * @brief The path to the appdataDirectory.
     */
    char* appdataDirectory;
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

    /**
     * @brief The width of the window.
     */
    int windowWidth;
    /**
     * @brief The height of the window.
     */
    int windowHeight;

    /**
     * @brief The path to the appdata directory.
     */
    char* appDataDirectory;
#endif
} HxfAppParam;

/**
 * @brief Launch the application.
 *
 * @param param The paramater given to the application.
 */
void hxfStartApp(const HxfAppParam* restrict param);