#include <radiovideo/outputs/audio/output_audio_wav.h>
#include <raptordsp/misc/wav.h>
#include <cassert>
#include <malloc.h>
#include <volk/volk.h>

radio_output_audio_wav::radio_output_audio_wav(const char* filename) {
	file = fopen(filename, "wb");
	assert(file != nullptr);
}

void radio_output_audio_wav::configure(float sample_rate, int channels, int buffer_size) {
	//Set
	this->buffer_size = buffer_size;
	this->channels = channels;
	this->sample_rate = (int)sample_rate;

	//Reset
	this->written = 0;

	//Write WAV header
	update_wav_header();

	//Allocate the buffer
	output_buffer = (short*)malloc(sizeof(short) * buffer_size * channels);
}

void radio_output_audio_wav::push(float* samples, int count_per_channel) {
	//Validate
	assert(count_per_channel <= buffer_size);

	//Convert to short
	volk_32f_s32f_convert_16i(output_buffer, samples, 32768, count_per_channel * channels);

	//Write to file
	written += fwrite(output_buffer, 1, sizeof(short) * count_per_channel * channels, file);
}

void radio_output_audio_wav::close() {
	//Update the WAV header to reflect the new length of the file
	update_wav_header();	

	//Close file
	fclose(file);

	//Destroy
	free(output_buffer);
	delete(this);
}

void radio_output_audio_wav::update_wav_header() {
	//Rewind to beginning of the file
	fseek(file, 0, SEEK_SET);

	//Create WAV file header
	unsigned char buffer[WAV_HEADER_SIZE];
	create_wav_header(buffer, written <= 0 ? -1 : written, channels, sample_rate, 16);

	//Write WAV header
	fwrite(buffer, 1, WAV_HEADER_SIZE, file);

	//Go back to end of file
	fseek(file, 0, SEEK_END);
}