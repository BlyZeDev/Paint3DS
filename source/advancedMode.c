#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "simpleMode.h"

typedef struct color
{
	u8 R;
	u8 G;
	u8 B;
} color_rgb;

void addToRgb(color_rgb* color, int selectedColorIndex, int value)
{
	switch (selectedColorIndex)
	{
		case 0: color->R += value; break;
		case 1: color->G += value; break;
		case 2: color->B += value; break;
	}
}

void printAdvancedControls()
{
	printf("\x1b[3;23H and \x1b[36mL\x1b[0m or \x1b[36mR\x1b[0m");
	printf("\x1b[4;0HAdjust Pen size: \x1b[36mSound Slider\x1b[0m");
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

	consoleSelect(topScreen);
	printSimpleControls();
	printAdvancedControls();

	printf("\x1b[6;0HCurrent Mode: ");
	printf("\x1b[7;0HCurrent Pen Size: ");
	printf("\x1b[8;0HCurrent Color");

	bool isPen = true;
	u8 selectedColorIndex = 0;
	u8 prevSelectedColorIndex = 1;
	color_rgb color = { 255, 255, 255 };

	u32 clrDraw = C2D_Color32(color.R, color.G, color.B, 0xFF);
	
	touchPosition currentTouch;
	touchPosition prevTouch;
	u32 kDown;
	u32 kHeld;
	u8 currentPenSize;
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
		if (kDown & KEY_LEFT) addToRgb(&color, selectedColorIndex, -1);
		if (kDown & KEY_RIGHT) addToRgb(&color, selectedColorIndex, 1);
		if (kDown & (KEY_L | KEY_ZL)) addToRgb(&color, selectedColorIndex, -10);
		if (kDown & (KEY_R | KEY_ZR)) addToRgb(&color, selectedColorIndex, 10);
		if (kDown & KEY_SELECT) C2D_TargetClear(bottom, clrClear);
		if (kDown & KEY_TOUCH) hidTouchRead(&prevTouch);

		MCUHWC_GetSoundSliderLevel(&currentPenSize);
		currentPenSize /= 4;
		currentPenSize++;

		clrDraw = isPen ? C2D_Color32(color.R, color.G, color.B, 0xFF) : clrClear;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_SceneBegin(bottom);

		if (kHeld & KEY_TOUCH)
		{
			hidTouchRead(&currentTouch);

			C2D_DrawLine(prevTouch.px, prevTouch.py, clrDraw,
				currentTouch.px, currentTouch.py, clrDraw, currentPenSize, 0);

			C2D_DrawEllipseSolid(
				currentTouch.px - currentPenSize / 2, currentTouch.py - currentPenSize / 2, 0,
				currentPenSize - (currentPenSize % 2), currentPenSize - (currentPenSize % 2), clrDraw);
		}
		
		C3D_FrameEnd(0);

		printf("\x1b[6;15H%s", isPen ? "\x1b[32mPen   \x1b[0m" : "\x1b[31mEraser\x1b[0m");

		printf("\x1b[7;19H\x1b[31m%u  \x1b[0m", currentPenSize);

		printf("\x1b[%d;0H ", prevSelectedColorIndex + 9);
		printf("\x1b[%d;0H>", selectedColorIndex + 9);

		printf("\x1b[9;2H\x1b[31m%u  \x1b[0m", color.R);
		printf("\x1b[10;2H\x1b[32m%u  \x1b[0m", color.G);
		printf("\x1b[11;2H\x1b[34m%u  \x1b[0m", color.B);

		prevTouch = currentTouch;
	}

	C2D_Fini();
	C3D_Fini();
}