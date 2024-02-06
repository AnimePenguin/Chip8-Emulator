#pragma once

#include "constants.h"
#include "stack.h"

typedef struct CPU {
	unsigned short programCounter;
	unsigned short indexRegister;

	Stack stack;

	BYTE registers[NUM_OF_REGISTERS];

	BYTE delayTimer;
	BYTE soundTimer;
} CPU;

void doCPUCycle(CPU* cpu, BYTE memory[], BYTE screenBuffer[H][W]);