#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

void drawLine(u16 startX, u16 startY, u16 endX, u16 endY, int color)
{
	int dx = abs(endX - startX);
	int dy = abs(endY - startY);

	int sx = (startX < endX) ? 1 : -1;
	int sy = (startY < endY) ? 1 : -1;

	int err = dx - dy;
	int err2;

	while (true)
	{
		printf("\x1b[%d;%dH", startX, startY);
		printf("\x1b[%dm \x1b[0m", color);

		if (startX == endX && startY == endY) break;

		err2 = err * 2;

		if (err2 > -dy)
		{
			err -= dy;
			startX += sx;
		}

		if (err2 < dx)
		{
			err += dx;
			startY += sy;
		}
	}
}

void printSimpleControls()
{
	printf("\x1b[0;0HSwitch between Pen and Eraser: \x1b[36mA\x1b[0m");
	printf("\x1b[2;0HClear the whole screen: \x1b[36mSelect\x1b[0m");
	printf("\x1b[3;0HNavigate colors: \x1b[36mD-Pad\x1b[0m");
	printf("\x1b[30;8HPress \x1b[31mStart\x1b[0m to exit the application");
}

void simpleMode(PrintConsole* topScreenPtr)
{
	const int MIN_COLOR = 41;
	const int MAX_COLOR = 47;

	PrintConsole bottomScreen;
	
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	consoleSelect(topScreenPtr);
	printSimpleControls(topScreenPtr);

	bool isPen = true;
	int color = MAX_COLOR;

	touchPosition currentTouch;
	touchPosition prevTouch;
	u32 kDown;
	while (aptMainLoop())
	{
		hidScanInput();
		kDown = hidKeysDown();

		consoleSelect(&bottomScreen);

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
		if (kDown & KEY_TOUCH) hidTouchRead(&prevTouch);

		if (hidKeysHeld() & KEY_TOUCH)
		{
			hidTouchRead(&currentTouch);

			drawLine((int)(prevTouch.py / 7.5), (int)(prevTouch.px / 7.5),
				(int)(currentTouch.py / 7.5), (int)(currentTouch.px / 7.5), isPen ? color : 40);
		}

		consoleSelect(topScreenPtr);
		printf("\x1b[5;0HCurrent Mode: %s", isPen ? "\x1b[32mPen   \x1b[0m" : "\x1b[31mEraser\x1b[0m");
		printf("\x1b[6;0HCurrent Color: \x1b[%dm \x1b[0m", color);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();

		prevTouch = currentTouch;
	}
}