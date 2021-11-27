#pragma once

#include <radiovideo/views/view.h>
#include <radiovideo/config/config_view_data_markers.h>
#include <radiovideo/misc/bundle.h>

struct config_view_data_markers_point_t {

	char text[65];
	int frame;
	pixel_t color;

};

class radio_view_data_markers : public radio_view {

public:
	radio_view_data_markers(context_channel* context, canvas_config_bundle* bundle);
	virtual void configure(float sampleRate, int fps) override;
	virtual void process(raptor_complex* input, int count) override;
	virtual void layout(int* width, int* height) override;
	virtual void dispose() override;

	config_view_data_markers_point_t* get_current_item();

	config_view_data_markers_point_t* data;
	int data_count;

private:
	int current_frame;
	int current_index;

};