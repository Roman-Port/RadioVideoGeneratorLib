#include <radiovideo/outputs/video/output_video_null.h>

radio_output_video_null::radio_output_video_null() {
}

void radio_output_video_null::configure(int width, int height, int fps) {
}

void radio_output_video_null::push(pixel_t* frame) {
}

void radio_output_video_null::close() {
	delete(this);
}