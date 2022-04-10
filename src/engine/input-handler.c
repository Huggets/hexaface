#include "input-handler.h"
#include "../input.h"
#include "../window.h"

#include <math.h>

static void emptyCallback(void* param) { }

/* MISC KEYS */

static void escapeKeyDown(void* param) {
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

static void iKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->i = 1;
}
static void iKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->i = 0;
}

static void jKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->j = 1;
}
static void jKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->j = 0;
}

static void kKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->k = 1;
}
static void kKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->k = 0;
}

static void lKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->l = 1;
}
static void lKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->l = 0;
}

static void oKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->o = 1;
}
static void oKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->o = 0;
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

static void uKeyDown(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->u = 1;
}
static void uKeyUp(void* param) {
    HxfKeyboardState* state = (HxfKeyboardState*)param;
    state->u = 0;
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
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_ESCAPE, escapeKeyDown, &app->mainWindow);

    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_LEFT, leftKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_RIGHT, rightKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_UP, upKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_DOWN, downKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_D, dKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_I, iKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_J, jKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_K, kKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_L, lKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_O, oKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_Q, qKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_S, sKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_U, uKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_Z, zKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SHIFT_LEFT, shiftKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SHIFT_RIGHT, shiftKeyDown, &app->keyboardState);
    hxfSetKeyDownCallback(&app->mainWindow, HXF_KEY_SPACE, spaceKeyDown, &app->keyboardState);

    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_LEFT, leftKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_RIGHT, rightKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_UP, upKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_DOWN, downKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_D, dKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_I, iKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_J, jKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_K, kKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_L, lKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_O, oKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_Q, qKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_S, sKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_U, uKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_Z, zKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SHIFT_LEFT, shiftKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SHIFT_RIGHT, shiftKeyUp, &app->keyboardState);
    hxfSetKeyUpCallback(&app->mainWindow, HXF_KEY_SPACE, spaceKeyUp, &app->keyboardState);
}

void hxfHandleInput(HxfAppData* restrict app) {
    const float moveSpeed = 3.0f;
    
    /* MISC KEYS */

    if (app->keyboardState.shift) {
        app->game.camera.position.y -= moveSpeed * app->frameDuration;
    }
    if (app->keyboardState.space) {
        app->game.camera.position.y += moveSpeed *app->frameDuration;
    }

    /* ARROW KEYS */

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