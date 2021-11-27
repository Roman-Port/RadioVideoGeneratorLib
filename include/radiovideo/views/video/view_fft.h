#pragma once

#include <radiovideo/views/view_video.h>
#include <radiovideo/config/config_view_video_fft.h>
#include <raptordsp/defines.h>
#include <fftw3.h>

class radio_view_video_fft : public radio_view_video {

public:
	radio_view_video_fft(context_channel* context, canvas_config_bundle* bundle, int output_size);
	virtual void configure(float sampleRate, int fps) override;
	virtual void process(raptor_complex* input, int count) override;
	virtual void dispose() override;

protected:
	virtual void process_frame(float* power, int size) = 0;

private:
	void process_block();

	int output_size;
	int size;
	float attack;
	float decay;
	float range;
	float offset;

	int samplesPerBlock;
	int written;
	int bufferPos;
	float fft_fps;
	bool reset;

	fftwf_plan plan;

	float* window;
	float* output;
	float* output_scaled;
	float* output_smoothened;
	raptor_complex* buffer;
	raptor_complex* fft_input;
	raptor_complex* fft_output;

};