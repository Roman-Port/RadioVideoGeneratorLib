#pragma once

#include <radiovideo/contexts/context_canvas.h>
#include <radiovideo/pixel.h>
#include <raptordsp/defines.h>

#define CHANNEL_MODULE_STEREO_DETECTED 0 /* bool; radio_view_audio_fm */
#define CHANNEL_MODULE_RDS_DETECTED 1 /* bool; radio_view_audio_fm */
#define CHANNEL_MODULE_MARKERS 2 /* radio_view_data_markers; radio_view_data_markers */
#define CHANNEL_MODULE_RDS_CLIENT 3 /* rds_client; radio_view_audio_fm */
#define CHANNEL_MAX_MODULES 4

/// <summary>
/// A channel is a mutated IQ source
/// </summary>
class context_channel {

public:
	/// <summary>
	/// Gets the canvas.
	/// </summary>
	/// <returns></returns>
	virtual context_canvas* get_canvas() = 0;

	/// <summary>
	/// Adds a module to the list. This should be done in the constructor for outputs. Modules are extra data pieces that may or may not exist.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="mod"></param>
	virtual void put_module(int id, void* mod) = 0;

	/// <summary>
	/// Gets a module from the list and returns if it was found or not.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="output"></param>
	/// <returns></returns>
	virtual bool get_module(int id, void** output) = 0;

	template<class T>
	bool get_module_generic(int id, T** output) {
		return get_module(id, (void**)output);
	}

	/// <summary>
	/// Queries the width and height and sets them to the MAXIMUM.
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	virtual void layout(int* width, int* height) = 0;

	/// <summary>
	/// Initially configures the output.
	/// </summary>
	/// <param name="sampleRate"></param>
	/// <param name="fps"></param>
	virtual void configure(float sampleRate, int fps) = 0;

	/// <summary>
	/// Waits for processing to finish. Returns if it was aborted or not.
	/// </summary>
	/// <param name="frame">The frame buffer to output to. Must be at least width*height.</param>
	/// <param name="stride">The stride (number of bytes) per line of the frame buffer.</param>
	virtual bool read(pixel_t* frame, size_t stride) = 0;

	/// <summary>
	/// Frees resources associated with this.
	/// </summary>
	virtual void dispose() = 0;

};