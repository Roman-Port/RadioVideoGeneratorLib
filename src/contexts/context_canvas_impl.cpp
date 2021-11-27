#include <radiovideo/contexts/context_canvas_impl.h>
#include <radiovideo/contexts/context_channel_impl.h>
#include <raptordsp/misc/wav.h>
#include <radiovideo/outputs/video/output_video_h264.h>
#include <radiovideo/outputs/video/output_video_null.h>
#include <radiovideo/outputs/audio/output_audio_wav.h>
#include <radiovideo/outputs/audio/output_audio_null.h>
#include <volk/volk.h>
#include <cassert>

context_canvas_impl::context_canvas_impl(const char* inputPathname, const char* outputPathname, int fps, bool dryRun, pixel_t background_color) : background_color(background_color) {
	//Open file
	FILE* input = fopen(inputPathname, "rb");
	assert(input != nullptr);

	//Query the length of the file
	fseek(input, 0, SEEK_END);
	assert(fgetpos(input, &input_length_bytes) == 0);
	fseek(input, 0, SEEK_SET);

	//Read WAV header
	unsigned char wavHeaderData[WAV_HEADER_SIZE];
	fread(wavHeaderData, 1, WAV_HEADER_SIZE, input);
	wav_header_data wav;
	assert(read_wav_header(wavHeaderData, &wav));

	//Close file
	fclose(input);

	//Read filename
	assert(strlen(inputPathname) + 1 < sizeof(input_filename));
	strcpy(input_filename, inputPathname);

	//Set
	this->output = output;
	this->sample_rate = wav.sample_rate;
	this->fps = fps;
	this->dry_run = dryRun;

	//Clear
	this->configured = false;
	this->channels_count = 0;
	this->width = 0;
	this->height = 0;
	this->error_sent = false;

	//Get video
	this->output = create_output_video(outputPathname);

	//Set up
	this->buffer_size = (int)(sample_rate / fps);
	input_length_frames = (input_length_bytes - WAV_HEADER_SIZE) / wav.channels / sizeof(short) / buffer_size;

	//Allocate
	buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * buffer_size);
	buffer_input = (short*)malloc(sizeof(short) * 2 * buffer_size);
}

int context_canvas_impl::get_buffer_size() {
	return buffer_size;
}

int context_canvas_impl::get_width() {
	assert(configured);
	return width;
}

int context_canvas_impl::get_height() {
	assert(configured);
	return height;
}

const char* context_canvas_impl::get_filename() {
	return input_filename;
}

int context_canvas_impl::get_framerate() {
	return fps;
}

int64_t context_canvas_impl::get_length_frames() {
	return input_length_frames;
}

void context_canvas_impl::add_channel(context_channel* channel) {
	//Validate
	assert(!configured);
	assert(channels_count < CANVAS_MAX_CHANNELS);

	//Register
	channels[channels_count++] = channel;
}

void context_canvas_impl::configure() {
	//Determine width and height
	width = 0;
	height = 0;
	for (int i = 0; i < channels_count; i++)
		channels[i]->layout(&width, &height);

	//Allocate frame
	frame = (pixel_t*)malloc(sizeof(pixel_t) * width * height);
	assert(frame != nullptr);

	//Fill frame
	for (int i = 0; i < width * height; i++)
		frame[i] = background_color;

	//Configure all
	for (int i = 0; i < channels_count; i++)
		channels[i]->configure(sample_rate, fps);

	//Configure output
	output->configure(width, height, fps);

	//Set flag
	configured = true;
}

int context_canvas_impl::process(pixel_t** output_frame) {
	//Make sure we're configured
	assert(configured);

	//Read all frames
	bool ok = true;
	for (int i = 0; i < channels_count; i++)
		ok = ok && channels[i]->read(frame, get_width());

	//If any of the outputs finished, we've reached the end of the file! All outputs SHOULD finish at the same time, anyways
	if (!ok)
		return CANVAS_RESULT_DONE;

	//Export
	output->push(frame);
	(*output_frame) = frame;

	return CANVAS_RESULT_OK;
}

void context_canvas_impl::report_error(canvas_error error) {
	error_lock.lock();
	error_sent = true;
	error_data = error;
	error_lock.unlock();
}

int context_canvas_impl::check_error(char* result, int resultLen) {
	error_lock.lock();
	int len = 0;
	if (error_sent)
		len = error_data.stringify(result, resultLen);
	error_lock.unlock();
	return len;
}

radio_output_video* context_canvas_impl::create_output_video(const char* filename) {
	if (dry_run)
		return new radio_output_video_null();
	return new radio_output_video_h264(filename);
}

radio_output_audio* context_canvas_impl::create_output_audio(const char* filename) {
	if (dry_run)
		return new radio_output_audio_null();
	return new radio_output_audio_wav(filename);
}

void context_canvas_impl::dispose() {
	//Destroy each channel
	for (int i = 0; i < channels_count; i++)
		channels[i]->dispose();

	//Free components
	free(frame);
	free(buffer);
	free(buffer_input);

	//Close outputs
	output->close();
}