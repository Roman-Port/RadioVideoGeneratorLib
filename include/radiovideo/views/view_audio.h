#pragma once

#include <radiovideo/views/view_demod.h>

class radio_view_audio : public radio_view_demod {

public:
	radio_view_audio(context_channel* context, canvas_config_bundle* bundle);
	virtual void dispose() override;

protected:
	virtual void configure_demod(float sampleRate) override;
	virtual void process_demod(raptor_complex* input, int count) override;

	virtual float configure_audio(float sampleRate) = 0;
	virtual void process_audio(raptor_complex* input, int count) = 0;
	void write_audio(int count);

	float* buffer_left;
	float* buffer_right;

private:
	radio_output_audio* output;
	float* buffer_output;

	float audio_scale;

};