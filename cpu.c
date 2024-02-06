#include <stdbool.h>

#include "external/raylib.h"

#include "cpu.h"
#include "key.h"

void clearScreenBuffer(BYTE screenBuffer[H][W]) {
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			screenBuffer[y][x] = 0;
		}
	}
}

// Opcode D instruction
void drawSprite(BYTE nibble[], CPU* cpu, BYTE memory[], BYTE screenBuffer[H][W]) {
	char x = cpu->registers[nibble[1]] % W;
	char y = cpu->registers[nibble[2]] % H;

	cpu->registers[0xF] = 0;

	for (int i = 0; i < nibble[3]; i++) {
		BYTE row = memory[cpu->indexRegister + i];
		int spriteY = y + i;

		if (spriteY >= H) {
			break;
		}

		for (int j = 0; j < 8; j++) {
			int spriteX = x + (7 - j);

			if (spriteX < 0 || spriteX >= W) {
				continue;
			}

			if (row & (1 << j)) {
				if (screenBuffer[spriteY][spriteX]) {
					cpu->registers[0xF] = 1;
					screenBuffer[spriteY][spriteX] = 0;
				} else {
					screenBuffer[spriteY][spriteX] = 1;
				}
			}
		}
	}

}

// Opcode 8 instructions
void doALUCalculation(BYTE nibble[], BYTE registers[]) {
	BYTE* VX = &registers[nibble[1]];
	BYTE* VY = &registers[nibble[2]];
	BYTE* VF = &registers[0xF];

	BYTE carry;

	switch (nibble[3]) {
		case 0x0:
			*VX = *VY;
			break;

		case 0x1:
			*VX |= *VY;
			break;

		case 0x2:
			*VX &= *VY;
			break;

		case 0x3:
			*VX ^= *VY;
			break;

		case 0x4:
			carry = (255 - *VX) < *VY ? 1 : 0;

			*VX += *VY;
			*VF = carry;
			break;

		case 0x5:
			carry = *VX >= *VY ? 1 : 0;

			*VX -= *VY;
			*VF = carry;
			break;

		case 0x6:
			carry = *VX & 1;

			*VX >>= 1;
			*VF = carry;
			break;

		case 0x7:
			carry = *VY >= *VX ? 1 : 0;

			*VX = *VY - *VX;
			*VF = carry;
			break;

		case 0xE:
			carry = *VX >> 7;

			*VX <<= 1;
			*VF = carry;
			break;
	}
}

// Opcode F instructions
void miscFunctions(BYTE NN, BYTE nibble[], CPU* cpu, BYTE memory[]) {
	BYTE* VX = &cpu->registers[nibble[1]];

	switch (NN) {
		case 0x0A:
			BYTE key = getKeyInHex(false);

			if (key == NO_PRESSED_KEY) {
				cpu->programCounter -= 2;
			} else {
				*VX = key;
			}

			break;

		case 0x07:
			*VX = cpu->delayTimer;
			break;

		case 0x15:
			cpu->delayTimer = *VX;
			break;

		case 0x18:
			cpu->soundTimer = *VX;
			break;

		case 0x1E:
			cpu->indexRegister += *VX;
			break;

		case 0x29:
			cpu->indexRegister = FONT_ADDRESS + (*VX * 5);
			break;

		case 0x33:
			memory[cpu->indexRegister] = (*VX/100) % 10;
			memory[cpu->indexRegister + 1] = (*VX/10) % 10;
			memory[cpu->indexRegister + 2] = *VX % 10;
			break;

		case 0x55:
			for (int i = 0; i <= nibble[1]; i++) {
				memory[cpu->indexRegister + i] = cpu->registers[i];
			}

			break;

		case 0x65:
			for (int i = 0; i <= nibble[1]; i++) {
				cpu->registers[i] = memory[cpu->indexRegister + i];
			}

			break;
	}
}

void doCPUCycle(CPU* cpu, BYTE memory[], BYTE screenBuffer[H][W]) {
	BYTE nibble[4]; // split instruction into nibbles

	nibble[0] = memory[cpu->programCounter] >> 4 & 0xF;
	nibble[1] = memory[cpu->programCounter] & 0xF;
	nibble[2] = memory[cpu->programCounter + 1] >> 4 & 0xF;
	nibble[3] = memory[cpu->programCounter + 1] & 0xF;

	BYTE NN = memory[cpu->programCounter + 1];
	unsigned short NNN = (nibble[1] << 8) | NN;

	BYTE* VX = &cpu->registers[nibble[1]];
	BYTE* VY = &cpu->registers[nibble[2]];

	cpu->programCounter += 2;

	switch (nibble[0]) {
		case 0x0:
			if (NNN == 0x0E0) {
				clearScreenBuffer(screenBuffer);
			} else if (NNN == 0x0EE) {
				cpu->programCounter = stackPop(&cpu->stack);
			} else {
				TraceLog(LOG_INFO, TextFormat("Call to 0x%03x", NNN));
			}

			break;

		case 0x1:
			cpu->programCounter = NNN;
			break;

		case 0x2:
			stackPush(&cpu->stack, cpu->programCounter);
			cpu->programCounter = NNN;
			break;

		case 0x3:
			if (*VX == NN) {
				cpu->programCounter += 2;
			}

			break;

		case 0x4:
			if (*VX != NN) {
				cpu->programCounter += 2;
			}

			break;

		case 0x5:
			if (*VX == *VY) {
				cpu->programCounter += 2;
			}

			break;

		case 0x6:
			*VX = NN;
			break;

		case 0x7:
			*VX += NN;
			break;

		case 0x8:
			doALUCalculation(nibble, cpu->registers);
			break;

		case 0x9:
			if (*VX != *VY) {
				cpu->programCounter += 2;
			}

			break;

		case 0xA:
			cpu->indexRegister = NNN;
			break;

		case 0xB:
			cpu->programCounter = cpu->registers[0] + NNN;
			break;

		case 0xC:
			*VX = GetRandomValue(0, 255) & NN;
			break;

		case 0xD:
			drawSprite(nibble, cpu, memory, screenBuffer);
			break;

		case 0xE:
			if (NN == 0x9E && getKeyInHex(true) == *VX) {
				cpu->programCounter += 2;

			} else if (NN == 0xA1 && getKeyInHex(true) != *VX) {
				cpu->programCounter += 2;
			}

			break;

		case 0xF:
			miscFunctions(NN, nibble, cpu, memory);
			break;
	}
}