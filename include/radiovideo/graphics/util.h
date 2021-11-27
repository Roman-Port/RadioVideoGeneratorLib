#pragma once

#include <radiovideo/pixel.h>
#include <radiovideo/misc/bundle.h>

void util_interpolate_colors(const pixel_t* buffer, int bufferCount, float interp, pixel_t* output);
void util_unpack_colors(const pixel_t* buffer, int bufferCount, pixel_t* output, int outputCount);
void util_unpack_paint(canvas_config_bundle* paint, pixel_t* output, int outputCount);