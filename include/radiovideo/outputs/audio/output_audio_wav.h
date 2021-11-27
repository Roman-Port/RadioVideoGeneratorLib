#pragma once

#include <radiovideo/outputs/output.h>
#include <stdio.h>

class radio_output_audio_wav : public radio_output_audio {

public:
	radio_output_audio_wav(const char* filename);
	virtual void configure(float sample_rate, int channels, int buffer_size) override;
	virtual void push(float* samples, int count_per_channel) override;
	virtual void close() override;

private:
	void update_wav_header();

	FILE* file;
	short* output_buffer;
	int buffer_size;
	int sample_rate;
	int channels;
	int written;

};