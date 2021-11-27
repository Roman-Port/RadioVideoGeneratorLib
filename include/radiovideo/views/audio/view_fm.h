#pragma once

#include <radiovideo/views/view_audio.h>
#include <radiovideo/config/config_view_audio_fm.h>
#include <raptorrds/demod_bcast_fm_stereo_rds.h>

class radio_view_audio_fm : public radio_view_audio {

public:
	radio_view_audio_fm(context_channel* context, canvas_config_bundle* bundle);
	virtual void dispose() override;

protected:
	virtual float configure_audio(float sampleRate) override;
	virtual void process_audio(raptor_complex* input, int count) override;

private:
	demod_bcast_fm_stereo_rds demodulator;
	bool rds_detected;
	bool stereo_detected;
	bool stereo_enabled;
	
};