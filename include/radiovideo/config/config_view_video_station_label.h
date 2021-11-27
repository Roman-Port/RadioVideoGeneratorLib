#pragma once

#include <radiovideo/config/config_view_video.h>
#include <radiovideo/graphics/text/font.h>

struct config_view_video_station_label_t : public config_view_video_t {

	char* title;
	char* subtext_a;
	char* subtext_b;
	font_t font_title;
	font_t font_subtext;
	bool show_detection_icons;

};