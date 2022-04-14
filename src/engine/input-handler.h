#pragma once

#include "../app.h"

/**
 * @brief Initialize the input of the application.
 * 
 * It includes the definition of the callback functions that are called
 * when the keys are pressed or released.
 * 
 * @param app The application that initializes the input.
 */
void hxfInputInit(HxfAppData* app);

void hxfHandleInput(HxfAppData* restrict app);