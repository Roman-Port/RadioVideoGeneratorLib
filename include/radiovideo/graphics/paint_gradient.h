#pragma once

#include <radiovideo/paint.h>

class radio_paint_gradient : public radio_paint {

public:
	radio_paint_gradient(pixel_t* colors, int count);
	radio_paint_gradient(pixel_t colors1, pixel_t color2);
	virtual void calculate(pixel_t* output, float value) override;

private:
	pixel_t* colors;
	int count;

};