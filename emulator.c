#include <stdbool.h>

#include "external/raylib.h"

#include "emulator.h"
#include "cpu.h"

// Drawing Consts
#define CHIP8_BG_COLOR BLACK
#define CHIP8_PIXEL_COLOR WHITE

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

	for (int i = 0; i <= dataSize; i++) {
		memory[ROM_ADDRESS + i] = fileData[i];
	}

	UnloadFileData(fileData);
}

Rectangle getDrawArea() {
	int width = GetScreenWidth();
	int height = GetScreenHeight();

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
	BeginDrawing();

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

	EndDrawing();
}

void initEmulator(char* fileName) {
	Sound beep = LoadSound("resources/beep.wav");

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

	Rectangle drawArea = getDrawArea();

	double accClockTime = 0.0;
	double acc60HzTime = 0.0;

	double last_time = GetTime();

	bool halted = false;

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_SPACE)) {
			halted = !halted;
		} else if (GetKeyPressed() == KEY_P) {
			goto emulator_start; // Restart this function
		}

		if (IsWindowResized()) {
			drawArea = getDrawArea();
		}

		if (halted) {
			drawScreen(screenBuffer, &drawArea);
			continue;
		}

		double delta = GetTime() - last_time;
		last_time = GetTime();

		accClockTime += delta;
		acc60HzTime += delta;

		if (accClockTime >= CLOCK_PERIOD) {
			accClockTime = 0;

			doCPUCycle(&cpu, memory, screenBuffer);
		}

		if (acc60HzTime >= 0.01666) {
			acc60HzTime = 0;

			if (cpu.delayTimer) {
				cpu.delayTimer--;
			}

			if (cpu.soundTimer) {
				cpu.soundTimer--;
				PlaySound(beep);
			}

			drawScreen(screenBuffer, &drawArea);
		}
	}
	
	UnloadSound(beep);
}