#pragma once

#include "constants.h"

char* openFileDialog(); // char* is allocated. Free it.
bool initEmulator(char* fileName); // Restart if true is returned