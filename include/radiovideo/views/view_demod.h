#pragma once

#include <radiovideo/config/config_view_audio.h>
#include <radiovideo/outputs/output.h>
#include <radiovideo/views/view.h>
#include <raptordsp/filter/fir/filter_complex.h>
#include <stdio.h>
#include <radiovideo/misc/bundle.h>

class radio_view_demod : public radio_view {

public:
	radio_view_demod(context_channel* context, canvas_config_bundle* bundle);
	virtual void configure(float sampleRate, int fps) override;
	virtual void process(raptor_complex* input, int count) override;
	virtual void layout(int* width, int* height) override;

protected:
	virtual void configure_demod(float sampleRate) = 0;
	virtual void process_demod(raptor_complex* input, int count) = 0;

private:
	float bandwidth;
	raptor_filter_ccf filter;

};