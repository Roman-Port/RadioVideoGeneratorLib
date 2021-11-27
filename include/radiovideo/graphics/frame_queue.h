#pragma once

#include <radiovideo/pixel.h>
#include <mutex>
#include <condition_variable>

struct frame_queue_element_t {

	pixel_t* src; /* not owned by us; NOT freed when disposed */
	int x;
	int y;
	int width;
	int height;

	pixel_t* queue; /* IS owned by us and IS freed when disposed */

};

class frame_queue {

public:
	frame_queue(int maxElements, int frames);
	void add_element(pixel_t* src, int x, int y, int width, int height);
	void init();
	
	bool enqueue();
	bool dequeue(pixel_t* ptr, size_t stride);

	void dispose();

private:
	frame_queue_element_t* elements;
	int elements_count;

	bool disposing;

	int buffer_use;
	int buffer_len;
	int buffer_read;
	int buffer_write;

	std::mutex m;
	std::condition_variable cv;

};