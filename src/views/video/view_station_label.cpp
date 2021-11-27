#include <radiovideo/views/video/view_station_label.h>
#include <radiovideo/views/data/view_markers.h>

#define LABEL_BORDER 2
#define LABEL_PADDING 10
#define LABEL_SUB_SPACING 1
#define LABEL_ICON_SPACING 4
#define LABEL_RDS_HEIGHT 30

#define COLOR_RDS_RT pixel_t(50, 51, 51)
#define COLOR_RDS_PS pixel_t(80, 81, 82)

/* WIDGETS */

class station_label_widget_remotebool : public station_label_widget {
public:
	station_label_widget_remotebool(font_renderer* renderer, bool* widget, char* label) {
		this->widget = widget;
		this->label = label;
		width = renderer->measure_line_width(label, strlen(label)) + LABEL_PADDING + LABEL_PADDING;
	}
	virtual void refresh(pixel_t* outColor, char** outText) override {
		*outText = label;
		*outColor = *widget ? pixel_t(255) : pixel_t(100);
	}
private:
	char* label;
	bool* widget;
};

class station_label_widget_markers : public station_label_widget {
public:
	station_label_widget_markers(font_renderer* renderer, radio_view_data_markers* source) {
		this->source = source;
		this->width = 0;
		for (int i = 0; i < source->data_count; i++)
			width = std::max(width, renderer->measure_line_width(source->data[i].text, strlen(source->data[i].text)));
		width += LABEL_PADDING + LABEL_PADDING;
	}
	virtual void refresh(pixel_t* outColor, char** outText) override {
		*outText = source->get_current_item()->text;
		*outColor = source->get_current_item()->color;
	}
private:
	radio_view_data_markers* source;

};

/* CORE */

radio_view_video_station_label::radio_view_video_station_label(context_channel* context, canvas_config_bundle* bundle) : radio_view_video(context, bundle),
	border_width(bundle->get_int("border_width")),
	padding(bundle->get_int("padding")),
	color_background(bundle->get_color("color_background")),
	color_font(bundle->get_color("color_font")),
	color_border(bundle->get_color("color_border")),
	renderer_title(get_canvas(), bundle->get_int("width"), bundle->get_font("font_title"), color_font, color_background, false, 0),
	renderer_subtext(get_canvas(), bundle->get_int("width"), bundle->get_font("font_subtext"), color_font, color_background, false, 0),
	renderer_icons(get_canvas(), bundle->get_int("width"), bundle->get_font("font_subtext"), color_font, color_background, false, 0),
	widgets_count(0),
	show_detection_icons(bundle->get_int("show_detection_icons"))
{
	//Copy the texts
	bundle->get_string("title", sizeof(text_title), text_title);
	bundle->get_string("subtext_a", sizeof(text_subtext_a), text_subtext_a);
	bundle->get_string("subtext_b", sizeof(text_subtext_b), text_subtext_b);
}

void radio_view_video_station_label::configure(float sampleRate, int fps) {
	//Get modules
	void* temp;
	/*if (get_context()->get_module(CHANNEL_MODULE_MARKERS, &temp))
		push_widget(new station_label_widget_markers(&renderer_icons, (radio_view_data_markers*)temp));*/
	if (show_detection_icons && get_context()->get_module(CHANNEL_MODULE_STEREO_DETECTED, &temp))
		push_widget(new station_label_widget_remotebool(&renderer_icons, (bool*)temp, "ST"));
	if (show_detection_icons && get_context()->get_module(CHANNEL_MODULE_RDS_DETECTED, &temp))
		push_widget(new station_label_widget_remotebool(&renderer_icons, (bool*)temp, "RDS"));

	//Fill all with black
	fill(0, 0, get_width(), get_height(), color_background);

	//Calculate layout
	int titleWidth = padding + renderer_title.measure_line_width(text_title, strlen(text_title)) + padding;
	int subWidth = get_width() - titleWidth - padding;
	int subHeight = get_height() / 2;

	//Draw title text
	renderer_title.draw_line_centered(text_title, strlen(text_title), 0, 0, titleWidth, get_height(), FONT_HCENTER_CENTER, FONT_VCENTER_CENTER);
	renderer_subtext.draw_line_centered(text_subtext_a, strlen(text_subtext_a), titleWidth, 0, subWidth, subHeight, FONT_HCENTER_LEFT, FONT_VCENTER_BOTTOM);
	renderer_subtext.draw_line_centered(text_subtext_b, strlen(text_subtext_b), titleWidth, subHeight, subWidth, subHeight, FONT_HCENTER_LEFT, FONT_VCENTER_TOP);

	//Do border
	if (border_width > 0)
		outline(0, 0, get_width(), get_height(), border_width, color_border);

	//Refresh widgets
	refresh_widgets();
}

void radio_view_video_station_label::process(raptor_complex* input, int count) {
	//Update widgets
	refresh_widgets();
}

void radio_view_video_station_label::push_widget(station_label_widget* widget) {
	widgets[widgets_count++] = widget;
}

void radio_view_video_station_label::refresh_widgets() {
	//Get starting position
	int startX = get_width() - padding;
	int startY = padding;
	int height = get_height() - padding - padding;

	//Loop through each widget
	pixel_t color;
	char* text;
	for (int i = 0; i < widgets_count; i++) {
		//Query it
		widgets[i]->refresh(&color, &text);

		//Reconfigure text renderer
		renderer_icons.update_colors(pixel_t(0), color);

		//Draw
		renderer_icons.draw_line_centered(text, strlen(text), startX - widgets[i]->width, startY, widgets[i]->width, height, FONT_HCENTER_CENTER, FONT_VCENTER_CENTER);

		//Update state
		startX -= widgets[i]->width + LABEL_ICON_SPACING;
	}
}

void radio_view_video_station_label::dispose() {
	for (int i = 0; i < widgets_count; i++)
		delete(widgets[i]);
	radio_view_video::dispose();
}