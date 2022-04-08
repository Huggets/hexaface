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

static void initWorld(HxfAppData* restrict app) {
    const HxfVec3 brown = { 135.0f / 255.0f, 64.0f / 255.0f, 13.0f / 255.0f };
    const HxfVec3 green = { 0.0f, 124.0f / 255.0f, 67.0f / 255.0f };
    // Fill with brown the bottom
    for (int y = 6; y != 2; y--) {
        for (int x = 0; x != 15; x++) {
            for (int z = 0; z != 15; z++) {
                app->world.cubes[z][y][x] = 1;
            }
        }
    }

    // A green on top of the brown
    for (int x = 0; x != 15; x++) {
        for (int z = 0; z != 15; z++) {
            app->world.cubes[z][3][x] = 2;
        }
    }

    for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
                uint8_t type = app->world.cubes[z][y][x];
                if (type != 0) {
                    HxfCubeData* const restrict cube = &app->engine.drawingData.cubes[app->engine.drawingData.cubeCount];
                    cube->cubePosition.x = x;
                    cube->cubePosition.y = y;
                    cube->cubePosition.z = z;
                    switch (type) {
                    case 1:
                        cube->cubeColor = brown;
                        break;
                    case 2:
                        cube->cubeColor = green;
                        break;
                    }

                    app->engine.drawingData.cubeCount++;
                }
            }
        }
    }
}

void hxfStartApp(const HxfAppParam* restrict param) {
    // Define the default app data
    HxfAppData app = {
        .engine.keyboardState = &app.keyboardState,
        .engine.camera = &app.camera,
        .engine.drawingData = {
            .cubesVertices = {
                {-0.5f, -0.5f, 0.0f},
                {0.5f, -0.5f, 0.0f},
                {0.5f, -0.5f, 1.0f},
                {-0.5f, -0.5f, 1.0f},
                {-0.5f, 0.5f, 0.0f},
                {0.5f, 0.5f, 0.0f},
                {0.5f, 0.5f, 1.0f},
                {-0.5f, 0.5f, 1.0f},
            },
            .cubesVertexIndices = {
                0, 1, 2, 2, 3, 0,
                3, 2, 6, 6, 7, 3,
                7, 6, 5, 5, 4, 7,
                4, 5, 1, 1, 0, 4,
                2, 1, 5, 5, 6, 2,
                0, 3, 7, 7, 4, 0
            },
            .ubo = {
                .model = HXF_MAT4_IDENTITY,
                .view = HXF_MAT4_IDENTITY,
                .projection = hxfPerspectiveProjection(0.1f, 100.0f, M_PI / 180.0f * 60.0f, (float)param->windowWidth / (float)param->windowHeight)
            },
        },
        .camera = {
            .position = { 0.0f, 0.0f, 2.0f },
            .up = { 0.0f, -1.0f, 0.0f },
            .yaw = -M_PI_2,
            .pitch = 0.0f,
        },
        .run = 1,
    };

    // Scale the model matrix by 0.5
    const HxfVec3 scaleVector = { 0.125f, 0.125f, 0.125f };
    app.engine.drawingData.ubo.model = hxfMat4ScaleMatrix(&scaleVector);

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
    initWorld(&app);
    hxfInitEngine(&app.engine);

    // Run the main loop
    mainLoop(&app);

    // Stop the application
    hxfStopEngine(&app.engine);
    hxfDestroyEngine(&app.engine);
    hxfDestroyMainWindow(&app.mainWindow);
}