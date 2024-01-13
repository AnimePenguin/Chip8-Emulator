#pragma once

#include "constants.h"

#define NO_PRESSED_KEY 0x10

// Returns NO_PRESSED_KEY when no key is pressed
BYTE getKeyInHex(bool checkHeld);