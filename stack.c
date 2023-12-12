#include "stack.h"

void stackPush(Stack* stack, unsigned short value) {
	stack->index++;
	stack->values[stack->index] = value;
}

unsigned short stackPop(Stack* stack) {
	unsigned short value = stack->values[stack->index];
	stack->index--;

	return value;
}