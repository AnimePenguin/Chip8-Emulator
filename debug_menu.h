#pragma once

#include "external\raylib.h"
#include "external\raygui.h"

#include "constants.h"
#include "cpu.h"

typedef struct DebugMenuContext {
	Rectangle drawArea;
	Rectangle menuRect;

	Rectangle registerBoxRect;
	int selectedRegister;

	int selectedOtherVariable;

	Rectangle memoryScrollPanel;
	Vector2 memoryScrollOffset;
	Rectangle memoryScrollView;
	Rectangle memoryScrollViewSize;

	int selectedMemoryBox;

	Rectangle autoMoveCheckBoxRect;
	bool autoMoveMemory;

	Rectangle stackRect;
} DebugMenuContext;

DebugMenuContext newDebugMenuContext(Rectangle* drawArea);

void resizeDebugMenu(Rectangle* newDrawArea, DebugMenuContext* context);
void drawDebugMenu(CPU* cpu, BYTE memory[], DebugMenuContext* context);