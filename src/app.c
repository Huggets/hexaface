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
            .cubesVertices = {
                { {-0.5f, -0.5f, 0.0f}, {0.0f, -1.0f, 0.0f} },
                { {0.5f, -0.5f, 0.0f}, {0.0f, -1.0f, 0.0f} },
                { {0.5f, -0.5f, 1.0f}, {0.0f, -1.0f, 0.0f} },
                { {-0.5f, -0.5f, 1.0f}, {0.0f, -1.0f, 0.0f} },

                { {-0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f} },
                { {0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f} },
                { {0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f} },
                { {-0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f} },

                { {-0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f} },
                { {0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f} },
                { {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
                { {-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },

                { {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f} },
                { {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f} },
                { {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f} },
                { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f} },

                { {0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f } },
                { {0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f } },
                { {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f } },
                { {0.5f, 0.5f, 1.0f}, {1.0f, 0.0f, 0.0f } },

                { {-0.5f, -0.5f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
                { {-0.5f, -0.5f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
                { {-0.5f, 0.5f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
                { {-0.5f, 0.5f, 0.0f}, {-1.0f, 0.0f, 0.0f} }
            },
            .cubesVerticesIndex = {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4,
                8, 9, 10, 10, 11, 8,
                12, 13, 14, 14, 15, 12,
                16, 17, 18, 18, 19, 16,
                20, 21, 22, 22, 23, 20
            },
            .ubo = {
                .model = HXF_MAT4_IDENTITY,
                .view = HXF_MAT4_IDENTITY,
                .projection = hxfPerspectiveProjection(0.1f, 10.f, M_PI / 3.0f, (float)param->windowWidth / (float)param->windowHeight),
                .lightPosition = { 0.0f, 0.0f, 0.0f },
                .lightColor = { 0.8f, 0.8f, 0.8f, }
            },
            .cubes = {
                { {0.0f, 0.0f, 0.0f}, {79.0f / 255.0f, 18.0f / 255.0f, 140.0f / 255.0f} },
                { {-1.0f, 0.0f, 0.0f}, {79.0f / 255.0f, 18.0f / 255.0f, 140.0f / 255.0f} },
                { {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f} },
            },
        },
        .camera = {
            .position = { 0.0f, 0.0f, 2.0f },
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
        param->nCmdShow,
        param->windowWidth,
        param->windowHeight
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