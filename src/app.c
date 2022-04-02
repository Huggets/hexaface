#include "app.h"
#include "engine/input-handler.h"

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
        hxfEngineFrame(&app->engine);

        app->run = !app->mainWindow.shouldDestroyed;
    }
}

void hxfStartApp(const HxfAppParam* restrict param) {
    // Define the default app data
    HxfAppData app = {
        .engine.drawingData = {
            .vertexPositions = {
                {-0.5f, -0.5f, 0.0f},
                {0.5f, -0.5f, 0.0f},
                {0.5f, -0.5f, 1.0f},
                {-0.5f, -0.5f, 1.0f},
                {-0.5f, 0.5f, 0.0f},
                {0.5f, 0.5f, 0.0f},
                {0.5f, 0.5f, 1.0f},
                {-0.5f, 0.5f, 1.0f},
            },
            .indexData = {
                0, 1, 2, 2, 3, 0,
                3, 2, 6, 6, 7, 3,
                7, 6, 5, 5, 4, 7,
                4, 5, 1, 1, 0, 4,
                2, 1, 5, 5, 6, 2,
                0, 3, 7, 7, 4, 0
            },
            .ubo = {
                HXF_MAT4_IDENTITY,
                HXF_MAT4_IDENTITY,
                hxfPerspectiveProjection(0.1f, 10.f, M_PI / 3.0f, (float)HXF_WINDOW_WIDTH / (float)HXF_WINDOW_HEIGHT)
            },
            .cubes = {
                { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },
                { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
                { {2.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
                { {3.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f} },
                { {4.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f} },
            }
        },
        .camera = {
            .position = { 0.0f, 0.0f, 1.0f },
            .up = { 0.0f, -1.0f, 0.0f },
            .yaw = -M_PI_2,
            .pitch = 0.0f,
        },
        .engine.keyboardState = &app.keyboardState,
        .engine.camera = &app.camera,
        .run = 1,
    };

    // Create the main window
    HxfWindowParam windowParameter = {
        param->hInstance,
        param->nCmdShow
    };
    hxfCreateMainWindow(&windowParameter, &app.mainWindow);
    app.engine.mainWindow = &app.mainWindow;

    // Initialization
    hxfInitInput(&app);
    hxfInitEngine(&app.engine);

    // Run the main loop
    mainLoop(&app);

    // Stop the application
    hxfStopEngine(&app.engine);
    hxfDestroyEngine(&app.engine);
    hxfDestroyMainWindow(&app.mainWindow);
}