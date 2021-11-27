#include <radiovideo/views/audio/view_fm.h>

radio_view_audio_fm::radio_view_audio_fm(context_channel* context, canvas_config_bundle* bundle) : radio_view_audio(context, bundle),
	stereo_enabled(bundle->get_int("stereo")),
	rds_detected(false),
	stereo_detected(false),
	demodulator(get_context()->get_canvas()->get_buffer_size())
{
	//Set up demodulator
	demodulator.stereo_enabled = stereo_enabled;

	//Register modules
	context->put_module(CHANNEL_MODULE_STEREO_DETECTED, &stereo_detected);
	context->put_module(CHANNEL_MODULE_RDS_DETECTED, &rds_detected);
	context->put_module(CHANNEL_MODULE_RDS_CLIENT, demodulator.get_rds());
}

float radio_view_audio_fm::configure_audio(float sampleRate) {
	return demodulator.configure(sampleRate);
}

void radio_view_audio_fm::process_audio(raptor_complex* input, int count) {
	//Process demodulator
	count = demodulator.process(input, count, buffer_left, buffer_right);

	//Write
	write_audio(count);

	//Update state
	stereo_detected = demodulator.is_stereo_detected();
	rds_detected = demodulator.is_rds_detected();
}

void radio_view_audio_fm::dispose() {
	radio_view_audio::dispose();
}