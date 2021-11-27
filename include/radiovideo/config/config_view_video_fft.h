#pragma once

#include <radiovideo/config/config_view_video.h>

struct config_view_video_fft_t : public config_view_video_t {

	int32_t fft_size;
	float attack;
	float decay;
	float range;
	float offset;

};