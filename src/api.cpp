#include <radiovideo/api.h>
#include <radiovideo/views/video/view_spectrum_waterfall.h>
#include <radiovideo/views/video/view_spectrum.h>
#include <radiovideo/views/video/view_station_label.h>
#include <radiovideo/views/video/view_rds_strip.h>
#include <radiovideo/views/video/view_pi_id.h>
#include <radiovideo/views/audio/view_fm.h>
#include <radiovideo/views/data/view_markers.h>
#include <radiovideo/views/data/view_rds.h>
#include <radiovideo/graphics/paint_solid.h>
#include <radiovideo/graphics/paint_gradient.h>
#include <radiovideo/config/config_view_audio_fm.h>

/* CANVAS */

int radiovideo_canvas_get_width(context_canvas_impl* canvas) {
	return canvas->get_width();
}

int radiovideo_canvas_get_height(context_canvas_impl* canvas) {
	return canvas->get_height();
}

int64_t radiovideo_canvas_get_length_frames(context_canvas_impl* canvas) {
	return canvas->get_length_frames();
}

int radiovideo_canvas_process(context_canvas_impl* canvas, pixel_t** frame) {
	return canvas->process(frame);
}

int radiovideo_canvas_check_error(context_canvas_impl* canvas, char* buffer, int bufferLen) {
	return canvas->check_error(buffer, bufferLen);
}

void radiovideo_canvas_dispose(context_canvas_impl* canvas) {
	canvas->dispose();
}

/* BUNDLE */

canvas_config_bundle* radiovideo_bundle_create() {
	return new canvas_config_bundle();
}

void radiovideo_bundle_put_int(canvas_config_bundle* bundle, const char* key, int value) {
	bundle->put_int(key, value);
}

void radiovideo_bundle_put_string(canvas_config_bundle* bundle, const char* key, const char* value) {
	bundle->put_string(key, value);
}

void radiovideo_bundle_put_font(canvas_config_bundle* bundle, const char* key, font_t value) {
	bundle->put_font(key, value);
}

void radiovideo_bundle_put_color(canvas_config_bundle* bundle, const char* key, uint8_t r, uint8_t g, uint8_t b) {
	bundle->put_color(key, pixel_t(r, g, b));
}

void radiovideo_bundle_put_bundle(canvas_config_bundle* bundle, const char* key, canvas_config_bundle* child) {
	bundle->put_bundle(key, child);
}

void radiovideo_bundle_put_float(canvas_config_bundle* bundle, const char* key, float value) {
	bundle->put_float(key, value);
}

void radiovideo_bundle_put_long(canvas_config_bundle* bundle, const char* key, int64_t value) {
	bundle->put_long(key, value);
}

void radiovideo_bundle_destroy(canvas_config_bundle* bundle) {
	delete(bundle);
}

radio_view* radiovideo_inflate_view(context_channel_impl* channel, canvas_config_bundle* bundle) {
	radio_view* view = nullptr;
	switch (bundle->get_int("view_type")) {
	case RADIOVIDEO_VIEW_VIDEO_SPECTRUM: view = new radio_view_video_spectrum(channel, bundle); break;
	case RADIOVIDEO_VIEW_VIDEO_SPECTRUM_WATERFALL: view = new radio_view_video_spectrum_waterfall(channel, bundle); break;
	case RADIOVIDEO_VIEW_VIDEO_STATION_LABEL: view = new radio_view_video_station_label(channel, bundle); break;
	case RADIOVIDEO_VIEW_VIDEO_RDS_STRIP: view = new radio_view_video_rds_strip(channel, bundle); break;
	case RADIOVIDEO_VIEW_VIDEO_PI_ID: view = new radio_view_video_pi_id(channel, bundle); break;
	case RADIOVIDEO_VIEW_AUDIO_FM: view = new radio_view_audio_fm(channel, bundle); break;
	case RADIOVIDEO_VIEW_DATA_MARKERS: view = new radio_view_data_markers(channel, bundle); break;
	case RADIOVIDEO_VIEW_DATA_RDS_DEMOD: view = new radio_view_data_rds(channel, bundle); break;
	}
	return view;
}

context_canvas* radiovideo_inflate(canvas_config_bundle* bundle, char* error, size_t errorLen) {
	try {
		//Create the canvas
		char input_filename[1024];
		char output_filename[1024];
		bundle->get_string("input_filename", sizeof(input_filename), input_filename);
		bundle->get_string("output_filename", sizeof(output_filename), output_filename);
		context_canvas_impl* canvas = new context_canvas_impl(
			input_filename,
			output_filename,
			bundle->get_int("fps"),
			bundle->get_int("dry_run"),
			bundle->get_color("background_color")
		);

		//Inflate each channel
		canvas_config_bundle* channels = bundle->get_bundle("channels");
		canvas_config_bundle_element_t* channelCursor = nullptr;
		canvas_config_bundle* channelBundle = nullptr;
		while (channels->enumerate(&channelCursor)) {
			//Get the actual bundle
			channelBundle = channels->get_bundle(channelCursor->key);

			//Create the channel (do I need to add this?)
			context_channel_impl* channel = new context_channel_impl(
				canvas,
				bundle->get_long("center_freq") - channelBundle->get_long("freq"),
				channelBundle->get_float("bandwidth"),
				channelBundle->get_float("transition_width")
			);
			canvas->add_channel(channel);

			//Read the views
			canvas_config_bundle* views = channelBundle->get_bundle("views");
			canvas_config_bundle_element_t* viewCursor = nullptr;
			while (views->enumerate(&viewCursor)) {
				//Construct
				radio_view* view = radiovideo_inflate_view(channel, views->get_bundle(viewCursor->key));

				//Add or throw an error
				if (view == nullptr)
					canvas->report_error(canvas_error("Internal Error Adding View", "Unknown view type."));
				else
					channel->put_output(view);
			}
		}

		//Commit
		canvas->configure();

		return canvas;
	}
	catch (std::runtime_error ex) {
		strcpy_s(error, errorLen - 1, ex.what());
		return 0;
	}
}

/* BUNDLE WRITER */

canvas_config_bundle_writer* radiovideo_bundlerwriter_serialize(canvas_config_bundle* bundle, int* len) {
	canvas_config_bundle_writer* writer = new canvas_config_bundle_writer();
	bundle->save(writer);
	(*len) = writer->get_size();
	return writer;
}

void radiovideo_bundlerwriter_read(canvas_config_bundle_writer* writer, int len, void* buffer) {
	writer->read(buffer, len);
}

void radiovideo_bundlerwriter_dispose(canvas_config_bundle_writer* reader) {
	delete(reader);
}