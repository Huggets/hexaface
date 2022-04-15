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

/**
 * @brief The game loop.
 */
static void mainLoop(HxfAppData* restrict app) {
    const clock_t startClock = clock(); ///< The time when the loop started.
    clock_t currentClock = startClock; ///< The time when the current frame started.

    while (app->run) {
        clock_t lastClock = currentClock; // Get the last clock
        currentClock = clock(); // Update the current clock
        app->frameDuration = (float)(currentClock - lastClock) / (float)CLOCKS_PER_SEC; // Time duration of the last frame (in seconds)

        hxfReadWindowMessages(&app->mainWindow);
        hxfHandleInput(app);
        hxfGameFrame(&app->game);
        hxfGraphicsFrame(&app->graphics);

        app->run = !app->mainWindow.shouldDestroyed;
    }
}

void hxfAppStart(const HxfAppParam* restrict param) {
    const int halfWindowHeight = param->windowHeight / 2;
    const int selectorCubeSize = 100.0f;

    // Set the default app data

    HxfAppData app = {
        .appdataDirectory = param->appDataDirectory,
        .game.appdataDirectory = param->appDataDirectory,
        .graphics.appdataDirectory = param->appDataDirectory,

        .run = 1,

        .game.graphics = &app.graphics,
        .game.camera = {
            .position = { 0.0f, 3.0f, -2.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .yaw = M_PI_2,
            .pitch = 0.0f,
        },

        .graphics.keyboardState = &app.keyboardState,
        .graphics.camera = &app.game.camera,
        .graphics.world = &app.game.world,
        .graphics.drawingData = {
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
            .mvp = {
                .model = HXF_MAT4_IDENTITY,
                .view = HXF_MAT4_IDENTITY,
                .projection = hxfPerspectiveProjectionMatrix(0.01f, 128.0f, M_PI / 180.0f * 60.0f, (float)param->windowWidth / (float)param->windowHeight)
            },
            .iconVertices = {
                { { 0.0f - selectorCubeSize / 2, halfWindowHeight - selectorCubeSize }, { 0.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { selectorCubeSize - selectorCubeSize / 2, halfWindowHeight - selectorCubeSize }, { 16.0f / TEXTURE_WIDTH, 0.0f / TEXTURE_HEIGHT } },
                { { selectorCubeSize - selectorCubeSize / 2, halfWindowHeight }, { 16.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
                { { 0.0f - selectorCubeSize / 2, halfWindowHeight }, { 0.0f / TEXTURE_WIDTH, 16.0f / TEXTURE_HEIGHT } },
            },
            .iconVertexIndices = {
                0, 1, 2, 2, 3, 0
            },
            .iconInstances = {
                { 1 }
            }
        },
    };

    // Create the main window

    HxfWindowParam windowParameter = {
        param->hInstance,
        param->nCmdShow,
        param->windowWidth,
        param->windowHeight
    };
    hxfCreateMainWindow(&windowParameter, &app.mainWindow);
    app.graphics.mainWindow = &app.mainWindow;

    // Initialization

    hxfInputInit(&app);
    hxfGameInit(&app.game);
    hxfGraphicsInit(&app.graphics);

    // Run the main loop

    mainLoop(&app);

    // Stop the application

    hxfGraphicsStop(&app.graphics);
    hxfGameStop(&app.game);
    hxfGraphicsDestroy(&app.graphics);
    hxfDestroyMainWindow(&app.mainWindow);
}