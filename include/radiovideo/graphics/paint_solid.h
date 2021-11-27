#pragma once

#include <radiovideo/paint.h>

class radio_paint_solid : public radio_paint {

public:
	radio_paint_solid(pixel_t color);
	virtual void calculate(pixel_t* output, float value) override;

private:
	pixel_t color;

};