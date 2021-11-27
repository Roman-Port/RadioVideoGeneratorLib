#include <radiovideo/views/view_audio.h>
#include <cassert>
#include <malloc.h>
#include <volk/volk.h>
#include <raptordsp/filter/builder/builder_lowpass.h>

radio_view_audio::radio_view_audio(context_channel* context, canvas_config_bundle* bundle) : radio_view_demod(context, bundle),
	audio_scale(bundle->get_float("audio_scale"))
{
	//Open output
	char buffer[512];
	bundle->get_string("output_filename", sizeof(buffer), buffer);
	this->output = context->get_canvas()->create_output_audio(buffer);
}

void radio_view_audio::configure_demod(float sampleRate) {
	//Allocate buffers
	buffer_left = (float*)malloc(sizeof(float) * get_context()->get_canvas()->get_buffer_size());
	buffer_right = (float*)malloc(sizeof(float) * get_context()->get_canvas()->get_buffer_size());
	buffer_output = (float*)malloc(sizeof(float) * 2 * get_context()->get_canvas()->get_buffer_size());

	//Configure audio component
	sampleRate = configure_audio(sampleRate);

	//Configure
	output->configure(sampleRate, 2, get_context()->get_canvas()->get_buffer_size());
}

void radio_view_audio::process_demod(raptor_complex* input, int count) {
	process_audio(input, count);
}

void radio_view_audio::write_audio(int count) {
	//Interleave
	volk_32f_x2_interleave_32fc((lv_32fc_t*)buffer_output, buffer_left, buffer_right, count);

	//Multiply
	volk_32f_s32f_multiply_32f(buffer_output, buffer_output, audio_scale, count * 2);

	//Output
	output->push(buffer_output, count);
}

void radio_view_audio::dispose() {
	//Destroy the output
	output->close();

	//Destroy buffers
	free(buffer_left);
	free(buffer_right);
	free(buffer_output);

	delete(this);
}