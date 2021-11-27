#pragma once

#include <stdint.h>

/// <summary>
/// A channel is a mutation (decimation, oscilation) of the incoming IQ file.
/// </summary>
struct config_channel_t {

	const char* label;
	int64_t freq_offset;
	float bandwidth;
	float transition_width;

};