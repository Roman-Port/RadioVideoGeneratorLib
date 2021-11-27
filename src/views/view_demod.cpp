#include <radiovideo/views/view_demod.h>
#include <cassert>
#include <malloc.h>
#include <volk/volk.h>
#include <raptordsp/filter/builder/builder_lowpass.h>

radio_view_demod::radio_view_demod(context_channel* context, canvas_config_bundle* bundle) : radio_view(context),
	bandwidth(bundle->get_float("bandwidth"))
{
}

void radio_view_demod::layout(int* width, int* height) {
	//Not a video component so...do nothing!
}

void radio_view_demod::configure(float sampleRate, int fps) {
	//Design FIR filter for incoming samples and configure
	raptor_filter_builder_lowpass builder(sampleRate, bandwidth * 0.5f);
	builder.automatic_tap_count(bandwidth * 0.1f);
	int decimation = builder.calculate_decimation(&sampleRate);
	filter.configure(builder.build_taps_real(), decimation);

	//Configure audio component
	configure_demod(sampleRate);
}

void radio_view_demod::process(raptor_complex* input, int count) {
	//Filter and decimate
	count = filter.process(input, input, count);

	//Allow client to process
	process_demod(input, count);
}