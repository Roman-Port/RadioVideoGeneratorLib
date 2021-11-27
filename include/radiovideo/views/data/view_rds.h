#pragma once

#include <radiovideo/views/view_demod.h>
#include <raptorrds/pipeline.h>

class radio_view_data_rds : public radio_view_demod {

public:
	radio_view_data_rds(context_channel* context, canvas_config_bundle* bundle);
	virtual void dispose() override;

protected:
	virtual void configure_demod(float sampleRate) override;
	virtual void process_demod(raptor_complex* input, int count) override;

private:
	rds_pipeline rds;

	float* mpx_buffer;

	float gain;
	raptor_complex last_sample;

};