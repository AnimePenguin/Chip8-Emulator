#pragma once

#include "constants.h"
#include "stack.h"

typedef struct CPU {
	unsigned int programCounter;
	unsigned short indexRegister;

	Stack stack;

	BYTE registers[16];

	BYTE delayTimer;
	BYTE soundTimer;
} CPU;

void doCPUCycle(CPU* cpu, BYTE memory[], BYTE screenBuffer[H][W]);