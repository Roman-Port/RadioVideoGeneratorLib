#pragma once

#include <radiovideo/views/video/view_spectrum.h>
#include <radiovideo/config/config_view_video_spectrum_waterfall.h>

class radio_view_video_spectrum_waterfall : public radio_view_video_spectrum {

public:
	radio_view_video_spectrum_waterfall(context_channel* context, canvas_config_bundle* bundle);
	virtual void dispose() override;

protected:
	virtual void process_frame(float* power, int size) override;

private:
	int spectrum_height;
	int spectrum_padding;
	int waterfall_height;
	int waterfall_width;

	pixel_t waterfall_colors[256];

	pixel_t* waterfall_swap;
	int waterfall_swap_pos;

};