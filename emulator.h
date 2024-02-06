#pragma once

#include "external\raylib.h"

#include "constants.h"

// Value at char* is allocated, remember to free it
char* openFileDialog();

// Opens file dialog if fileName is NULL
void initEmulator(char* fileName);