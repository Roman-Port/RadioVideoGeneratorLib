#pragma once

#include <radiovideo/config/config_view_video_fft.h>
#include <radiovideo/paint.h>

struct config_view_video_spectrum_t : public config_view_video_fft_t {

	radio_paint* paint_foreground;
	radio_paint* paint_background;

};