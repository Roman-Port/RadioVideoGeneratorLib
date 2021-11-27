#pragma once

#include <radiovideo/pixel.h>

/// <summary>
/// Defines how to draw colors
/// </summary>
class radio_paint {

public:
	/// <summary>
	/// Calculates the value for output.
	/// </summary>
	/// <param name="output">The value to be written.</param>
	/// <param name="value">The position, between 0-1, inclusive.</param>
	virtual void calculate(pixel_t* output, float value) = 0;

};