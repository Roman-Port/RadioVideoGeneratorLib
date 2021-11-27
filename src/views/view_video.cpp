#include <radiovideo/views/view_video.h>
#include <cassert>
#include <malloc.h>

radio_view_video::radio_view_video(context_channel* context, canvas_config_bundle* bundle) : radio_view(context) {
	//Set
	width = bundle->get_int("width");
	height = bundle->get_int("height");
	offset_x = bundle->get_int("x");
	offset_y = bundle->get_int("y");

	//Allocate canvas
	int size = width * height;
	canvas = (pixel_t*)malloc(sizeof(pixel_t) * size);
	assert(canvas != 0);

	//Fill canvas with red
	for (int i = 0; i < size; i++) {
		canvas[i].r = 255;
		canvas[i].g = 0;
		canvas[i].b = 0;
	}
}

void radio_view_video::layout(int* width, int* height) {
	int maxPosX = get_offset_x() + get_width();
	int maxPosY = get_offset_y() + get_height();
	if (maxPosX > *width)
		(*width) = maxPosX;
	if (maxPosY > *height)
		(*height) = maxPosY;
}

bool radio_view_video::query_video_data(pixel_t** outCanvas, int* outX, int* outY, int* outWidth, int* outHeight) {
	(*outCanvas) = get_canvas();
	(*outX) = get_offset_x();
	(*outY) = get_offset_y();
	(*outWidth) = width;
	(*outHeight) = height;
	return true;
}

pixel_t* radio_view_video::get_canvas() {
	return canvas;
}

int radio_view_video::get_width() {
	return width;
}

int radio_view_video::get_height() {
	return height;
}

int radio_view_video::get_offset_x() {
	return offset_x;
}

int radio_view_video::get_offset_y() {
	return offset_y;
}

pixel_t* radio_view_video::get_pixel(int x, int y) {
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	return &canvas[(y * width) + x];
}

void radio_view_video::fill(int offsetX, int offsetY, int fillWidth, int fillHeight, pixel_t filling) {
	pixel_t* ptr;
	for (int y = 0; y < fillHeight; y++) {
		ptr = get_pixel(offsetX, offsetY + y);
		for (int x = 0; x < fillWidth; x++)
			ptr[x] = filling;
	}
}

void radio_view_video::outline(int offsetX, int offsetY, int boxWidth, int boxHeight, int size, pixel_t filling) {
	fill(offsetX, offsetY, boxWidth, size, filling); //top
	fill(offsetX, offsetY, size, boxHeight, filling); //left
	fill(offsetX, offsetY + boxHeight - size, boxWidth, size, filling); //bottom
	fill(offsetX + boxWidth - size, offsetY, size, boxHeight, filling); //right
}

void radio_view_video::dispose() {
	free(canvas);
}