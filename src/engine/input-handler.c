#include "input-handler.h"
#include "game-handler.h"
#include "../input.h"
#include "../window.h"

#include <math.h>
#include <time.h>

static void emptyCallback(void* param) { }

/* MISC KEYS */

static void escapeKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.escape = 1;
}
static void escapeKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.escape = 0;
}

static void shiftKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.shift = 1;
}
static void shiftKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.shift = 0;
}

static void spaceKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.space = 1;
}
static void spaceKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.space = 0;
}

/* ARROW KEYS */

static void leftKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.leftArrow = 1;
}
static void leftKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.leftArrow = 0;
}

static void rightKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.rightArrow = 1;
}
static void rightKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.rightArrow = 0;
}

static void upKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.upArrow = 1;
}
static void upKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.upArrow = 0;
}

static void downKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.downArrow = 1;
}
static void downKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.downArrow = 0;
}

/* LETTER KEYS */

static void aKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.a = 1;

    // if (app->game.camera.isPointingToCube) {
    //     hxfReplaceCube(&app->game, &app->game.camera.nearPointedCube, app->game.cubeSelector);
    // }
}
static void aKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.a = 0;
}

static void cKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.c = 1;

    app->game.cubeSelector++;
    if (app->game.cubeSelector == HXF_TEXTURE_COUNT) {
        app->game.cubeSelector = HXF_TEXTURE_COUNT - 1;
    }
}
static void cKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.c = 0;
}

static void dKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.d = 1;
}
static void dKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.d = 0;
}

static void eKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.e = 1;

    // if (app->game.camera.isPointingToCube) {
    //     hxfReplaceCube(&app->game, &app->game.camera.pointedCube, 0);
    // }
}
static void eKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.e = 0;
}

static void iKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.i = 1;
}
static void iKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.i = 0;
}

static void jKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.j = 1;
}
static void jKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.j = 0;
}

static void kKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.k = 1;
}
static void kKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.k = 0;
}

static void lKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.l = 1;
}
static void lKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.l = 0;
}

static void oKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.o = 1;
}
static void oKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.o = 0;
}

static void qKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.q = 1;
}
static void qKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.q = 0;
}

static void sKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.s = 1;
}
static void sKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.s = 0;
}

static void uKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.u = 1;
}
static void uKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.u = 0;
}

static void wKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.w = 1;

    app->game.cubeSelector--;
    if (app->game.cubeSelector == 0) {
        app->game.cubeSelector = 1;
    }
}
static void wKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.w = 0;
}

static void zKeyDown(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.z = 1;
}
static void zKeyUp(void* param) {
    HxfAppData* app = (HxfAppData*)param;
    app->keyboardState.z = 0;
}

void hxfInitInput(HxfAppData* app) {
    // Initialize the default callbacks

    for (int i = 0; i != HXF_WINDOW_KEY_CALLBACK_COUNT; i++) {
        hxfSetKeyDownCallback(&app->mainWindow, i, emptyCallback, NULL);
        hxfSetKeyUpCallback(&app->mainWindow, i, emptyCallback, NULL);
    }

    // Set the custom callbacks that are needed

    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_ESCAPE, escapeKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_LEFT, leftKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_RIGHT, rightKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_UP, upKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_DOWN, downKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_A, aKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_C, cKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_D, dKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_E, eKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_I, iKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_J, jKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_K, kKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_L, lKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_O, oKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_Q, qKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_S, sKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_U, uKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_W, wKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_Z, zKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SHIFT_LEFT, shiftKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SHIFT_RIGHT, shiftKeyDown, app);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SPACE, spaceKeyDown, app);

    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_ESCAPE, escapeKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_LEFT, leftKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_RIGHT, rightKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_UP, upKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_DOWN, downKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_A, aKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_C, cKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_D, dKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_E, eKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_I, iKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_J, jKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_K, kKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_L, lKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_O, oKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_Q, qKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_S, sKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_U, uKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_W, wKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_Z, zKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SHIFT_LEFT, shiftKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SHIFT_RIGHT, shiftKeyUp, app);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SPACE, spaceKeyUp, app);
}

void hxfHandleInput(HxfAppData* restrict app) {
    const float moveSpeed = 3.0f;
    const float repeatTime = 0.2f;

    /* MISC KEYS */

    if (app->keyboardState.escape) {
        app->mainWindow.shouldDestroyed = 1;
    }
    if (app->keyboardState.shift) {
        app->game.camera.position.y -= moveSpeed * app->frameDuration;
    }
    if (app->keyboardState.space) {
        app->game.camera.position.y += moveSpeed * app->frameDuration;
    }

    /* ARROW KEYS */

    // Change the camera orientation.

    if (app->keyboardState.leftArrow) {
        app->game.camera.yaw -= M_PI * app->frameDuration;
    }
    if (app->keyboardState.rightArrow) {
        app->game.camera.yaw += M_PI * app->frameDuration;
    }
    if (app->keyboardState.downArrow) {
        app->game.camera.pitch -= M_PI * app->frameDuration;
    }
    if (app->keyboardState.upArrow) {
        app->game.camera.pitch += M_PI * app->frameDuration;
    }

    /* LETTER KEYS */

    // Change the camera position.

    if (app->keyboardState.z) {
        HxfVec3 tmp = app->game.camera.front;
        tmp.x *= app->frameDuration * moveSpeed;
        tmp.y *= app->frameDuration * moveSpeed;
        tmp.z *= app->frameDuration * moveSpeed;

        app->game.camera.position = hxfVec3Add(&app->game.camera.position, &tmp);
    }
    if (app->keyboardState.s) {
        HxfVec3 tmp = app->game.camera.front;
        tmp.x *= app->frameDuration * moveSpeed;
        tmp.y *= app->frameDuration * moveSpeed;
        tmp.z *= app->frameDuration * moveSpeed;

        app->game.camera.position = hxfVec3Sub(&app->game.camera.position, &tmp);
    }
    if (app->keyboardState.q) {
        HxfVec3 tmp = hxfVec3Cross(&app->game.camera.front, &app->game.camera.up);
        tmp = hxfVec3Normalize(&tmp);
        tmp.x *= -app->frameDuration * moveSpeed;
        tmp.y *= -app->frameDuration * moveSpeed;
        tmp.z *= -app->frameDuration * moveSpeed;

        app->game.camera.position = hxfVec3Add(&app->game.camera.position, &tmp);
    }
    if (app->keyboardState.d) {
        HxfVec3 tmp = hxfVec3Cross(&app->game.camera.front, &app->game.camera.up);
        tmp = hxfVec3Normalize(&tmp);
        tmp.x *= app->frameDuration * moveSpeed;
        tmp.y *= app->frameDuration * moveSpeed;
        tmp.z *= app->frameDuration * moveSpeed;

        app->game.camera.position = hxfVec3Add(&app->game.camera.position, &tmp);
    }

    static int wasPressedA = 0;
    if (app->keyboardState.a) {
        static float lastTimeTriggered = 0.0f;
        float tmp = (float)clock() / (float)CLOCKS_PER_SEC;

        // Place a cube if the key wasn’t press the last frame or with auto-repeat
        if (!wasPressedA) {
            wasPressedA = 1;
            hxfReplaceCube(&app->game, &app->game.camera.nearPointedCube, app->game.cubeSelector);
            lastTimeTriggered = tmp;
        }
        else if (tmp > lastTimeTriggered + repeatTime) {
            lastTimeTriggered = tmp;
            hxfReplaceCube(&app->game, &app->game.camera.nearPointedCube, app->game.cubeSelector);
        }
    }
    else {
        wasPressedA = 0;
    }

    static int wasPressedE = 0;
    if (app->keyboardState.e) {
        static float lastTimeTriggered = 0.0f;
        float tmp = (float)clock() / (float)CLOCKS_PER_SEC;

        // Destroy a cube if the key wasn’t press the last frame or with auto-repeat
        if (!wasPressedE) {
            wasPressedE = 1;
            hxfReplaceCube(&app->game, &app->game.camera.pointedCube, 0);
            lastTimeTriggered = tmp;
        }
        else if (tmp > lastTimeTriggered + repeatTime) {
            lastTimeTriggered = tmp;
            hxfReplaceCube(&app->game, &app->game.camera.pointedCube, 0);
        }
    }
    else {
        wasPressedE = 0;
    }

    /* COMPUTING */

    // Limit the pitch

    if (app->game.camera.pitch > 1.570796251f) {
        app->game.camera.pitch = 1.570796251f;
    }
    else if (app->game.camera.pitch < -1.570796251f) {
        app->game.camera.pitch = -1.570796251f;
    }

    // Update the camera direction and front

    HxfVec3 direction = {
        cosf(app->game.camera.yaw) * cosf(app->game.camera.pitch),
        sinf(app->game.camera.pitch),
        sinf(app->game.camera.yaw) * cosf(app->game.camera.pitch)
    };
    app->game.camera.direction = direction;
    app->game.camera.front = direction;
    app->game.camera.front.y = 0.f;
    app->game.camera.front = hxfVec3Normalize(&app->game.camera.front);
}