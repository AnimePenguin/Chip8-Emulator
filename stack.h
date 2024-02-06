#pragma once

#define STACK_SIZE 16

typedef struct Stack {
	unsigned short values[STACK_SIZE];
	int index;
} Stack;

void stackPush(Stack* stack, unsigned short value);
unsigned short stackPop(Stack* stack);