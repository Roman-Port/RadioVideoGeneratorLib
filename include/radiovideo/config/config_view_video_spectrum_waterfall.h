#pragma once

#include <radiovideo/config/config_view_video_spectrum.h>

struct config_view_video_spectrum_waterfall_t : public config_view_video_spectrum_t {

	radio_paint* paint_waterfall;
	int spectrum_padding;
	int spectrum_height;

};