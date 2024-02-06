#include "stack.h"

void stackPush(Stack* stack, unsigned short value) {
	stack->values[stack->index] = value;
	stack->index++;
}

unsigned short stackPop(Stack* stack) {
	stack->index--;
	return stack->values[stack->index];
}