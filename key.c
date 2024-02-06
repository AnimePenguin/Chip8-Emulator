#include "external/raylib.h"

#include "key.h"

typedef struct KeyBind {
	int key;
	BYTE hex;
} KeyBind;

const KeyBind keyMap[] = {
	{KEY_ONE, 0x1}, {KEY_TWO, 0x2}, {KEY_THREE, 0x3}, {KEY_FOUR, 0xC},
	{KEY_Q  , 0x4}, {KEY_W  , 0x5}, {KEY_E    , 0x6}, {KEY_R   , 0xD},
	{KEY_A  , 0x7}, {KEY_S  , 0x8}, {KEY_D    , 0x9}, {KEY_F   , 0xE},
	{KEY_Z  , 0xA}, {KEY_X  , 0x0}, {KEY_C    , 0xB}, {KEY_V   , 0xF},
};

BYTE guiHeldKey = NO_PRESSED_KEY;
BYTE guiReleasedKey = NO_PRESSED_KEY;

BYTE getKeyInHex(bool checkHeld) {

	if (checkHeld) {
		if (guiHeldKey != NO_PRESSED_KEY) {
			return guiHeldKey;
		}

	} else {
		if (guiReleasedKey != NO_PRESSED_KEY) {
			return guiReleasedKey;
		}
	}

	// Use IsKeyDown function if checkHeld else use IsKeyReleased function
	bool (*checkKey)(int) = checkHeld ? &IsKeyDown : &IsKeyReleased;

	for (int i = 0; i <= 0xF; i++) {

		if ((*checkKey)(keyMap[i].key)) {
			return keyMap[i].hex;
		}

	}

	return NO_PRESSED_KEY;
}