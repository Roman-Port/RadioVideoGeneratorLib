#include <radiovideo/graphics/paint_solid.h>

radio_paint_solid::radio_paint_solid(pixel_t color) {
	this->color = color;
}

void radio_paint_solid::calculate(pixel_t* output, float value) {
	*output = color;
}