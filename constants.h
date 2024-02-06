#pragma once

#define NULL ((void *)0)

// GUI/Drawing Consts
#define WINDOW_BG_COLOR (Color) {10, 10, 10, 255}

// Chip-8 Consts
#define W 64 // SCREEN_WIDTH
#define H 32 // SCREEN_HEIGHT

#define CPU_CLOCK_PERIOD 0.00125 // 800Hz
#define TIME_PERIOD_OF_60HZ 0.01666
#define MEM_SIZE 4096
#define NUM_OF_REGISTERS 16
#define ROM_ADDRESS 512
#define FONT_ADDRESS 0x50

typedef unsigned char BYTE;