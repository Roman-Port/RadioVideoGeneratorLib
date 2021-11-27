#pragma once

#include <radiovideo/pixel.h>

class radio_output_audio {

public:
	virtual void configure(float sample_rate, int channels, int buffer_size) = 0;
	virtual void push(float* samples, int count_per_channel) = 0;
	virtual void close() = 0;

};

class radio_output_video {

public:
	virtual void configure(int width, int height, int fps) = 0;
	virtual void push(pixel_t* frame) = 0;
	virtual void close() = 0;

};