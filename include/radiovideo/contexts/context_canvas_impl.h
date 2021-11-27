#pragma once

#include <radiovideo/contexts/context_canvas.h>
#include <radiovideo/contexts/context_channel.h>
#include <raptordsp/defines.h>
#include <radiovideo/config/config_channel.h>
#include <radiovideo/pixel.h>
#include <radiovideo/outputs/output.h>
#include <mutex>

#define CANVAS_MAX_CHANNELS 32

#define CANVAS_RESULT_OK 1
#define CANVAS_RESULT_DONE 0

/// <summary>
/// Canvas is an entire IQ file and all components and channels under it.
/// </summary>
class context_canvas_impl : public context_canvas {

public:
	context_canvas_impl(const char* inputPathname, const char* outputPathname, int fps, bool dryRun, pixel_t background_color);

	/// <summary>
	/// Returns the maximum buffer size per frame.
	/// </summary>
	/// <returns></returns>
	virtual int get_buffer_size() override;

	/// <summary>
	/// Gets the width of the canvas AFTER configure is called.
	/// </summary>
	/// <returns></returns>
	virtual int get_width() override;

	/// <summary>
	/// Gets the height of the canvas AFTER configure is called.
	/// </summary>
	/// <returns></returns>
	virtual int get_height() override;

	/// <summary>
	/// Gets the video frame rate.
	/// </summary>
	/// <returns></returns>
	virtual int get_framerate() override;

	/// <summary>
	/// Gets the input filename.
	/// </summary>
	/// <returns></returns>
	virtual const char* get_filename() override;

	/// <summary>
	/// Sends an error to the user.
	/// </summary>
	/// <param name="text"></param>
	virtual void report_error(canvas_error error) override;

	/// <summary>
	/// Get the length of the input file, in frames.
	/// </summary>
	/// <returns></returns>
	int64_t get_length_frames();

	/// <summary>
	/// Initially configures the canvas.
	/// </summary>
	void configure();

	/// <summary>
	/// Processes a frame and returns a pointer to the display.
	/// </summary>
	int process(pixel_t** output_frame);

	/// <summary>
	/// Checks if any errors were reported.
	/// </summary>
	/// <param name="result"></param>
	/// <param name="resultLen"></param>
	/// <returns>Length of error.</returns>
	int check_error(char* result, int resultLen);

	/// <summary>
	/// Registers the new channel. MUST be called before configure.
	/// </summary>
	/// <param name="channel"></param>
	void add_channel(context_channel* chammel);

	/// <summary>
	/// Creates a video output.
	/// </summary>
	/// <returns></returns>
	virtual radio_output_video* create_output_video(const char* filename) override;

	/// <summary>
	/// Creates an audio output.
	/// </summary>
	/// <returns></returns>
	virtual radio_output_audio* create_output_audio(const char* filename) override;

	/// <summary>
	/// Frees resources associated with this.
	/// </summary>
	virtual void dispose() override;

private:
	char input_filename[512];
	float sample_rate;
	int64_t input_length_bytes;
	int64_t input_length_frames;
	pixel_t background_color;

	bool dry_run;
	int fps;
	int buffer_size;
	radio_output_video* output;
	
	bool configured;
	pixel_t* frame;
	int width;
	int height;

	raptor_complex* buffer;
	short* buffer_input;

	context_channel* channels[CANVAS_MAX_CHANNELS];
	int channels_count;

	std::mutex error_lock;
	bool error_sent;
	canvas_error error_data;

};