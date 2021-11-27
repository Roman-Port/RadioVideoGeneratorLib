#pragma once

#include <radiovideo/views/view_video.h>
#include <radiovideo/config/config_view_video_rds_strip.h>
#include <radiovideo/graphics/text/font_renderer.h>
#include <raptorrds/client/client.h>

#define RDS_STRIP_WRAPPING_MODE_ON 2
#define RDS_STRIP_WRAPPING_MODE_AUTO 1
#define RDS_STRIP_WRAPPING_MODE_OFF 0

class radio_view_video_rds_strip : public radio_view_video, public rds_event_sync_changed_handler, public rds_event_picode_update_handler, public rds_event_ps_partial_update_handler, public rds_event_rt_partial_update_handler {

public:
	radio_view_video_rds_strip(context_channel* context, canvas_config_bundle* config);
	virtual void configure(float sampleRate, int fps) override;
	virtual void process(raptor_complex* input, int count) override;
	virtual void dispose() override;

	virtual void handle_rds_sync_changed(rds_event_sync_changed_t* evt) override;
	virtual void handle_rds_picode_update(rds_event_picode_update_t* evt) override;
	virtual void handle_rds_ps_partial_update(rds_event_ps_partial_update_t* evt) override;
	virtual void handle_rds_rt_partial_update(rds_event_rt_partial_update_t* evt) override;

private:
	font_renderer renderer_rds_ps;
	font_renderer renderer_rds_rt;

	pixel_t border_color;
	int border_width;
	int padding;

	bool reset_on_picode;
	bool reset_on_sync;
	int rt_wrapping;

	rds_client* rds;
	bool ps_stale;
	bool rt_stale;

	canvas_rectangle_t ps_rect;
	canvas_rectangle_t rt_rect;

	void draw_rt();

};