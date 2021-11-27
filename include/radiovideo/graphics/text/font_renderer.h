#pragma once

#include <radiovideo/pixel.h>
#include <radiovideo/graphics/text/font.h>
#include <radiovideo/graphics/rectangle.h>

#define FONT_VCENTER_TOP -1
#define FONT_VCENTER_CENTER 0
#define FONT_VCENTER_BOTTOM 1

#define FONT_HCENTER_LEFT -1
#define FONT_HCENTER_CENTER 0
#define FONT_HCENTER_RIGHT 1

class font_renderer {

public:
	font_renderer(pixel_t* canvas, size_t stride, font_t font, pixel_t colorForeground, pixel_t colorBackground, bool monospace, int character_spacing);
	void update_colors(pixel_t colorForeground, pixel_t colorBackground);
	void measure_character(char value, int* width, int* height);
	void measure_line(char* text, int length, int* width, int* height);
	int measure_line_width(char* text, int length);
	int measure_line_width_monospaced(int length);
	int draw_character(char value, int x, int y, int remainingWidth, bool* result);
	int draw_line(char* text, int length, int x, int y, int width);
	int draw_line_centered(char* text, int length, int x, int y, int width, int height, int horizCentering, int verticalCentering);
	int draw_line_centered(char* text, int length, canvas_rectangle_t rect, int horizCentering, int verticalCentering) {
		return draw_line_centered(text, length, rect.x, rect.y, rect.width, rect.height, horizCentering, verticalCentering);
	}

private:
	font_t font;
	unsigned char char_width;
	unsigned char char_height;

	pixel_t color_table[256];

	pixel_t* canvas;
	size_t stride;

	bool monospace;
	int character_spacing;

};