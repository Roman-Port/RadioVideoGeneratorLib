#include <radiovideo/views/video/view_pi_id.h>
#include <picodes.h>

radio_view_video_pi_id::radio_view_video_pi_id(context_channel* context, canvas_config_bundle* bundle) : radio_view_video(context, bundle),
renderer(get_canvas(), get_width(), bundle->get_font("font"), bundle->get_color("foreground"), bundle->get_color("background"), true, 0),
mode(bundle->get_int("mode")),
rds(nullptr),
stale(true),
demo(bundle->get_int("demo"))
{
	fill(0, 0, get_width(), get_height(), bundle->get_color("background"));
}

void radio_view_video_pi_id::configure(float sampleRate, int fps) {
	//Get and set up RDS, if available
	if (get_context()->get_module(CHANNEL_MODULE_RDS_CLIENT, (void**)&rds)) {
		rds->on_sync_changed.bind(this);
		rds->on_picode_update.bind(this);
	}
	else {
		throw std::runtime_error("No RDS demodulator component! Add an FM demodulator, or remove the RDS view.");
	}
}

void radio_view_video_pi_id::process(raptor_complex* input, int count) {
	if (rds != nullptr && stale) {
		//Get the station
		picodes_station_t* station = picodes_get_station(demo ? 0x3C0C : rds->picode);
		if ((!demo && !rds->has_sync) || station == 0) {
			//Nothing to show! Clear...
			draw_single("");
		}
		else {
			//Switch on the mode
			switch (mode) {
			case VIEW_PIID_MODE_CALL:
			{
				//Simply show the call on one line
				char buffer[16];
				sprintf_s(buffer, sizeof(buffer), "%s-FM", station->callsign);
				draw_single(buffer);
				break;
			}
			case VIEW_PIID_MODE_CALL_FREQ:
			{
				//Show the call and freq on the same line
				char buffer[16];
				sprintf_s(buffer, sizeof(buffer), "%i.%i FM %s", station->freq / 10, station->freq % 10, station->callsign);
				draw_single(buffer);
				break;
			}
			case VIEW_PIID_MODE_CALL_FREQ_DUAL:
			{
				//Show the call on one line and the freq on the other
				char bufferA[16];
				char bufferB[16];
				sprintf_s(bufferA, sizeof(bufferA), "%i.%i FM", station->freq / 10, station->freq % 10);
				sprintf_s(bufferB, sizeof(bufferB), "%s", station->callsign);
				draw_double(bufferA, bufferB);
				break;
			}
			default:
				throw std::runtime_error("Unknown mode.");
			}
		}

		//Update state
		stale = false;
	}
}

void radio_view_video_pi_id::dispose() {
	radio_view_video::dispose();
}

void radio_view_video_pi_id::handle_rds_sync_changed(rds_event_sync_changed_t* evt) {
	stale = true;
}

void radio_view_video_pi_id::handle_rds_picode_update(rds_event_picode_update_t* evt) {
	stale = true;
}

void radio_view_video_pi_id::draw_single(char* buffer) {
	renderer.draw_line_centered(buffer, strlen(buffer), 0, 0, get_width(), get_height(), FONT_HCENTER_CENTER, FONT_VCENTER_CENTER);
}

void radio_view_video_pi_id::draw_double(char* bufferA, char* bufferB) {
	int height = get_height() / 2;
	renderer.draw_line_centered(bufferA, strlen(bufferA), 0, 0, get_width(), height, FONT_HCENTER_CENTER, FONT_VCENTER_BOTTOM);
	renderer.draw_line_centered(bufferB, strlen(bufferB), 0, get_height() - height, get_width(), height, FONT_HCENTER_CENTER, FONT_VCENTER_TOP);
}