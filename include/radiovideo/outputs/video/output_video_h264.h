#pragma once

#include <radiovideo/outputs/output.h>
#include <stdio.h>
#include <stdint.h>
#include <x264.h>

class radio_output_video_h264 : public radio_output_video {

public:
	radio_output_video_h264(const char* filename);
	virtual void configure(int width, int height, int fps) override;
	virtual void push(pixel_t* frame) override;
	virtual void close() override;

private:
	FILE* file;
	int width;
	int height;
	int frame_count;

	x264_picture_t pic_in;
	x264_picture_t pic_out;
	x264_param_t params;
	x264_nal_t* nals;
	x264_t* encoder;
	int num_nals;
	int pts;

	bool prepare_h264(int fps);

};