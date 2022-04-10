#include "app.h"
#include "engine/input-handler.h"
#include "engine/game-handler.h"

#include <time.h>
#include <math.h>

static void mainLoop(HxfAppData* restrict app) {
    const clock_t startClock = clock();
    clock_t currentClock = startClock;

    while (app->run) {
        clock_t lastClock = currentClock; // Get the last clock
        currentClock = clock(); // Update the current clock
        app->frameDuration = (float)(currentClock - lastClock) / (float)CLOCKS_PER_SEC; // Time duration of the last frame (in seconds)

        hxfReadWindowMessages(&app->mainWindow);
        hxfHandleInput(app);
        hxfGameFrame(&app->game);
        hxfEngineFrame(&app->engine);

        app->run = !app->mainWindow.shouldDestroyed;
    }
}

void hxfStartApp(const HxfAppParam* restrict param) {
    // Set the default app data

    HxfAppData app = {
        .game.engine = &app.engine,
        .engine.keyboardState = &app.keyboardState,
        .engine.camera = &app.game.camera,
        .engine.world = &app.game.world,
        .engine.drawingData = {
            .cubesVertices = {
                { 0.0f, 1.0f, 0.0f },
                { 0.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f },
                { 1.0f, 0.0f, 0.0f }
            },
            .cubesVertexIndices = {
                0, 1, 2, 2, 3, 0, // Top
                1, 5, 6, 6, 2, 1, // Back
                5, 4, 7, 7, 6, 5, // Bottom
                4, 0, 3, 3, 7, 4, // Front
                2, 6, 7, 7, 3, 2, // Right
                0, 4, 5, 5, 1, 0  // Left
            },
            .ubo = {
                .model = HXF_MAT4_IDENTITY,
                .view = HXF_MAT4_IDENTITY,
                .projection = hxfPerspectiveProjectionMatrix(0.01f, 128.0f, M_PI / 180.0f * 60.0f, (float)param->windowWidth / (float)param->windowHeight)
            },
            .textures = {
                { 0 }, // Air
                { 135.0f / 255.0f, 64.0f / 255.0f, 13.0f / 255.0f }, // Brown
                { 0.0f, 124.0f / 255.0f, 67.0f / 255.0f }, // Green
            }
        },
        .game.camera = {
            .position = { 0.0f, 3.0f, -2.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .yaw = 0.0f,
            .pitch = 0.0f,
        },
        .run = 1,
    };

    // Create the main window

    HxfWindowParam windowParameter = {
        param->hInstance,
        param->nCmdShow,
        param->windowWidth,
        param->windowHeight
    };
    hxfCreateMainWindow(&windowParameter, &app.mainWindow);
    app.engine.mainWindow = &app.mainWindow;

    // Initialization

    hxfInitInput(&app);
    hxfInitGame(&app.game);
    hxfInitEngine(&app.engine);

    // Run the main loop

    mainLoop(&app);

    // Stop the application

    hxfStopEngine(&app.engine);
    hxfStopGame(&app.game);
    hxfDestroyEngine(&app.engine);
    hxfDestroyMainWindow(&app.mainWindow);
}