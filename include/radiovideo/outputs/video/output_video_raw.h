#pragma once

#include <radiovideo/outputs/output.h>
#include <stdio.h>

class radio_output_video_raw : public radio_output_video {

public:
	radio_output_video_raw(const char* filename);
	virtual void configure(int width, int height, int fps) override;
	virtual void push(pixel_t* frame) override;
	virtual void close() override;

private:
	FILE* file;
	int width;
	int height;

};