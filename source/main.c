#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

typedef struct color
{
	u8 R;
	u8 G;
	u8 B;
} color_rgb;

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

void printControls(PrintConsole* screen)
{
	consoleSelect(screen);

	printf("\x1b[0;0HSwitch between Pen and Eraser: \x1b[36mA\x1b[0m");
	printf("\x1b[2;0HClear the whole screen: \x1b[36mSelect\x1b[0m");
	printf("\x1b[3;0HNavigate colors: \x1b[36mD-Pad\x1b[0m");
	printf("\x1b[30;8HPress \x1b[31mStart\x1b[0m to exit the application");
}

void simpleMode(PrintConsole* topScreen)
{
	const int MIN_COLOR = 41;
	const int MAX_COLOR = 47;

	PrintConsole bottomScreen;
	
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	printControls(topScreen);

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

		consoleSelect(topScreen);
		printf("\x1b[5;0HCurrent Mode: %s", isPen ? "\x1b[32mPen   \x1b[0m" : "\x1b[31mEraser\x1b[0m");
		printf("\x1b[6;0HCurrent Color: \x1b[%dm \x1b[0m", color);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();

		prevTouch = currentTouch;
	}
}

void advancedMode(PrintConsole* topScreen)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	u32 clrClear = C2D_Color32(0x00, 0x00, 0x00, 0xFF);

	consoleClear();
	C2D_TargetClear(bottom, clrClear);

	printControls(topScreen);

	bool isPen = true;
	u8 selectedColorIndex = 0;
	u8 prevSelectedColorIndex = 1;
	color_rgb color;
	color.R = 255;
	color.G = 255;
	color.B = 255;

	u32 clrCurrent = C2D_Color32(color.R, color.G, color.B, 0xFF);
	u32 clrDraw = clrCurrent;
	
	touchPosition currentTouch;
	touchPosition prevTouch;
	u32 kDown;
	u32 kHeld;
	while (aptMainLoop())
	{
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();

		if (kDown & KEY_START) break;
		if (kDown & KEY_A) isPen = !isPen;
		if (kDown & KEY_UP)
		{
			prevSelectedColorIndex = selectedColorIndex;
			selectedColorIndex = (selectedColorIndex + 2) % 3;
		}
		if (kDown & KEY_DOWN)
		{
			prevSelectedColorIndex = selectedColorIndex;
			selectedColorIndex = (selectedColorIndex + 1) % 3;
		}
		if ((kDown & KEY_LEFT) || (kHeld & KEY_LEFT))
		{
			switch (selectedColorIndex)
			{
				case 0: color.R--; break;
				case 1: color.G--; break;
				case 2: color.B--; break;
			}

			clrCurrent = C2D_Color32(color.R, color.G, color.B, 0xFF);
		}
		if ((kDown & KEY_RIGHT) || (kHeld & KEY_RIGHT))
		{
			switch (selectedColorIndex)
			{
				case 0: color.R++; break;
				case 1: color.G++; break;
				case 2: color.B++; break;
			}

			clrCurrent = C2D_Color32(color.R, color.G, color.B, 0xFF);
		}
		if (kDown & KEY_SELECT) C2D_TargetClear(bottom, clrClear);
		if (kDown & KEY_TOUCH) hidTouchRead(&prevTouch);

		clrDraw = isPen ? clrCurrent : clrClear;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_SceneBegin(bottom);

		if (kHeld & KEY_TOUCH)
		{
			hidTouchRead(&currentTouch);

			C2D_DrawLine(prevTouch.px, prevTouch.py, clrDraw,
				currentTouch.px, currentTouch.py, clrDraw, 2, 0);
		}
		
		C3D_FrameEnd(0);

		printf("\x1b[5;0HCurrent Mode: %s", isPen ? "\x1b[32mPen   \x1b[0m" : "\x1b[31mEraser\x1b[0m");
		printf("\x1b[6;0HCurrent Color");
		printf("\x1b[%d;0H ", prevSelectedColorIndex + 7);
		printf("\x1b[%d;0H>", selectedColorIndex + 7);

		printf("\x1b[7;2H\x1b[31m%u\x1b[0m", color.R);
		printf("\x1b[8;2H\x1b[32m%u\x1b[0m", color.G);
		printf("\x1b[9;2H\x1b[33m%u\x1b[0m", color.B);

		prevTouch = currentTouch;
	}

	C2D_Fini();
	C3D_Fini();
}

int main(int argc, char **argv)
{
	gfxInitDefault();
	hidInit();
	mcuHwcInit();

	PrintConsole topScreen;
	consoleInit(GFX_TOP, &topScreen);

	advancedMode(&topScreen);

	mcuHwcExit();
	hidExit();
	gfxExit();
	return 0;
}
