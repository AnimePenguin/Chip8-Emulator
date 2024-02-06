#include "debug_menu.h"
#include "key.h"

#define MENU_WIDTH 592
#define MENU_HEIGHT 260

#define MEMEORY_BOX_SIZE 52

extern BYTE guiHeldKey;
extern BYTE guiReleasedKey;

Rectangle getMenuRect(Rectangle* drawArea) {
	Rectangle menuRect = {
		drawArea->x + (drawArea->width - MENU_WIDTH)/2,
		drawArea->y + (drawArea->height - MENU_HEIGHT)/2,
		MENU_WIDTH, MENU_HEIGHT
	};

	return menuRect;
}

Rectangle getRegisterBoxRect(Rectangle* menuRect) {
	Rectangle registerBoxRect = {
		menuRect->x + 8, menuRect->y + 20,
		208, 152
	};

	return registerBoxRect;
}

Rectangle getMemoryScrollPanelRect(Rectangle* menuRect) {
	Rectangle memoryScrollPanelRect = {
		menuRect->x + 8, menuRect->y + 184,
		576, 72
	};

	return memoryScrollPanelRect;
}

Rectangle getAutoMoveCheckBoxRect(Rectangle* menuRect) {
	Rectangle checkBoxRect = {
		menuRect->x + 126,
		menuRect->y + 190,
		12, 12
	};

	return checkBoxRect;
}

Rectangle getStackRect(Rectangle* menuRect) {
	Rectangle stackRect = {
		menuRect->x + 234,
		menuRect->y + 146,
		154, 24
	};

	return stackRect;
}

DebugMenuContext newDebugMenuContext(Rectangle* drawArea) {
	Rectangle menuRect = getMenuRect(drawArea);

	DebugMenuContext context = {
		.menuRect = menuRect,
		.registerBoxRect = getRegisterBoxRect(&menuRect),
		.selectedRegister = -1,
		.selectedOtherVariable = -1,

		.memoryScrollPanel = getMemoryScrollPanelRect(&menuRect),
		.memoryScrollOffset = {0, 0},
		.memoryScrollView = {0, 0, 0, 0},
		.memoryScrollViewSize = {0, 0, MEM_SIZE * MEMEORY_BOX_SIZE, 32},

		.selectedMemoryBox = -1,

		.autoMoveCheckBoxRect = getAutoMoveCheckBoxRect(&menuRect),
		.autoMoveMemory = true,

		.stackRect = getStackRect(&menuRect),
	};

	return context;
}

void resizeDebugMenu(Rectangle* newDrawArea, DebugMenuContext* context) {
	context->menuRect = getMenuRect(newDrawArea);
	context->registerBoxRect = getRegisterBoxRect(&context->menuRect);
	context->memoryScrollPanel = getMemoryScrollPanelRect(&context->menuRect);
	context->autoMoveCheckBoxRect = getAutoMoveCheckBoxRect(&context->menuRect);
	context->stackRect = getStackRect(&context->menuRect);
}

void unselectAllInputBoxes(DebugMenuContext* context) {
	context->selectedRegister = -1;
	context->selectedOtherVariable = -1;
	context->selectedMemoryBox = -1;
}

void drawRegisters(CPU* cpu, DebugMenuContext* context) {
	GuiGroupBox(context->registerBoxRect, "Registers");

	for (int i = 0; i < NUM_OF_REGISTERS; i++) {

		Rectangle registerRect = {
			context->registerBoxRect.x + 24 + (i / 5) * 64,
			context->registerBoxRect.y + 10 + (i % 5) * 24,
			40, 16
		};

		// Keep the value box for F at bottom
		if (i == 15) {
			registerRect.x = context->registerBoxRect.x + 88;
			registerRect.y = context->registerBoxRect.y + 130;
		}

		int value = cpu->registers[i];

		bool clicked = GuiValueBox(
			registerRect, TextFormat("%X ", i),
			&value, 0, 255, context->selectedRegister == i
		);

		cpu->registers[i] = value;

		if (clicked) {
			unselectAllInputBoxes(context);
			context->selectedRegister = i;
		}
	}
}

void drawOtherVaribles(CPU* cpu, DebugMenuContext* context) {
	Rectangle labelRect = {
		context->menuRect.x + 234, context->menuRect.y + 22,
		120, 24
	};

	Rectangle valueBoxRect = {
		context->menuRect.x + 348, context->menuRect.y + 22,
		40, 24
	};

	char* labels[] = {
		"Program Counter", "Index Register", 
		"Delay Timer", "Sound Timer"
	};

	unsigned short* shortValues[] = {
		&cpu->programCounter, &cpu->indexRegister,
	};

	BYTE* byteValues[] = {
		&cpu->delayTimer, &cpu->soundTimer,
	};

	for (int i = 0; i < 4; i++) {
		GuiLabel(labelRect, labels[i]);

		// The first two values use short as datatype and the last two use byte
		int value = i < 2 ? *shortValues[i] : *byteValues[i - 2];

		bool clicked = GuiValueBox(
			valueBoxRect, NULL, &value, 0, MEM_SIZE, 
			context->selectedOtherVariable == i
		);

		if (clicked) {
			unselectAllInputBoxes(context);
			context->selectedOtherVariable = i;
		}

		if (i < 2) {
			*shortValues[i] = value;
		} else {
			*byteValues[i - 2] = value;
		}

		valueBoxRect.y += 32;
		labelRect.y += 32;
	}
}

void drawStack(CPU* cpu, DebugMenuContext* context) {
	Stack* stack = &cpu->stack;

	const char** values = MemAlloc(stack->index * sizeof(char*));

	for (int i = 0; i < stack->index; i++) {
		values[i] = TextFormat("0x%03X", stack->values[stack->index - i - 1]);
	}

	const char* joinedValues = TextJoin(values, stack->index, ", ");
	GuiDummyRec(context->stackRect, TextFormat("Stack: [%s]", joinedValues));

	MemFree(values);
}

void drawMemory(CPU* cpu, BYTE memory[], DebugMenuContext* context) {
	if (context->autoMoveMemory) {
		context->memoryScrollOffset.x = cpu->programCounter * -MEMEORY_BOX_SIZE;
	}

	GuiScrollPanel(
		context->memoryScrollPanel, "Memory", context->memoryScrollViewSize,
		&context->memoryScrollOffset, &context->memoryScrollView
	);

	BeginScissorMode(
		context->memoryScrollView.x, context->memoryScrollView.y,
		context->memoryScrollView.width, context->memoryScrollView.height
	);

	Rectangle labelRect = {
		context->memoryScrollView.x + context->memoryScrollOffset.x,
		context->memoryScrollView.y,
		48, 16,
	};

	Rectangle valueBoxRect = {
		context->memoryScrollView.x + context->memoryScrollOffset.x,
		context->memoryScrollView.y + 16,
		48, 16
	};

	// The index from which to start drawing memory boxes
	int drawStart = -context->memoryScrollOffset.x/MEMEORY_BOX_SIZE;
	valueBoxRect.x += drawStart * MEMEORY_BOX_SIZE;
	labelRect.x += drawStart * MEMEORY_BOX_SIZE;

	for (int i = drawStart; i < drawStart + 12; i++) {
		GuiLabel(labelRect, TextFormat("0x%03X", i));

		int value = memory[i];

		bool clicked = GuiValueBox(
			valueBoxRect, NULL, &value, 0, 255, 
			context->selectedMemoryBox == i
		);

		if (clicked) {
			unselectAllInputBoxes(context);
			context->selectedMemoryBox = i;
		}

		memory[i] = value;

		valueBoxRect.x += MEMEORY_BOX_SIZE;
		labelRect.x += MEMEORY_BOX_SIZE;
	}

	EndScissorMode();

	GuiCheckBox(
		context->autoMoveCheckBoxRect,
		"Automatically Move Memory View to Program Counter",
		&context->autoMoveMemory
	);
}

void drawKeyboard(DebugMenuContext* context) {
	BYTE keys[] = {
		0x1, 0x2, 0x3, 0xC,
		0x4, 0x5, 0x6, 0xD,
		0x7, 0x8, 0x9, 0xE,
		0xA, 0x0, 0xB, 0xF,
	};

	guiReleasedKey = NO_PRESSED_KEY;
	bool keyPressed = false;

	for (int i = 0; i < 16; i++) {
		Rectangle keyRect = {
			context->menuRect.x + 418 + (i % 4) * 38, 
			context->menuRect.y + 24 + (i / 4) * 38,
			30, 30
		};

		GuiButton(keyRect, TextFormat("%X", keys[i]));

		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			Vector2 mousePos = GetMousePosition();

			if (CheckCollisionPointRec(mousePos, keyRect)) {
				guiHeldKey = keys[i];
				keyPressed = true;
			}
		}
	}

	if (!keyPressed) {
		guiReleasedKey = guiHeldKey;
		guiHeldKey = NO_PRESSED_KEY;
	}
}

void drawDebugMenu(CPU* cpu, BYTE memory[], DebugMenuContext* context) {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		unselectAllInputBoxes(context);
	}

	DrawRectangleRec(
		context->menuRect, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))
	);

	GuiGroupBox(context->menuRect, "Debug Menu");

	drawRegisters(cpu, context);
	drawOtherVaribles(cpu, context);
	drawStack(cpu, context);
	drawMemory(cpu, memory, context);
	drawKeyboard(context);
}