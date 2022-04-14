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
#include "engine/graphics-handler.h"
#include "engine/game-handler.h"

/**
 * @struct HxfAppData
 * @brief The application data.
 */
typedef struct HxfAppData {
    HxfWindow mainWindow; ///< The main window of the application
    HxfKeyboardState keyboardState; ///< The keyboard state
    HxfGameData game; ///< The game data
    HxfGraphicsHandler graphics; ///< The graphics handler.

    int run; ///< If set to 0, it indicates that the app should stop.
    float frameDuration; ///< The duration (in seconds) of the last frame.
    char* appdataDirectory; ///< The path to the appdataDirectory.
} HxfAppData;

/**
 * @struct HxfAppParam
 * @brief The parameter that are given to start the application.
 */
typedef struct HxfAppParam {
#if defined(HXF_WIN32)
    HINSTANCE hInstance; ///< The HINSTANCE parameter that is given in the main function.
    int nCmdShow; ///< The nCmdShow parameter that is given in the main function.
#endif
    int windowWidth; ///< The main window’s width.
    int windowHeight; ///< The main window’s height.
    char* appDataDirectory; ///< The path to the appdata directory.
} HxfAppParam;

/**
 * @brief Launch the application with the given parameters.
 */
void hxfAppStart(const HxfAppParam* restrict param);