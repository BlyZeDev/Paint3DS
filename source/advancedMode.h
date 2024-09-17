#ifndef ADVANCEDMODE_H
#define ADVANCEDMODE_H

typedef struct color
{
	u8 R;
	u8 G;
	u8 B;
} color_rgb;

void addToRgb(color_rgb* color, int selectedColorIndex, int value);

void printAdvancedControls();

void advancedMode(PrintConsole* topScreen);

#endif