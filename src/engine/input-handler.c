#include "input-handler.h"
#include "../input.h"
#include "../window.h"

#include <math.h>

static void emptyCallback(void* param) { }

/* MISC KEYS */

static void escapeKeyCallback(void* param) {
    HxfWindow* window = (HxfWindow*)param;
    window->shouldDestroyed = 1;
}

static void shiftKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->shift = 1;
}
static void shiftKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->shift = 0;
}

static void spaceKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->space = 1;
}
static void spaceKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->space = 0;
}

/* ARROW KEYS */

static void leftKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->leftArrow = 1;
}
static void leftKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->leftArrow = 0;
}

static void rightKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->rightArrow = 1;
}
static void rightKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->rightArrow = 0;
}

static void upKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->upArrow = 1;
}
static void upKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->upArrow = 0;
}

static void downKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->downArrow = 1;
}
static void downKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->downArrow = 0;
}

/* LETTER KEYS */

static void dKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->d = 1;
}
static void dKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->d = 0;
}

static void qKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->q = 1;
}
static void qKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->q = 0;
}

static void sKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->s = 1;
}
static void sKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->s = 0;
}

static void zKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->z = 1;
}
static void zKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->z = 0;
}

void hxfInitInput(HxfAppData* restrict app) {
    // Initialize the default callbacks
    for (int i = 0; i != HXF_WINDOW_KEY_CALLBACK_COUNT; i++) {
        hxfSetKeyDownCallback(&app->mainWindow, i, emptyCallback, NULL);
        hxfSetKeyUpCallback(&app->mainWindow, i, emptyCallback, NULL);
    }

    // Set the custom callbacks that are needed
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_ESCAPE, escapeKeyCallback, &app->mainWindow);

    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_LEFT, leftKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_RIGHT, rightKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_UP, upKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_DOWN, downKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_D, dKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_Q, qKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_S, sKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_Z, zKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SHIFT_LEFT, shiftKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SHIFT_RIGHT, shiftKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SPACE, spaceKeyDown, &app->keyboardState);

    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_LEFT, leftKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_RIGHT, rightKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_UP, upKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_DOWN, downKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_D, dKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_Q, qKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_S, sKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_Z, zKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SHIFT_LEFT, shiftKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SHIFT_RIGHT, shiftKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SPACE, spaceKeyUp, &app->keyboardState);
}

void hxfHandleInput(HxfAppData* restrict app) {
    /* MISC KEYS */

    if (app->keyboardState.shift) {
        app->camera.position.y += app->frameDuration;
    }
    if (app->keyboardState.space) {
        app->camera.position.y -= app->frameDuration;
    }

    /* ARROW KEYS */

    if (app->keyboardState.leftArrow) {
        app->camera.yaw += 3.14f * app->frameDuration;
    }
    if (app->keyboardState.rightArrow) {
        app->camera.yaw -= 3.14f * app->frameDuration;
    }
    if (app->keyboardState.upArrow) {
        app->camera.pitch -= 3.14f * app->frameDuration;
    }
    if (app->keyboardState.downArrow) {
        app->camera.pitch += 3.14f * app->frameDuration;
    }

    /* LETTER KEYS */

    if (app->keyboardState.z) {
        HxfVec3 tmp = app->camera.front;
        tmp.x *= app->frameDuration;
        tmp.y *= app->frameDuration;
        tmp.z *= app->frameDuration;

        app->camera.position = hxfVec3Add(app->camera.position, tmp);
    }
    if (app->keyboardState.s) {
        HxfVec3 tmp = app->camera.front;
        tmp.x *= app->frameDuration;
        tmp.y *= app->frameDuration;
        tmp.z *= app->frameDuration;

        app->camera.position = hxfVec3Sub(app->camera.position, tmp);
    }
    if (app->keyboardState.q) {
        HxfVec3 tmp = hxfVec3Normalize(hxfVec3Cross(app->camera.front, app->camera.up));
        tmp.x *= -app->frameDuration;
        tmp.y *= -app->frameDuration;
        tmp.z *= -app->frameDuration;

        app->camera.position = hxfVec3Add(app->camera.position, tmp);
    }
    if (app->keyboardState.d) {
        HxfVec3 tmp = hxfVec3Normalize(hxfVec3Cross(app->camera.front, app->camera.up));
        tmp.x *= app->frameDuration;
        tmp.y *= app->frameDuration;
        tmp.z *= app->frameDuration;

        app->camera.position = hxfVec3Add(app->camera.position, tmp);
    }

    /* COMPUTING */
    M_PI_2;

    // Limits
    if (app->camera.pitch > 1.570796251f) {
        app->camera.pitch = 1.570796251f;
    }
    else if (app->camera.pitch < -1.570796251f) {
        app->camera.pitch = -1.570796251f;
    }

    HxfVec3 direction = {
        cosf(app->camera.yaw) * cosf(app->camera.pitch),
        sinf(app->camera.pitch),
        sinf(app->camera.yaw) * cosf(app->camera.pitch)
    };
    app->camera.direction = direction;
    app->camera.front = direction;
    app->camera.front.y = 0.f;
    app->camera.front = hxfVec3Normalize(app->camera.front);
}