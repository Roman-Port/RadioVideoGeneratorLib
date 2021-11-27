#include <radiovideo/graphics/text/font_renderer.h>
#include <cassert>
#include <algorithm>
#include <stdexcept>

//#define DEBUG_FONT_BOUNDRIES

#define INTERPOLATE(output, inputA, inputB, ratio) output = ((ratio) * inputB) + ((1 - (ratio)) * inputA)
#define GET_CHARACTER(index) (&font[FONT_STRUCT_HEADER_LEN + ((FONT_CHAR_HEADER_LEN + (char_width * char_height)) * (unsigned char)(index))])
#define GET_PIXEL(x, y) (&canvas[(x) + ((y) * stride)])

font_renderer::font_renderer(pixel_t* canvas, size_t stride, font_t font, pixel_t colorForeground, pixel_t colorBackground, bool monospace, int character_spacing) {
	//Set
	this->canvas = canvas;
	this->stride = stride;
	this->font = font;
	this->monospace = monospace;
	this->character_spacing = character_spacing;

	//Read some data from the font
	char_width = font[0];
	char_height = font[1];

	//Do debugging options
#ifdef DEBUG_FONT_BOUNDRIES
	colorForeground = pixel_t(255, 255, 255);
	colorBackground = pixel_t(255, 0, 0);
#endif
	
	//Create the color table by interpolating
	update_colors(colorForeground, colorBackground);
}

void font_renderer::update_colors(pixel_t colorForeground, pixel_t colorBackground) {
	float ratio;
	for (int i = 0; i < 256; i++) {
		ratio = i / 255.0f;
		INTERPOLATE(color_table[i].r, colorForeground.r, colorBackground.r, ratio);
		INTERPOLATE(color_table[i].g, colorForeground.g, colorBackground.g, ratio);
		INTERPOLATE(color_table[i].b, colorForeground.b, colorBackground.b, ratio);
	}
}

void debug_fill(pixel_t* canvas, size_t stride, pixel_t color, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
			canvas[x] = color;
		canvas += stride;
	}
}

void font_renderer::measure_character(char value, int* width, int* height) {
	if (monospace) {
		*width = char_width;
		*height = char_height;
	}
	else {
		*width = GET_CHARACTER(value)[0];
		*height = char_height;
	}
}

void font_renderer::measure_line(char* text, int length, int* width, int* height) {
	*width = 0;
	*height = 0;
	int widthTmp;
	int heightTmp;
	for (int i = 0; i < length; i++) {
		measure_character(text[i], &widthTmp, &heightTmp);
		*height = std::max(*height, heightTmp);
		*width += widthTmp;
	}
	*width += character_spacing * (length - 1);
}

int font_renderer::measure_line_width(char* text, int length) {
	int width;
	int height;
	measure_line(text, length, &width, &height);
	return width;
}

int font_renderer::measure_line_width_monospaced(int length) {
	if (!monospace)
		throw std::runtime_error("This is only supported in monospaced mode.");
	return (length * char_width) + (std::max(0, length - 1) * character_spacing);
}

int font_renderer::draw_character(char value, int dstX, int dstY, int remainingWidth, bool* result) {
	//Get the character data
	unsigned char* data = &GET_CHARACTER(value)[FONT_CHAR_HEADER_LEN];

	//Query character size
	int width;
	int height;
	measure_character(value, &width, &height);

	//If we don't have enough space, abort
	(*result) = width <= remainingWidth;
	if (!*result)
		return 0;

	//Draw line by line
	pixel_t* dst;
	for (int y = 0; y < height; y++) {
		//Get the current pixel
		dst = GET_PIXEL(dstX, dstY + y);

		//Copy across
		for (int x = 0; x < width; x++)
			dst[x] = color_table[data[x]];

		//Go to next line of character data
		data += char_width;
	}

	return width + character_spacing;
}

int font_renderer::draw_line(char* text, int length, int x, int y, int width) {
	int use = 0;
	int drawn = 0;
	bool status = true;
	for (int i = 0; i < length && status; i++) {
		use += draw_character(text[i], x + use, y, width - use, &status);
		drawn += status;
	}
	return drawn;
}

int font_renderer::draw_line_centered(char* text, int length, int x, int y, int width, int height, int horizCentering, int verticalCentering) {
	//Fill entire with background
	debug_fill(GET_PIXEL(x, y), stride, color_table[255], width, height);

	//Do debug stuff
#ifdef DEBUG_FONT_BOUNDRIES
	debug_fill(GET_PIXEL(x, y), stride, pixel_t(0, 255, 0), width, height);
#endif

	//Measure and figure out how many characters we can draw
	int boxWidth;
	int boxHeight;
	do {
		measure_line(text, length, &boxWidth, &boxHeight);
	} while (boxWidth > width && --length > 0);

	//Make sure we have height vertically
	if (boxHeight > height)
		length = 0;

	//Abort on failure
	if (length == 0)
		return 0;

	//Center horizontally
	switch (horizCentering) {
	case FONT_HCENTER_CENTER: x += (width - boxWidth) / 2; break;
	case FONT_HCENTER_RIGHT: x += (width - boxWidth); break;
	}	
	
	//Center vertically
	switch (verticalCentering) {
	case FONT_VCENTER_CENTER: y += (height - boxHeight) / 2; break;
	case FONT_VCENTER_BOTTOM: y += (height - boxHeight); break;
	}

	//Render
	return draw_line(text, length, x, y, boxWidth);
}