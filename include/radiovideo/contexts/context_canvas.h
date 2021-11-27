#pragma once

#include <radiovideo/misc/error.h>
#include <radiovideo/outputs/output.h>

/// <summary>
/// Canvas is an entire IQ file and all components and channels under it.
/// </summary>
class context_canvas {

public:
	/// <summary>
	/// Returns the maximum buffer size per frame.
	/// </summary>
	/// <returns></returns>
	virtual int get_buffer_size() = 0;

	/// <summary>
	/// Gets the width of the canvas AFTER configure is called.
	/// </summary>
	/// <returns></returns>
	virtual int get_width() = 0;

	/// <summary>
	/// Gets the width of the canvas AFTER configure is called.
	/// </summary>
	/// <returns></returns>
	virtual int get_height() = 0;

	/// <summary>
	/// Gets the video frame rate.
	/// </summary>
	/// <returns></returns>
	virtual int get_framerate() = 0;

	/// <summary>
	/// Gets the input filename.
	/// </summary>
	/// <returns></returns>
	virtual const char* get_filename() = 0;

	/// <summary>
	/// Sends an error to the user.
	/// </summary>
	/// <param name="text"></param>
	virtual void report_error(canvas_error error) = 0;

	/// <summary>
	/// Creates a video output.
	/// </summary>
	/// <returns></returns>
	virtual radio_output_video* create_output_video(const char* filename) = 0;

	/// <summary>
	/// Creates an audio output.
	/// </summary>
	/// <returns></returns>
	virtual radio_output_audio* create_output_audio(const char* filename) = 0;

	/// <summary>
	/// Frees resources associated with this.
	/// </summary>
	virtual void dispose() = 0;

};