#include <radiovideo/views/video/view_fft.h>
#include <algorithm>
#include <cassert>
#include <malloc.h>
#include <raptordsp/filter/window.h>
#include <volk/volk.h>

radio_view_video_fft::radio_view_video_fft(context_channel* context, canvas_config_bundle* bundle, int output_size) : radio_view_video(context, bundle) {
	//Set
	this->output_size = output_size;
    this->size = bundle->get_int("fft_size");
    this->attack = bundle->get_float("attack");
    this->decay = bundle->get_float("decay");
	this->range = bundle->get_float("range");
	this->offset = bundle->get_float("offset");

    //Clear
    this->written = 0;
    this->samplesPerBlock = 0;
    this->bufferPos = 0;
    this->reset = true;

    //Create windowing function
    window = raptor_window_build(win_type::WIN_HANN, size, 1, true);

    //Create buffers
    size_t alignment = volk_get_alignment();
    output = (float*)volk_malloc(size * sizeof(float), alignment);
    output_scaled = (float*)volk_malloc(output_size * sizeof(float), alignment);
    output_smoothened = (float*)volk_malloc(output_size * sizeof(float), alignment);
    buffer = (raptor_complex*)volk_malloc(size * sizeof(raptor_complex), alignment);
    fft_input = (raptor_complex*)fftwf_malloc(size * sizeof(raptor_complex));
    fft_output = (raptor_complex*)fftwf_malloc(size * sizeof(raptor_complex));

    //Create plan
    plan = fftwf_plan_dft_1d(size, (fftwf_complex*)fft_input, (fftwf_complex*)fft_output, FFTW_FORWARD, FFTW_ESTIMATE);
}

void radio_view_video_fft::configure(float sampleRate, int fps) {
    //Set and reset
    fft_fps = sampleRate / fps / size;
    written = 0;
    reset = true;

    //Clear the output buffer
    for (int i = 0; i < output_size; i++)
        output_smoothened[i] = 0;

    //Send empty frame
    process_frame(output_smoothened, output_size);
}

void radio_view_video_fft::process(raptor_complex* input, int count) {
    //Add
    bool frameExported = false;
    while (count > 0) {
        //Calculate how much can be written to the buffer
        int writable = std::min(count, size - bufferPos);
        memcpy(&fft_input[bufferPos], input, writable * sizeof(raptor_complex));

        //Update state
        bufferPos += writable;
        count -= writable;
        input += writable;

        //Process blocks when we get them
        if (size == bufferPos) {
            process_block();
            frameExported = true;
        }            
    }

    //Process frame if we have one
    if (frameExported)
        process_frame(output_smoothened, output_size);
}

void offset_spectrum(raptor_complex* buffer, int count) {
    count /= 2;
    raptor_complex* left = buffer;
    raptor_complex* right = buffer + count;
    raptor_complex temp;
    for (int i = 0; i < count; i++)
    {
        temp = *left;
        *left++ = *right;
        *right++ = temp;
    }
}

void apply_smoothening(float* buffer, float* incoming, int count, float attack, float decay)
{
    float ratio;
    for (int i = 0; i < count; i++)
    {
        ratio = buffer[i] < incoming[i] ? attack : decay;
        buffer[i] = buffer[i] * (1 - ratio) + incoming[i] * ratio;
    }
}

void resize_power(float* input, float* output, int inputLen, int outputLen)
{
    int inputIndex = 0;
    int outputIndex = 0;
    float max = 0;
    bool resetMax = true;
    while (inputIndex < inputLen)
    {
        //Read value
        if (resetMax)
            max = input[inputIndex++];
        else
            max = std::max(input[inputIndex++], max);

        //Write to output
        int targetOutput = (inputIndex * outputLen) / inputLen;
        resetMax = (outputIndex < targetOutput);
        while (outputIndex < targetOutput)
            output[outputIndex++] = max;
    }
}

void radio_view_video_fft::process_block() {
    //Unwrap the circular buffer to the FFT input
    //memcpy(fft_input, &buffer[bufferPos], (size - bufferPos) * sizeof(raptor_complex));
    //memcpy(&fft_input[size - bufferPos], buffer, bufferPos * sizeof(raptor_complex));

    //Apply window function
    for (int i = 0; i < size; i++) {
        ((float*)&fft_input[i])[0] *= window[i];
        ((float*)&fft_input[i])[1] *= window[i];
    }

    //Compute fft
    fftwf_execute(plan);

    //Offset the spectrum to center
    offset_spectrum(fft_output, size);

    //Calculate magnitude squared
    volk_32fc_magnitude_squared_32f(output, fft_output, size);

    //Scale to dB
    //https://github.com/gnuradio/gnuradio/blob/1a0be2e6b54496a8136a64d86e372ab219c6559b/gr-utils/plot_tools/plot_fft_base.py#L88
    for (int i = 0; i < size; i++)
        output[i] = 20 * log10(abs((output[i] + 1e-15f) / size));

    //Scale to fit window and scale to [0, 1]
    for (int i = 0; i < size; i++)
        output[i] = (-output[i] - offset) / range;

    //Constrain
    for (int i = 0; i < size; i++)
        output[i] = std::max(0.0f, std::min(1.0f, output[i]));

    //Resize power to desired size
    resize_power(output, output_scaled, size, output_size);

    //Smoothen (or copy if this is the first time)
    if (reset)
        memcpy(output_smoothened, output_scaled, output_size * sizeof(float));
    else
        apply_smoothening(output_smoothened, output_scaled, output_size, attack / fft_fps, decay / fft_fps);
    reset = false;

    //Reset state
    bufferPos = 0;
}

void radio_view_video_fft::dispose() {
    fftwf_destroy_plan(plan);
    free(window);
    volk_free(output);
    volk_free(output_scaled);
    volk_free(output_smoothened);
    volk_free(buffer);
    fftwf_free(fft_input);
    fftwf_free(fft_output);
    radio_view_video::dispose();
}