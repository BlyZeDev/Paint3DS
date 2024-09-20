#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "simpleMode.h"
#include "advancedMode.h"

typedef enum mode
{
	NONE,
	SIMPLE,
	ADVANCED
} mode;

int main(int argc, char **argv)
{
	gfxInitDefault();
	hidInit();
	mcuHwcInit();

	PrintConsole topScreen;
	consoleInit(GFX_TOP, &topScreen);
	
	printf("\x1b[14;12HPress \x1b[36mA\x1b[0m for Advanced Mode");
	printf("\x1b[16;12HPress \x1b[36mL\x1b[0m+\x1b[36mR\x1b[0m for Simple Mode");
	printf("\x1b[19;8HPress \x1b[31mStart\x1b[0m to exit the application");

	mode mode = NONE;

	u32 kDown;
	u32 kHeld;
	while (aptMainLoop())
	{
		hidScanInput();

		kDown = hidKeysDown();
		kHeld = hidKeysHeld();

		if (kDown & KEY_START) break;
		if (kDown & KEY_A)
		{
			mode = ADVANCED;
			break;
		}
		if ((kHeld & (KEY_L | KEY_ZL)) && (kHeld & (KEY_R | KEY_ZR)))
		{
			mode = SIMPLE;
			break;
		}
	}

	consoleClear();

	switch (mode)
	{
		case SIMPLE: simpleMode(&topScreen); break;
		case ADVANCED: advancedMode(&topScreen); break;
		default: break;
	}

	mcuHwcExit();
	hidExit();
	gfxExit();
	return 0;
}
