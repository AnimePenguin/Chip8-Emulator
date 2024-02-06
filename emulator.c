#include <stdbool.h>

#include "emulator.h"
#include "debug_menu.h"
#include "cpu.h"

#define FALLBACK_ROM "resources/no_rom.ch8"

#define CHIP8_BG_COLOR BLACK
#define CHIP8_PIXEL_COLOR WHITE

#define PAUSE_KEY KEY_SPACE
#define SLOW_MODE_KEY KEY_L
#define DEBUG_KEY KEY_SEMICOLON
#define CHANGE_ROM_KEY KEY_O
#define RESTART_KEY KEY_P

const BYTE FONT[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void loadRomToMemory(char* fileName, BYTE memory[]) {
	int dataSize;
	BYTE* fileData = LoadFileData(fileName, &dataSize);

	if (dataSize >= (MEM_SIZE - ROM_ADDRESS)) {
		TraceLog(LOG_FATAL, "ROM exceeds Chip-8 memory size");
	}

	for (int i = 0; i <= dataSize; i++) {
		memory[ROM_ADDRESS + i] = fileData[i];
	}

	UnloadFileData(fileData);
}

Rectangle getDrawArea(bool useHalfScreen) {
	int width = GetScreenWidth();
	int height = GetScreenHeight();

	if (useHalfScreen) {
		height /= 2;
	}

	Vector2 drawSize = {width, width/2};

	if (height*2 < width) {
		drawSize = (Vector2){height*2, height};
	}

	drawSize.x -= (int)drawSize.x % W;
	drawSize.y -= (int)drawSize.y % H;

	Vector2 drawPos = {
		(width - drawSize.x)/2,
		(height - drawSize.y)/2,
	};

	Rectangle drawArea = { 
		drawPos.x, drawPos.y,
		drawSize.x, drawSize.y,
	};

	return drawArea;
}

void drawScreen(BYTE screenBuffer[H][W], const Rectangle* drawArea) {
	ClearBackground(WINDOW_BG_COLOR);
	DrawRectangleRec(*drawArea, CHIP8_BG_COLOR);

	int blockSize = drawArea->width / W;

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			if (!screenBuffer[y][x]) {
				continue;
			}

			DrawRectangle(
				drawArea->x + x*blockSize,
				drawArea->y + y*blockSize, 
				blockSize, blockSize, CHIP8_PIXEL_COLOR
			);
		}
	}
}

void initEmulator(char* fileName) {
	Sound beep = LoadSound("resources/beep.wav");

	if (fileName == NULL) {
		fileName = openFileDialog();

		if (fileName == NULL) {
			fileName = MemAlloc((TextLength(FALLBACK_ROM) + 1) * sizeof(char));
			TextCopy(fileName, FALLBACK_ROM);
		}
	}

	double clockPeriod = CPU_CLOCK_PERIOD;

	bool slowMode = false;
	bool showDebugMenu = false;

	bool shouldResizeWindow = false;

	Rectangle drawArea = getDrawArea(showDebugMenu);

	Rectangle menuDrawArea = getDrawArea(true);
	menuDrawArea.y += menuDrawArea.height;

	DebugMenuContext debugContext = newDebugMenuContext(&menuDrawArea);

	emulator_start:
	CPU cpu = {
		.programCounter = ROM_ADDRESS,
		.indexRegister = 0,
		.stack = {},
		.registers = {0},
		.delayTimer = 0,
		.soundTimer = 0,
	};

	// Memory needs ROM and Font loaded on to it.
	BYTE memory[MEM_SIZE] = {0};

	// Load Font to memory at FONT_ADDRESS
	for (int i = 0; i < 80; i++) {
		memory[FONT_ADDRESS + i] = FONT[i];
	}

	loadRomToMemory(fileName, memory);

	BYTE screenBuffer[H][W] = {0};

	double accClockTime = 0.0;
	double acc60HzTime = 0.0;

	double last_time = GetTime();

	bool paused = false;

	while (!WindowShouldClose()) {

		shouldResizeWindow = IsWindowResized();

		switch (GetKeyPressed()) {
			case PAUSE_KEY:
				paused = !paused;
				break;

			case SLOW_MODE_KEY:
				slowMode = !slowMode;
				clockPeriod = slowMode ? TIME_PERIOD_OF_60HZ : CPU_CLOCK_PERIOD;

				break;

			case DEBUG_KEY:
				showDebugMenu = !showDebugMenu;
				shouldResizeWindow = true;

				break;

			case CHANGE_ROM_KEY:
				char* newFileName = openFileDialog();

				shouldResizeWindow = true;

				if (newFileName != NULL) {
					MemFree(fileName);
					fileName = newFileName;

					goto emulator_start;
				}

				break;

			case RESTART_KEY:
				goto emulator_start; // Restart Chip-8 components
				break;
		}

		if (shouldResizeWindow) {
			drawArea = getDrawArea(showDebugMenu);

			menuDrawArea = getDrawArea(true);
			menuDrawArea.y += menuDrawArea.height;

			resizeDebugMenu(&menuDrawArea, &debugContext);
		}

		double delta = GetTime() - last_time;
		last_time = GetTime();

		accClockTime += delta;
		acc60HzTime += delta;

		if (accClockTime >= clockPeriod && !paused) {
			accClockTime = 0;

			doCPUCycle(&cpu, memory, screenBuffer);
		}

		if (acc60HzTime >= TIME_PERIOD_OF_60HZ) {

			BeginDrawing();

				drawScreen(screenBuffer, &drawArea);
				
				if (showDebugMenu) {
					drawDebugMenu(&cpu, memory, &debugContext);
				}

				if (paused) {
					DrawText("Paused", 4, 4, 16, GRAY);
				} else if (slowMode) {
					DrawText("Slow Mode", 4, 4, 16, GRAY);
				}

			EndDrawing();

			if (!paused) {
				acc60HzTime = 0;

				if (cpu.delayTimer) {
					cpu.delayTimer--;
				}

				if (cpu.soundTimer) {
					cpu.soundTimer--;
					PlaySound(beep);
				}
			}

		}
	}
	
	MemFree(fileName);
	UnloadSound(beep);
}