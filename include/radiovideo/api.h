#pragma once

#include <radiovideo/contexts/context_canvas_impl.h>
#include <radiovideo/contexts/context_channel_impl.h>
#include <radiovideo/outputs/video/output_video_null.h>
#include <radiovideo/outputs/video/output_video_h264.h>
#include <radiovideo/outputs/audio/output_audio_null.h>
#include <radiovideo/outputs/audio/output_audio_wav.h>
#include <radiovideo/graphics/text/font.h>
#include <radiovideo/paint.h>
#include <radiovideo/misc/bundle.h>
#include <radiovideo/misc/writer.h>

#define EXPORT_API extern "C" __declspec( dllexport )

#define RADIOVIDEO_VIEW_VIDEO_SPECTRUM				101
#define RADIOVIDEO_VIEW_VIDEO_SPECTRUM_WATERFALL	102
#define RADIOVIDEO_VIEW_VIDEO_STATION_LABEL			103
#define RADIOVIDEO_VIEW_VIDEO_RDS_STRIP				104
#define RADIOVIDEO_VIEW_VIDEO_PI_ID 				105
#define RADIOVIDEO_VIEW_AUDIO_FM					201
#define RADIOVIDEO_VIEW_DATA_MARKERS				301
#define RADIOVIDEO_VIEW_DATA_RDS_DEMOD				302

/* CANVAS */

EXPORT_API int radiovideo_canvas_get_width(context_canvas_impl* canvas);
EXPORT_API int radiovideo_canvas_get_height(context_canvas_impl* canvas);
EXPORT_API int64_t radiovideo_canvas_get_length_frames(context_canvas_impl* canvas);
EXPORT_API int radiovideo_canvas_process(context_canvas_impl* canvas, pixel_t** frame);
EXPORT_API int radiovideo_canvas_check_error(context_canvas_impl* canvas, char* buffer, int bufferLen);
EXPORT_API void radiovideo_canvas_dispose(context_canvas_impl* canvas);
EXPORT_API context_canvas* radiovideo_inflate(canvas_config_bundle* bundle, char* error, size_t errorLen);

/* BUNDLE */

EXPORT_API canvas_config_bundle* radiovideo_bundle_create();
EXPORT_API void radiovideo_bundle_put_int(canvas_config_bundle* bundle, const char* key, int value);
EXPORT_API void radiovideo_bundle_put_string(canvas_config_bundle* bundle, const char* key, const char* value);
EXPORT_API void radiovideo_bundle_put_font(canvas_config_bundle* bundle, const char* key, font_t value);
EXPORT_API void radiovideo_bundle_put_color(canvas_config_bundle* bundle, const char* key, uint8_t r, uint8_t g, uint8_t b);
EXPORT_API void radiovideo_bundle_put_bundle(canvas_config_bundle* bundle, const char* key, canvas_config_bundle* child);
EXPORT_API void radiovideo_bundle_put_float(canvas_config_bundle* bundle, const char* key, float value);
EXPORT_API void radiovideo_bundle_put_long(canvas_config_bundle* bundle, const char* key, int64_t value);
EXPORT_API void radiovideo_bundle_destroy(canvas_config_bundle* bundle);

/* BUNDLE - WRITER */

EXPORT_API canvas_config_bundle_writer* radiovideo_bundlerwriter_serialize(canvas_config_bundle* bundle, int* len);
EXPORT_API void radiovideo_bundlerwriter_read(canvas_config_bundle_writer* writer, int len, void* buffer);
EXPORT_API void radiovideo_bundlerwriter_dispose(canvas_config_bundle_writer* writer);