#include <radiovideo/views/video/view_spectrum_waterfall.h>
#include <radiovideo/graphics/util.h>
#include <cassert>
#include <cstring>

#define WATERFALL_ENABLED waterfall_height

radio_view_video_spectrum_waterfall::radio_view_video_spectrum_waterfall(context_channel* context, canvas_config_bundle* bundle) : radio_view_video_spectrum(context, bundle, 0, 0, bundle->get_int("width"), bundle->get_int("spectrum_height")) {
	//Calculate size
	spectrum_height = bundle->get_int("spectrum_height");
	spectrum_padding = bundle->get_int("spectrum_height") + bundle->get_int("spectrum_padding");
	waterfall_height = bundle->get_int("height") - spectrum_padding;
	waterfall_width = bundle->get_int("width");

	//Validate
	if (waterfall_height < 0)
		throw std::runtime_error("Waterfall Height is too small!");
	if (bundle->get_int("spectrum_height") <= 0)
		throw std::runtime_error("Spectrum Height is too small!");

	//Compute waterfall colors
	util_unpack_paint(bundle->get_bundle("waterfall_paint"), waterfall_colors, sizeof(waterfall_colors) / sizeof(pixel_t));

	//Prepare swap
	if (WATERFALL_ENABLED) {
		waterfall_swap = (pixel_t*)malloc(sizeof(pixel_t) * waterfall_width * waterfall_height);
		waterfall_swap_pos = 0;
	}
}

void radio_view_video_spectrum_waterfall::process_frame(float* power, int size) {
	//Process spectrum first
	radio_view_video_spectrum::process_frame(power, size);

	//Apply waterfall actions
	if (WATERFALL_ENABLED) {
		//Validate
		assert(size == waterfall_width);

		//Get the line to write to
		pixel_t* line = &waterfall_swap[waterfall_swap_pos * waterfall_width];

		//Transfer samples, looking them up in the color table in the process
		for (int i = 0; i < size; i++)
			line[i] = waterfall_colors[(int)(power[i] * 255)];

		//Now, copy the two sides of the buffer
		int rowsDrawn = waterfall_height - waterfall_swap_pos;
		memcpy(get_pixel(0, spectrum_padding), line, sizeof(pixel_t) * rowsDrawn * waterfall_width);
		if (waterfall_swap_pos != 0)
			memcpy(get_pixel(0, spectrum_padding + rowsDrawn), waterfall_swap, sizeof(pixel_t) * waterfall_swap_pos * waterfall_width);

		//Advance cursor
		waterfall_swap_pos--;
		if (waterfall_swap_pos < 0)
			waterfall_swap_pos = waterfall_height - 1;
	}
}

void radio_view_video_spectrum_waterfall::dispose() {
	if (WATERFALL_ENABLED)
		free(waterfall_swap);
	radio_view_video_spectrum::dispose();
}