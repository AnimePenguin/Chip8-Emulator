EXE_NAME := Chip8Emulator
C_FILES := main.c emulator.c file_dialog.c stack.c cpu.c
C_HEADERS := constants.h emulator.h stack.h cpu.h
C_FLAGS := -Wall -std=c99 -Wno-missing-braces
EXTERNAL_INCLUDES := -I external/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

EXAMPLE_ROM := "Breakout (Brix hack) [David Winter, 1997].ch8"

run: compile
	$(EXE_NAME) $(EXAMPLE_ROM)

compile: $(C_FILES) $(C_HEADERS)
	gcc $(C_FILES) -o $(EXE_NAME) $(C_FLAGS) $(ARGS) $(EXTERNAL_INCLUDES)

release: $(C_FILES) $(C_HEADERS)
	$(MAKE) compile ARGS=-O3

gdb: $(C_FILES) $(C_HEADERS)
	$(MAKE) compile ARGS=-g
	gdb --args $(EXE_NAME) $(EXAMPLE_ROM)
