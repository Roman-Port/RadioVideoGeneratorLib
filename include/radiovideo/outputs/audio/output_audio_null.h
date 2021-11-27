#pragma once

#include <radiovideo/outputs/output.h>

class radio_output_audio_null : public radio_output_audio {

public:
	radio_output_audio_null();
	virtual void configure(float sample_rate, int channels, int buffer_size) override;
	virtual void push(float* samples, int count_per_channel) override;
	virtual void close() override;

};