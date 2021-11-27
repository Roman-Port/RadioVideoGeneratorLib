#pragma once

#include <radiovideo/views/view_video.h>
#include <radiovideo/config/config_view_video_rds_strip.h>
#include <radiovideo/graphics/text/font_renderer.h>
#include <raptorrds/client/client.h>

#define VIEW_PIID_MODE_CALL 0
#define VIEW_PIID_MODE_CALL_FREQ 1
#define VIEW_PIID_MODE_CALL_FREQ_DUAL 2

class radio_view_video_pi_id : public radio_view_video, public rds_event_sync_changed_handler, public rds_event_picode_update_handler {

public:
	radio_view_video_pi_id(context_channel* context, canvas_config_bundle* config);
	virtual void configure(float sampleRate, int fps) override;
	virtual void process(raptor_complex* input, int count) override;
	virtual void dispose() override;

	virtual void handle_rds_sync_changed(rds_event_sync_changed_t* evt) override;
	virtual void handle_rds_picode_update(rds_event_picode_update_t* evt) override;

private:
	font_renderer renderer;
	int mode;

	rds_client* rds;
	bool stale;
	bool demo;

	void draw_single(char* buffer);
	void draw_double(char* bufferA, char* bufferB);

};