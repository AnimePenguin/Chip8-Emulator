#define SUPPORT_SCROLLBAR_KEY_INPUT
#define RAYGUI_IMPLEMENTATION

#include "external\raygui.h"

#include "emulator.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main(int argc, char* argv[]) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chip-8 Emulator");
	SetWindowMinSize(512, 256);

	InitAudioDevice();
	SetMasterVolume(0.25);

	GuiLoadStyle("resources/style_jungle.rgs");
	GuiSetStyle(SCROLLBAR, SCROLL_SPEED, 10);
	GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 18);

	char* fileName = NULL;

	if (argc > 1) {
		
		if (FileExists(argv[1])) {
			fileName = argv[1];
		} else {
			TraceLog(LOG_ERROR, TextFormat("\"%s\" does not Exist", argv[1]));
		}

	}

	initEmulator(fileName);

	CloseAudioDevice();
	CloseWindow();

	return 0;
}