#include <radiovideo/views/video/view_spectrum.h>
#include <radiovideo/graphics/util.h>
#include <cassert>

radio_view_video_spectrum::radio_view_video_spectrum(context_channel* context, canvas_config_bundle* bundle) : radio_view_video_spectrum(context, bundle, 0, 0, bundle->get_int("width"), bundle->get_int("height")) {
}

radio_view_video_spectrum::radio_view_video_spectrum(context_channel* context, canvas_config_bundle* bundle, int override_margin_left, int override_margin_top, int override_content_width, int override_content_height) : radio_view_video_fft(context, bundle, bundle->get_int("width")) {
	//Set
	this->content_width = override_content_width;
	this->content_height = override_content_height;
	this->margin_left = override_margin_left;
	this->margin_top = override_margin_top;
	this->line = pixel_t(255);
	this->use_vertical_gradient = bundle->get_int("use_vertical_gradient");
	this->draw_outline = bundle->get_int("draw_outline");

	//"Inflate" colors to fill height
	compute_colors(&this->background, bundle->get_bundle("paint_background"));
	compute_colors(&this->foreground, bundle->get_bundle("paint_foreground"));
}

void radio_view_video_spectrum::configure(float sampleRate, int fps) {
	radio_view_video_fft::configure(sampleRate, fps);
}

void radio_view_video_spectrum::process_frame(float* frame, int length) {
	//Make sure the length matches the content width
	assert(length == content_width);

	//Fill left to right
	int value = (int)(frame[0] * (content_height - 1));
	int old_value;
	pixel_t* dst;
	for (int i = 0; i < content_width; i++) {
		//Get value
		old_value = value;
		value = (int)(frame[i] * (content_height - 1));

		//Fill background
		fill_column(i, value, background, foreground);

		//Draw outline
		if (draw_outline) {
			//Draw line at old
			dst = get_pixel(i - margin_left, margin_top + old_value);
			*dst = line;

			//Draw line to this
			while (old_value < value) {
				old_value++;
				dst = get_pixel(i - margin_left, margin_top + old_value);
				*dst = line;
			}
			while (old_value > value) {
				old_value--;
				dst = get_pixel(i - margin_left, margin_top + old_value);
				*dst = line;
			}
		}
	}
}

void radio_view_video_spectrum::compute_colors(pixel_t** result, canvas_config_bundle* paint) {
	//Create the buffer
	pixel_t* buffer = (pixel_t*)malloc(sizeof(pixel_t) * content_height);
	assert(buffer != nullptr);

	//Compute
	util_unpack_paint(paint, buffer, content_height);

	//Set
	(*result) = buffer;
}

void radio_view_video_spectrum::fill_column(int x, int value, pixel_t* srcTop, pixel_t* srcBottom) {
	pixel_t* dst = get_pixel(x - margin_left, margin_top);
	int w = get_width();
	int i = 0;
	while (i < value && i < content_height) {
		*dst = srcTop[i];
		dst += w;
		i++;
	}
	while (i < content_height) {
		if (use_vertical_gradient)
			*dst = srcBottom[value];
		else
			*dst = srcBottom[i];
		dst += w;
		i++;
	}
}

void radio_view_video_spectrum::dispose() {
	free(background);
	free(foreground);
	radio_view_video_fft::dispose();
}