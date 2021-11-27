#include <radiovideo/graphics/paint_gradient.h>
#include <malloc.h>
#include <cstring>
#include <cassert>

radio_paint_gradient::radio_paint_gradient(pixel_t* colors, int count) {
	this->count = count;

	//Allocate colors. Duplicate the last value for ease of use.
	assert(count >= 1);
	this->colors = (pixel_t*)malloc((count + 1) * sizeof(pixel_t));
	assert(this->colors != nullptr);
	
	//Copy
	if (colors != nullptr) {
		memcpy(this->colors, colors, count * sizeof(pixel_t));
		this->colors[count] = this->colors[count - 1];
	}
}

radio_paint_gradient::radio_paint_gradient(pixel_t color1, pixel_t color2) : radio_paint_gradient(nullptr, 2) {
	this->colors[0] = color1;
	this->colors[1] = color2;
	this->colors[2] = color2;
}

#define APPLY_WEIGHT(output, inputA, inputB, weightA, weightB) output = (inputA * weightA) + (inputB * weightB)

void radio_paint_gradient::calculate(pixel_t* output, float value) {
	//Make sure it's in range
	assert(value >= 0 && value <= 1);

	//Calculate what indicies this uses
	int index = (int)(value * (count - 1));

	//Calculate the weight each value has
	float weight = (value * (count - 1)) - index;
	float weightInverse = 1 - weight;

	//Apply weights
	APPLY_WEIGHT(output->r, colors[index].r, colors[index + 1].r, weightInverse, weight);
	APPLY_WEIGHT(output->g, colors[index].g, colors[index + 1].g, weightInverse, weight);
	APPLY_WEIGHT(output->b, colors[index].b, colors[index + 1].b, weightInverse, weight);
}