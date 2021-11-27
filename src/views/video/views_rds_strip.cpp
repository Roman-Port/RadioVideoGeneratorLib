#include <radiovideo/views/video/view_rds_strip.h>
#include <radiovideo/views/data/view_markers.h>
#include <picodes.h>

radio_view_video_rds_strip::radio_view_video_rds_strip(context_channel* context, canvas_config_bundle* bundle) : radio_view_video(context, bundle),
	renderer_rds_ps(get_canvas(), get_width(), bundle->get_font("ps_font"), bundle->get_color("ps_foreground"), bundle->get_color("ps_background"), true, 0),
	renderer_rds_rt(get_canvas(), get_width(), bundle->get_font("rt_font"), bundle->get_color("rt_foreground"), bundle->get_color("rt_background"), true, 0),
	rds(nullptr),
	ps_stale(true),
	rt_stale(true),
	rt_wrapping(bundle->get_int("text_wrapping"))
{
	//Read fields
	reset_on_picode = bundle->get_int("reset_picode");
	reset_on_sync = bundle->get_int("reset_sync");
	border_color = bundle->get_color("border_color");
	border_width = bundle->get_int("border_width");
	padding = bundle->get_int("padding");

	//Measure the PS width
	int psWidth = renderer_rds_ps.measure_line_width_monospaced(8);

	//Configure PS
	ps_rect.x = 0;
	ps_rect.y = 0;
	ps_rect.height = get_height();
	ps_rect.width = padding + psWidth + padding;

	//Configure RT to fill the remaining space
	rt_rect.x = ps_rect.x + ps_rect.width;
	rt_rect.y = 0;
	rt_rect.height = ps_rect.height;
	rt_rect.width = get_width() - rt_rect.x;

	//Fill the RT area
	fill(rt_rect.x, rt_rect.y, rt_rect.width, rt_rect.height, bundle->get_color("rt_background"));

	//Offset
	rt_rect.x += padding;
	rt_rect.width -= padding;
}

void radio_view_video_rds_strip::configure(float sampleRate, int fps) {
	//Get and set up RDS, if available
	if (get_context()->get_module(CHANNEL_MODULE_RDS_CLIENT, (void**)&rds)) {
		rds->on_sync_changed.bind(this);
		rds->on_picode_update.bind(this);
		rds->on_ps_partial_update.bind(this);
		rds->on_rt_partial_update.bind(this);
	}
	else {
		throw std::runtime_error("No RDS demodulator component! Add an FM demodulator, or remove the RDS view.");
	}
}

void radio_view_video_rds_strip::process(raptor_complex* input, int count) {
	if (rds != nullptr && (ps_stale || rt_stale)) {
		//Draw each field
		if (ps_stale)
			renderer_rds_ps.draw_line_centered(rds->ps, RDS_PS_LEN, ps_rect, FONT_HCENTER_CENTER, FONT_VCENTER_CENTER);
		if (rt_stale)
			draw_rt();

		//Do border
		if (border_width > 0)
			outline(0, 0, get_width(), get_height(), border_width, border_color);

		//Reset
		ps_stale = false;
		rt_stale = false;
	}
}

void radio_view_video_rds_strip::draw_rt() {
	//Obliterate the RT text for a test...
	//memcpy(rds->rt, "Turn The Page (Live) - Bob Seger & The Silver Bullet Band on KQR", RDS_RT_LEN);

	//Determine the length of the text by searching for printable characters
	int len = RDS_RT_LEN;
	while (len > 0 && rds->rt[len - 1] < ' ')
		len--;

	//Draw in single-line mode
	int printed = renderer_rds_rt.draw_line_centered(rds->rt, RDS_RT_LEN, rt_rect, FONT_HCENTER_LEFT, FONT_VCENTER_CENTER);

	//If we should wrap it (if it's always on, or if it's in auto and we didn't print all) redraw it like so
	if (rt_wrapping == RDS_STRIP_WRAPPING_MODE_ON || (rt_wrapping == RDS_STRIP_WRAPPING_MODE_AUTO && printed < len)) {
		int height = rt_rect.height / 2;
		renderer_rds_rt.draw_line_centered(rds->rt, printed, rt_rect.x, rt_rect.y, rt_rect.width, height, FONT_HCENTER_LEFT, FONT_VCENTER_BOTTOM);
		renderer_rds_rt.draw_line_centered(&rds->rt[printed], len - printed, rt_rect.x, rt_rect.y + height, rt_rect.width, height, FONT_HCENTER_LEFT, FONT_VCENTER_TOP);
	}
}

void radio_view_video_rds_strip::dispose() {
	radio_view_video::dispose();
}

void radio_view_video_rds_strip::handle_rds_sync_changed(rds_event_sync_changed_t* evt) {
	if (!evt->sync && reset_on_sync && rds != nullptr)
		rds->reset();
}

void radio_view_video_rds_strip::handle_rds_picode_update(rds_event_picode_update_t* evt) {
	if (evt->picode != 0 && reset_on_picode && rds != nullptr)
		rds->reset();
}

void radio_view_video_rds_strip::handle_rds_ps_partial_update(rds_event_ps_partial_update_t* evt) {
	ps_stale = true;
}

void radio_view_video_rds_strip::handle_rds_rt_partial_update(rds_event_rt_partial_update_t* evt) {
	rt_stale = true;
}