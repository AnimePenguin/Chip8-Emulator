#pragma once

#include "constants.h"

#define NO_PRESSED_KEY 0xFF

// if checkHeld is true, return hex if key pressed down
// if checkHeld is false, return hex if key pressed and released
//
// returns NO_PRESSED_KEY when no key is pressed
BYTE getKeyInHex(bool checkHeld);