#include "external/raylib.h"

#include "emulator.h"

int main(int argc, char* argv[]) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chip-8 Emulator");
	SetWindowMinSize(512, 256);

	InitAudioDevice();
	SetMasterVolume(0.25);

	char* fileName = NULL;

	if (argc > 1) {
		
		if (FileExists(argv[1])) {
			fileName = argv[1];
		} else {
			TraceLog(LOG_ERROR, TextFormat("\"%s\" does not Exist", argv[1]));
		}

	} else {
		TraceLog(LOG_INFO, "No ROM was Provided");
	}

	if (fileName == NULL) {
		fileName = openFileDialog();
	}

	if (fileName != NULL) {
		while (initEmulator(fileName));
		MemFree(fileName);
	} else {
		initEmulator("resources/no_rom.ch8");
	}

	CloseAudioDevice();
	CloseWindow();

	return 0;
}