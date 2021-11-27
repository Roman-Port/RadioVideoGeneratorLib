#pragma once

#include <radiovideo/contexts/context_channel.h>
#include <radiovideo/views/view.h>
#include <radiovideo/config/config_channel.h>
#include <raptordsp/defines.h>
#include <raptordsp/filter/fir/filter_complex.h>
#include <radiovideo/graphics/frame_queue.h>

#define CHANNEL_MAX_OUTPUTS 8

class context_channel_impl : public context_channel {

public:
	context_channel_impl(context_canvas* canvas, config_channel_t* config);
	context_channel_impl(context_canvas* canvas, int64_t freq_offset, float bandwidth, float transition_width);

	/// <summary>
	/// Gets the canvas.
	/// </summary>
	/// <returns></returns>
	virtual context_canvas* get_canvas() override;

	/// <summary>
	/// Adds a module to the list. This should be done in the constructor for outputs. Modules are extra data pieces that may or may not exist.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="mod"></param>
	virtual void put_module(int id, void* mod) override;

	/// <summary>
	/// Gets a module from the list and returns if it was found or not.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="output"></param>
	/// <returns></returns>
	virtual bool get_module(int id, void** output) override;

	/// <summary>
	/// Queries the width and height and sets them to the MAXIMUM.
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	virtual void layout(int* width, int* height) override;

	/// <summary>
	/// Initially configures the output.
	/// </summary>
	/// <param name="sampleRate"></param>
	/// <param name="fps"></param>
	virtual void configure(float sampleRate, int fps) override;

	/// <summary>
	/// Waits for processing to finish. Returns if it was aborted or not.
	/// </summary>
	/// <param name="frame">The frame buffer to output to. Must be at least width*height.</param>
	/// <param name="stride">The stride (number of bytes) per line of the frame buffer.</param>
	virtual bool read(pixel_t* frame, size_t stride) override;

	/// <summary>
	/// Frees resources associated with this.
	/// </summary>
	virtual void dispose() override;

	/// <summary>
	/// Adds an output. MUST BE CALLED BEFORE CONFIGURE!
	/// </summary>
	/// <param name="output"></param>
	void put_output(radio_view* output);

private:
	/// <summary>
	/// Called on each frame NOW on the current thread.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="count"></param>
	void process_sync(raptor_complex* input, int count);

	/// <summary>
	/// Worker thread.
	/// </summary>
	static void* worker_thread(void* ctx);

	context_canvas* canvas;
	const char* label;
	int64_t freq_offset;
	float bandwidth;
	float transition_width;

	bool configured;
	bool disposing; //worker is finishing
	bool disposed; //worker is finished

	void* modules[CHANNEL_MAX_MODULES];
	radio_view* outputs[CHANNEL_MAX_OUTPUTS];
	int outputs_count;

	FILE* input;
	raptor_complex* buffer;
	short* buffer_input;
	int decimation;
	raptor_complex phase;
	raptor_complex phase_inc;

	frame_queue queue;
	std::thread* worker;
	std::mutex exiting_m;
	std::condition_variable exiting_cv;

	raptor_filter_ccf filter;

};