#pragma once

#include <radiovideo/views/view_video.h>
#include <radiovideo/config/config_view_video_station_label.h>
#include <radiovideo/graphics/text/font_renderer.h>
#include <raptorrds/client/client.h>

class station_label_widget {

public:
	virtual void refresh(pixel_t* outColor, char** outText) = 0;

	int width;

};

class radio_view_video_station_label : public radio_view_video {

public:
	radio_view_video_station_label(context_channel* context, canvas_config_bundle* bundle);
	virtual void configure(float sampleRate, int fps) override;
	virtual void process(raptor_complex* input, int count) override;
	virtual void dispose() override;

private:
	void push_widget(station_label_widget* widget);
	void refresh_widgets();

	pixel_t color_background;
	pixel_t color_font;
	pixel_t color_border;
	int padding;
	int border_width;
	bool show_detection_icons;
	char text_title[65];
	char text_subtext_a[65];
	char text_subtext_b[65];

	font_renderer renderer_title;
	font_renderer renderer_subtext;
	font_renderer renderer_icons;

	station_label_widget* widgets[8];
	int widgets_count;

};