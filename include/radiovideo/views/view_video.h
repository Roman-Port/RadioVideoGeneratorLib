#pragma once

#include <radiovideo/pixel.h>
#include <radiovideo/views/view.h>
#include <radiovideo/misc/bundle.h>

class radio_view_video : public radio_view {

public:
	radio_view_video(context_channel* context, canvas_config_bundle* config);
	virtual void layout(int* width, int* height) override;
	virtual bool query_video_data(pixel_t** outCanvas, int* outX, int* outY, int* outWidth, int* outHeight) override;
	virtual void dispose() override;

public:
	pixel_t* get_pixel(int x, int y);
	pixel_t* get_canvas();
	int get_width();
	int get_height();
	int get_offset_x();
	int get_offset_y();
	
protected:
	void fill(int x, int y, int fillWidth, int fillHeight, pixel_t filling);
	void outline(int offsetX, int offsetY, int boxWidth, int boxHeight, int size, pixel_t filling);

private:
	pixel_t* canvas;
	int width;
	int height;
	int offset_x;
	int offset_y;

};