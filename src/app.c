#include "app.h"
#include "engine/input-handler.h"
#include "engine/game-handler.h"

#include <time.h>
#include <math.h>

/**
 * @brief The width of textures.png
 */
#define TEXTURE_WIDTH 96.0F
/**
 * @brief The height of textures.png
 */
#define TEXTURE_HEIGHT 80.0F

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
                // top
                { { 0.0f, 1.0f, 1.0f }, { 64.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 0.0f, 1.0f, 0.0f }, { 64.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 1.0f, 0.0f }, { 80.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 1.0f, 1.0f }, { 80.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                // back
                { { 0.0f, 1.0f, 0.0f }, { 32.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 0.0f, 0.0f, 0.0f }, { 32.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 0.0f, 0.0f }, { 48.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 1.0f, 0.0f }, { 48.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                // bottom
                { { 0.0f, 0.0f, 0.0f }, { 80.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 0.0f, 0.0f, 1.0f }, { 80.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 0.0f, 1.0f }, { 96.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 0.0f, 0.0f }, { 96.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                // front
                { { 0.0f, 0.0f, 1.0f }, { 0.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 0.0f, 1.0f, 1.0f }, { 0.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 1.0f, 1.0f }, { 16.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 0.0f, 1.0f }, { 16.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                // right
                { { 1.0f, 0.0f, 1.0f }, { 16.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 1.0f, 1.0f }, { 16.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 1.0f, 0.0f }, { 32.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 1.0f, 0.0f, 0.0f }, { 32.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                // left
                { { 0.0f, 0.0f, 0.0f }, { 48.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 0.0f, 1.0f, 0.0f }, { 48.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 0.0f, 1.0f, 1.0f }, { 64.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { 0.0f, 0.0f, 1.0f }, { 64.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } }
            },
            .cubesVertexIndices = {
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
                .projection = hxfPerspectiveProjectionMatrix(0.01f, 128.0f, M_PI / 180.0f * 60.0f, (float)param->windowWidth / (float)param->windowHeight)
            },
        },
        .game.camera = {
            .position = { 0.0f, 3.0f, -2.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .yaw = M_PI_2,
            .pitch = 0.0f,
        },
        .run = 1,
        .appdataDirectory = param->appDataDirectory,
        .game.appdataDirectory = param->appDataDirectory,
        .engine.appdataDirectory = param->appDataDirectory
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