#include <3ds.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	const char* ERASER = "\x1b[40m \x1b[0m";
	const int MIN_COLOR = 41;
	const int MAX_COLOR = 47;
	
	gfxInitDefault();
	hidInit();
	mcuHwcInit();

	PrintConsole topScreen, bottomScreen;

	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	consoleSelect(&bottomScreen);
	printf("\x1b[0;0HSwitch between Pen and Eraser: \x1b[36mA\x1b[0m");
	printf("\x1b[2;0HClear the whole screen: \x1b[36mSelect\x1b[0m");
	printf("\x1b[3;0HNavigate colors: \x1b[36mUp\x1b[0m or \x1b[36mDown\x1b[0m");
	printf("\x1b[30;3HPress \x1b[31mStart\x1b[0m to exit the application.");
	
	bool isPen = true;
	int color = MAX_COLOR;
	
	touchPosition touch;
	u32 kDown;
	while (aptMainLoop())
	{
		hidScanInput();
		kDown = hidKeysDown();
		
		if (kDown & KEY_START) break;
		if (kDown & KEY_A) isPen = !isPen;
		if (kDown & KEY_DOWN)
		{
			if (color == MAX_COLOR) color = MIN_COLOR - 1;
			color++;
		}
		if (kDown & KEY_UP)
		{
			if (color == MIN_COLOR) color = MAX_COLOR + 1;
			color--;
		}
		if (kDown & KEY_SELECT) consoleClear();
		
		if (hidKeysHeld() & KEY_TOUCH)
		{
			hidTouchRead(&touch);
			
			printf("\x1b[%d;%dH", (int)(touch.py / 7.5), ((int)(touch.px / 7.5)) + 5);
			
			if (isPen) printf("\x1b[%dm \x1b[0m", color);
			else printf(ERASER);
		}
		
		consoleSelect(&bottomScreen);
		printf("\x1b[5;0HCurrent Mode: %s", isPen ? "\x1b[32mPen   \x1b[0m" : "\x1b[31mEraser\x1b[0m");
		printf("\x1b[6;0HCurrent Color: \x1b[%dm \x1b[0m", color);

		consoleSelect(&topScreen);
		for (int row = 0; row <= 40; row++)
		{
			printf("\x1b[%d;4H|", row);
			printf("\x1b[%d;47H|", row);
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	mcuHwcExit();
	hidExit();
	gfxExit();
	return 0;
}
