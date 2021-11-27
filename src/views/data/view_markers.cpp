#include <radiovideo/views/data/view_markers.h>
#include <cassert>

radio_view_data_markers::radio_view_data_markers(context_channel* context, canvas_config_bundle* bundle) : radio_view(context),
	current_frame(0),
	current_index(0)
{
	//Get the data bundle 
	canvas_config_bundle* markersBundle = bundle->get_bundle("markers");

	//Allocate and set
	data_count = markersBundle->get_count();
	data = (config_view_data_markers_point_t*)malloc(sizeof(config_view_data_markers_point_t) * data_count);
	assert(data != nullptr);

	//Unpack
	int index = 0;
	canvas_config_bundle_element_t* cursor = nullptr;
	canvas_config_bundle* dataBundle;
	while (markersBundle->enumerate(&cursor)) {
		//Get
		dataBundle = markersBundle->get_bundle(cursor->key);

		//Read into data
		data[index].color = dataBundle->get_color("color");
		data[index].frame = (dataBundle->get_int("timestamp") * context->get_canvas()->get_framerate()) / 1000;
		dataBundle->get_string("text", sizeof(data[index].text), data[index].text);
	}

	//Register module
	if(data_count > 0)
		context->put_module(CHANNEL_MODULE_MARKERS, this);
}

void radio_view_data_markers::configure(float sampleRate, int fps) {

}

void radio_view_data_markers::process(raptor_complex* input, int count) {
	//Advance frame counter
	current_frame++;

	//Check if we need to update
	while (current_index + 1 < data_count && data[current_index + 1].frame <= current_frame)
		current_index++;
}

void radio_view_data_markers::layout(int* width, int* height) {

}

config_view_data_markers_point_t* radio_view_data_markers::get_current_item() {
	assert(current_index < data_count);
	return &data[current_index];
}

void radio_view_data_markers::dispose() {
	free(data);
}