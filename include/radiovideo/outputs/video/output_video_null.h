#pragma once

#include <radiovideo/outputs/output.h>

class radio_output_video_null : public radio_output_video {

public:
	radio_output_video_null();
	virtual void configure(int width, int height, int fps) override;
	virtual void push(pixel_t* frame) override;
	virtual void close() override;

};