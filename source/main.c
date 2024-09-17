#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "simpleMode.h"
#include "advancedMode.h"

int main(int argc, char **argv)
{
	gfxInitDefault();
	hidInit();
	mcuHwcInit();

	PrintConsole topScreen;
	consoleInit(GFX_TOP, &topScreen);
	
	printf("\x1b[15;12HPress \x1b[36mA\x1b[0m for Advanced Mode");
	printf("\x1b[17;12HPress \x1b[36mL\x1b[0m+\x1b[36mR\x1b[0m for Simple Mode");

	bool isSimpleMode = false;

	u32 kHeld;
	while (aptMainLoop())
	{
		hidScanInput();

		kHeld = hidKeysHeld();

		if (hidKeysDown() & KEY_A) break;
		if ((kHeld & (KEY_L | KEY_ZL)) && (kHeld & (KEY_R | KEY_ZR)))
		{
			isSimpleMode = true;
			break;
		}
	}

	consoleClear();

	if (isSimpleMode) simpleMode(&topScreen);
	else advancedMode(&topScreen);

	mcuHwcExit();
	hidExit();
	gfxExit();
	return 0;
}
