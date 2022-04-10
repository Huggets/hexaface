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
        hxfUpdatePointedCube(&app->camera, &app->world);
        hxfEngineFrame(&app->engine);

        app->run = !app->mainWindow.shouldDestroyed;
    }
}

static void setCube(HxfCubeData* restrict cubes, HxfDrawingData* restrict drawingData, const HxfVec3* restrict position, uint32_t textureId, size_t* index) {
    cubes[*index].cubePosition = *position;
    cubes[*index].cubeColor = drawingData->textures[textureId];
    (*index)++;
}

static void initWorld(HxfAppData* app) {
    // Initialize the cubes

    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != 3; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                app->world.cubes[x][y][z] = 1;
            }
        }
    }
    app->world.cubes[0][3][0] = 2;
    app->world.cubes[0][4][0] = 2;
    app->world.cubes[0][3][1] = 2;

    // Convert cube to drawing data

    for (int x = 0; x != HXF_WORLD_LENGTH; x++) {
        for (int y = 0; y != HXF_WORLD_LENGTH; y++) {
            for (int z = 0; z != HXF_WORLD_LENGTH; z++) {
                const uint32_t textureId = app->world.cubes[x][y][z];
                const HxfVec3 position = { x, y, z };
                HxfDrawingData* const drawingData = &app->engine.drawingData;

                if (textureId != 0) {
                    if ((x != HXF_WORLD_LENGTH - 1 && app->world.cubes[x + 1][y][z] == 0)
                        || x == HXF_WORLD_LENGTH - 1) {
                        setCube(drawingData->faces[4], drawingData, &position, textureId, &drawingData->faceRightCount);
                    }
                    if ((x != 0 && app->world.cubes[x - 1][y][z] == 0)
                        || x == 0) {
                        setCube(drawingData->faces[5], drawingData, &position, textureId, &drawingData->faceLeftCount);
                    }
                    if ((y != HXF_WORLD_LENGTH - 1 && app->world.cubes[x][y + 1][z] == 0)
                        || y == HXF_WORLD_LENGTH - 1) {
                        setCube(drawingData->faces[0], drawingData, &position, textureId, &drawingData->faceTopCount);
                    }
                    if ((y != 0 && app->world.cubes[x][y - 1][z] == 0)
                        || y == 0) {
                        setCube(drawingData->faces[2], drawingData, &position, textureId, &drawingData->faceBottomCount);
                    }
                    if ((z != HXF_WORLD_LENGTH - 1 && app->world.cubes[x][y][z + 1] == 0)
                        || z == HXF_WORLD_LENGTH - 1) {
                        setCube(drawingData->faces[1], drawingData, &position, textureId, &drawingData->faceBackCount);
                    }
                    if ((z != 0 && app->world.cubes[x][y][z - 1] == 0)
                        || z == 0) {
                        setCube(drawingData->faces[3], drawingData, &position, textureId, &drawingData->faceFrontCount);
                    }
                }
            }
        }
    }
}

void hxfStartApp(const HxfAppParam* restrict param) {
    // Set the default app data

    HxfAppData app = {
        .engine.keyboardState = &app.keyboardState,
        .engine.camera = &app.camera,
        .engine.world = &app.world,
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
        .camera = {
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
    initWorld(&app);
    hxfInitEngine(&app.engine);

    // Run the main loop

    mainLoop(&app);

    // Stop the application

    hxfStopEngine(&app.engine);
    hxfDestroyEngine(&app.engine);
    hxfDestroyMainWindow(&app.mainWindow);
}