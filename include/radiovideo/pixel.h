#pragma once

struct pixel_t {

	unsigned char r;
	unsigned char g;
	unsigned char b;

	pixel_t() {
		this->r = 0;
		this->g = 0;
		this->b = 0;
	}

	pixel_t(unsigned char r, unsigned char g, unsigned char b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	pixel_t(unsigned char brightness) : pixel_t(brightness, brightness, brightness) {
		
	}

};

#define COLOR_WHITE pixel_t(255, 255, 255)
#define COLOR_BLACK pixel_t(0, 0, 0)
#define COLOR_RED pixel_t(255, 0, 0)
#define COLOR_GREEN pixel_t(0, 255, 0)
#define COLOR_BLUE pixel_t(0, 0, 255)