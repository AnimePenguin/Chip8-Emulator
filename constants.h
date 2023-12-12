#pragma once

typedef unsigned char BYTE;

#define NULL ((void *)0)

// GUI Consts
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_BACKGROUND_COLOR (Color) {10, 10, 10, 255}

// Chip-8 Consts
#define W 64 // SCREEN_WIDTH
#define H 32 // SCREEN_HEIGHT

#define CLOCK_PERIOD 0.00124 // 800Hz
#define MEM_SIZE 4096
#define ROM_ADDRESS 512
#define FONT_ADDRESS 0x50