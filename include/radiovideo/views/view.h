#pragma once

#include <raptordsp/defines.h>
#include <radiovideo/contexts/context_channel.h>

class radio_view {

public:
	radio_view(context_channel* context);

	context_channel* get_context();

	/// <summary>
	/// Initially configures the output.
	/// </summary>
	/// <param name="sampleRate"></param>
	/// <param name="fps"></param>
	virtual void configure(float sampleRate, int fps) = 0;

	/// <summary>
	/// Called on each frame.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="count"></param>
	virtual void process(raptor_complex* input, int count) = 0;

	/// <summary>
	/// Queries the width and height and sets them to the MAXIMUM.
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	virtual void layout(int* width, int* height) = 0;

	virtual bool query_video_data(pixel_t** outCanvas, int* outX, int* outY, int* outWidth, int* outHeight) { return false; }

	/// <summary>
	/// Free all resources associated with this.
	/// </summary>
	virtual void dispose() = 0;

private:
	context_channel* context;

};