#pragma once

#include <time.h>

/**
 * @brief Represent the scan code of a key.
 * 
 * The name are based on an azerty keyboard.
 */
typedef enum HxfKeyCode {
    HXF_KEY_ESCAPE = 1,
    HXF_KEY_1 = 2,
    HXF_KEY_2 = 3,
    HXF_KEY_3 = 4,
    HXF_KEY_4 = 5,
    HXF_KEY_5 = 6,
    HXF_KEY_6 = 7,
    HXF_KEY_7 = 8,
    HXF_KEY_8 = 9,
    HXF_KEY_9 = 10,
    HXF_KEY_0 = 11,
    HXF_KEY_LEFT_PARENTHESIS = 12,
    HXF_KEY_EQUAL = 13,
    HXF_KEY_BACKSPACE = 14,
    HXF_KEY_TAB = 15,
    HXF_KEY_A = 16,
    HXF_KEY_Z = 17,
    HXF_KEY_E = 18,
    HXF_KEY_R = 19,
    HXF_KEY_T = 20,
    HXF_KEY_Y = 21,
    HXF_KEY_U = 22,
    HXF_KEY_I = 23,
    HXF_KEY_O = 24,
    HXF_KEY_P = 25,
    HXF_KEY_CIRCUMFLEX = 26,
    HXF_KEY_DOLLAR = 27,
    HXF_KEY_ENTER = 28,
    HXF_KEY_CONTROL = 29, // control left and right have the same scan code
    HXF_KEY_Q = 30,
    HXF_KEY_S = 31,
    HXF_KEY_D = 32,
    HXF_KEY_F = 33,
    HXF_KEY_G = 34,
    HXF_KEY_H = 35,
    HXF_KEY_J = 36,
    HXF_KEY_K = 37,
    HXF_KEY_L = 38,
    HXF_KEY_M = 39,
    HXF_KEY_PERCENT = 40,
    HXF_KEY_SECOND = 41,
    HXF_KEY_SHIFT_LEFT = 42,
    HXF_KEY_ASTERISK = 43,
    HXF_KEY_W = 44,
    HXF_KEY_X = 45,
    HXF_KEY_C = 46,
    HXF_KEY_V = 47,
    HXF_KEY_B = 48,
    HXF_KEY_N = 49,
    HXF_KEY_COMMA = 50,
    HXF_KEY_SEMICOLON = 51,
    HXF_KEY_COLON = 52,
    HXF_KEY_EXCLAMATION_MARK = 53,
    HXF_KEY_SHIFT_RIGHT = 54,
    HXF_KEY_ALT_GR = 56,
    HXF_KEY_SPACE = 57,
    HXF_KEY_F1 = 59,
    HXF_KEY_F2 = 60,
    HXF_KEY_F3 = 61,
    HXF_KEY_F4 = 62,
    HXF_KEY_F5 = 63,
    HXF_KEY_F6 = 64,
    HXF_KEY_F7 = 65,
    HXF_KEY_F8 = 66,
    HXF_KEY_F9 = 67,
    HXF_KEY_HOME = 71,
    HXF_KEY_UP = 72,
    HXF_KEY_PG_UP = 73,
    HXF_KEY_LEFT = 75,
    HXF_KEY_RIGHT = 77,
    HXF_KEY_END = 79,
    HXF_KEY_DOWN = 80,
    HXF_KEY_PG_DOWN = 81,
    HXF_KEY_INSERT = 82,
    HXF_KEY_SUPPR = 83,
    HXF_KEY_F11 = 87,
    HXF_KEY_F12 = 88,
    HXF_KEY_WINDOW_LEFT = 91,
    HXF_KEY_MENU = 93, // Button that work like a rigth click
} HxfKeyCode;

/**
 * @brief Represent the state of the keyboard.
 * 
 * The key is set to 1 when pressed and 0 when released.
 */
typedef struct HxfKeyboardState {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    int p;
    int q;
    int r;
    int s;
    int t;
    int u;
    int v;
    int w;
    int x;
    int z;
    int space;
    int ctrl;
    int shift;
    int tab;
    int leftArrow;
    int rightArrow;
    int upArrow;
    int downArrow;
    int escape;
} HxfKeyboardState;