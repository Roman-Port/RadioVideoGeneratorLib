#include <radiovideo/graphics/util.h>
#include <cassert>

#define APPLY_WEIGHT(output, inputA, inputB, weightA, weightB) output = (inputA * weightA) + (inputB * weightB)

void util_interpolate_colors(const pixel_t* colors, int count, float value, pixel_t* output) {
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

void util_unpack_colors(const pixel_t* buffer, int bufferCount, pixel_t* output, int outputCount) {
	for (int i = 0; i < outputCount; i++)
		util_interpolate_colors(buffer, bufferCount, (float)i / outputCount, &output[i]);
}

void util_unpack_paint(canvas_config_bundle* paint, pixel_t* output, int outputCount) {
	//Get the size of the paint
	int count = paint->get_count();

	//If the count is zero, just fill all with black
	if (count == 0) {
		for (int i = 0; i < outputCount; i++)
			output[i] = pixel_t(0, 0, 0);
		return;
	}

	//Allocate a temporary buffer
	pixel_t* buffer = (pixel_t*)malloc(sizeof(pixel_t) * count);
	assert(buffer != nullptr);

	//Enumerate through and unpack
	canvas_config_bundle_element_t* cursor = nullptr;
	int index = 0;
	while (paint->enumerate(&cursor))
		buffer[index++] = paint->get_color(cursor->key);

	//Make sure it's been read correctly
	assert(index == count);

	//Unpack normally
	util_unpack_colors(buffer, count, output, outputCount);

	//Free temporary buffer
	free(buffer);
}