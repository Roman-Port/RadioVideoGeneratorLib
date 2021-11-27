#pragma once

#include <radiovideo/views/video/view_fft.h>
#include <radiovideo/config/config_view_video_spectrum.h>

class radio_view_video_spectrum : public radio_view_video_fft {

public:
	radio_view_video_spectrum(context_channel* context, canvas_config_bundle* config);
	radio_view_video_spectrum(context_channel* context, canvas_config_bundle* config, int32_t override_margin_left, int32_t override_margin_top, int32_t override_content_width, int32_t override_content_height);
	virtual void dispose() override;

protected:
	virtual void configure(float sampleRate, int fps) override;
	virtual void process_frame(float* power, int size) override;

private:
	void compute_colors(pixel_t** buffer, canvas_config_bundle* paint);
	void fill_column(int x, int value, pixel_t* srcTop, pixel_t* srcBottom);

	bool use_vertical_gradient;
	bool draw_outline;

	int content_width;
	int content_height;
	int margin_left;
	int margin_top;
	pixel_t line;

	pixel_t* background;
	pixel_t* foreground;

};