#include <stdbool.h>

#include "external\raygui.h"

#include "emulator.h"

#define DIALOG_WIDTH 480
#define DIALOG_HEIGHT 460

#define DIRECTORY_ITEM_HEIGHT 20

Rectangle getWindowBoxRect() {
	Rectangle windowBoxRect = {
		(GetScreenWidth() - DIALOG_WIDTH)/2, 
		(GetScreenHeight() - DIALOG_HEIGHT)/2,
		DIALOG_WIDTH, DIALOG_HEIGHT,
	};

	return windowBoxRect;
}

Rectangle getHelpLabelRect() {
	Rectangle helpLabelRect = {
		(GetScreenWidth() - DIALOG_WIDTH)/2 + 10,
		(GetScreenHeight() - DIALOG_HEIGHT)/2 + 32,
		230, 24,
	};

	return helpLabelRect;
}

Rectangle getScrollPanelRect() {
	Rectangle scrollPanelRect = {
		(GetScreenWidth() - DIALOG_WIDTH)/2 + 10,
		(GetScreenHeight() - DIALOG_HEIGHT)/2 + 56,
		DIALOG_WIDTH - 20, DIALOG_HEIGHT - 95,
	};

	return scrollPanelRect;
}

Rectangle getCheckBoxRect() {
	Rectangle checkBoxRect = {
		(GetScreenWidth() - DIALOG_WIDTH)/2 + 10,
		(GetScreenHeight() + DIALOG_HEIGHT)/2 - 26,
		16, 16,
	};

	return checkBoxRect;
}

Rectangle getRefreshRect() {
	Rectangle refreshRect = {
		(GetScreenWidth() + DIALOG_WIDTH)/2 - 30,
		(GetScreenHeight() + DIALOG_HEIGHT)/2 - 28,
		20, 20,
	};

	return refreshRect;
}

// Returns approximate pixel height for all directory items
int refreshDirectory(FilePathList* pathItems) {
	if (pathItems->count != 0 && pathItems != NULL) {
		UnloadDirectoryFiles(*pathItems);
	}
	
	*pathItems = LoadDirectoryFiles(GetWorkingDirectory());
	return (pathItems->count + 2) * DIRECTORY_ITEM_HEIGHT;
}

char* openFileDialog() {
	char* file = NULL;

	SetTargetFPS(60);

	Rectangle windowBoxRect = getWindowBoxRect();
	Rectangle helpLabelRect = getHelpLabelRect();

	Rectangle scrollPanelRect = getScrollPanelRect();
	Rectangle scrollViewSize = {0, 0, DIALOG_WIDTH - 40, 0};
	Vector2 scrollOffset = {0, 0};
	Rectangle scrollView = {0, 0, 0, 0};

	Rectangle checkBoxRect = getCheckBoxRect();
	Rectangle refreshRect = getRefreshRect();

	bool dialogClosed = false;
	bool showAllFiles = false;

	FilePathList pathItems = {};
	scrollViewSize.height = refreshDirectory(&pathItems);

	while (!WindowShouldClose() && !dialogClosed) {

		if (IsWindowResized()) {
			windowBoxRect = getWindowBoxRect();
			helpLabelRect = getHelpLabelRect();
			scrollPanelRect = getScrollPanelRect();
			checkBoxRect = getCheckBoxRect();
			refreshRect = getRefreshRect();
		}

		BeginDrawing();

			ClearBackground(WINDOW_BG_COLOR);

			dialogClosed = GuiWindowBox(windowBoxRect, "File Dialog");
			GuiLabel(helpLabelRect, "Click to Select a ROM (*.ch8)");

			GuiCheckBox(checkBoxRect, "Show All Files", &showAllFiles);

			if (GuiButton(refreshRect, "#211#")) {
				scrollViewSize.height = refreshDirectory(&pathItems);
			}

			GuiScrollPanel(
				scrollPanelRect, NULL, scrollViewSize,
				&scrollOffset, &scrollView
			);

			BeginScissorMode(
				scrollView.x, scrollView.y,
				scrollView.width, scrollView.height
			);

			Rectangle itemRect = {
				scrollView.x + scrollOffset.x + 5, 
				scrollView.y + scrollOffset.y,
				scrollViewSize.width, DIRECTORY_ITEM_HEIGHT
			};

			if (GuiLabelButton(itemRect, "#3#Go Up a Directory (..)")) {
				ChangeDirectory("..");
				scrollViewSize.height = refreshDirectory(&pathItems);
			}

			itemRect.y += 20;

			for (int i = 0; i < pathItems.count; i++) {
				char* pathItem = pathItems.paths[i];

				const char* fileName = GetFileName(pathItem);

				if (IsPathFile(pathItem)) {

					if (IsFileExtension(fileName, ".ch8")) {
						fileName = TextFormat("#13#%s", fileName);
					} else if (showAllFiles) {
						fileName = TextFormat("#10#%s", fileName);
					} else {
						continue;
					}
					
				} else {
					fileName = TextFormat("#1#%s", fileName);
				}

				// Don't draw the buttons if they are out of the scroll panel
				bool isOnScreen = itemRect.y < scrollView.y + scrollView.height;
				isOnScreen = isOnScreen && (itemRect.y + 20) > scrollView.y;

				if (isOnScreen && GuiLabelButton(itemRect, fileName)) {

					if (IsPathFile(pathItem)) {
						file = MemAlloc((TextLength(pathItem) + 1) * sizeof(char));
						TextCopy(file, pathItem);

						dialogClosed = true;
						break;
					}

					ChangeDirectory(pathItem);
					scrollViewSize.height = refreshDirectory(&pathItems);

					break;
				}

				itemRect.y += DIRECTORY_ITEM_HEIGHT;
			}

			EndScissorMode();
			
		EndDrawing();
	}

	UnloadDirectoryFiles(pathItems);
	ChangeDirectory(GetApplicationDirectory());
	SetTargetFPS(0); // Remove limit for FPS

	return file;
}