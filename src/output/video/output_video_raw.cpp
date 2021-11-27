#include <radiovideo/outputs/video/output_video_raw.h>
#include <cassert>

radio_output_video_raw::radio_output_video_raw(const char* filename) {
	file = fopen(filename, "wb");
	assert(file != nullptr);
	this->width = -1;
	this->height = -1;
}

void radio_output_video_raw::configure(int width, int height, int fps) {
	this->width = width;
	this->height = height;
}

void radio_output_video_raw::push(pixel_t* frame) {
	assert(width > 0 && height > 0);
	fwrite(frame, sizeof(pixel_t), width * height, file);
}

void radio_output_video_raw::close() {
	fclose(file);
	delete(this);
}