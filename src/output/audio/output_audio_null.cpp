#include <radiovideo/outputs/audio/output_audio_null.h>

radio_output_audio_null::radio_output_audio_null() {

}

void radio_output_audio_null::configure(float sample_rate, int channels, int buffer_size) {

}

void radio_output_audio_null::push(float* samples, int count_per_channel) {

}

void radio_output_audio_null::close() {
	delete(this);
}