#include <radiovideo/graphics/frame_queue.h>
#include <malloc.h>
#include <cstring>

frame_queue::frame_queue(int maxElements, int frames) :
	buffer_len(frames),
	buffer_use(0),
	buffer_read(0),
	buffer_write(0),
	disposing(false),
	elements_count(0)
{
	elements = (frame_queue_element_t*)malloc(sizeof(frame_queue_element_t) * maxElements * frames);
}

void frame_queue::add_element(pixel_t* src, int x, int y, int width, int height) {
	elements[elements_count].src = src;
	elements[elements_count].x = x;
	elements[elements_count].y = y;
	elements[elements_count].width = width;
	elements[elements_count].height = height;
	elements[elements_count].queue = (pixel_t*)malloc(sizeof(pixel_t) * width * height * buffer_len);
	elements_count++;
}

bool frame_queue::enqueue() {
	//Wait for a spot to become available
	std::unique_lock<std::mutex> lk(m);
	while (!disposing && buffer_use == buffer_len)
		cv.wait(lk);
	if (disposing)
		return false;

	//Go through each element and transfer it into it's respective queue
	for (int i = 0; i < elements_count; i++) {
		memcpy(
			&elements[i].queue[buffer_write * elements[i].width * elements[i].height],
			elements[i].src,
			sizeof(pixel_t) * elements[i].width * elements[i].height
		);
	}

	//Update state
	buffer_use++;
	buffer_write = (buffer_write + 1) % buffer_len;

	//Unlock
	lk.unlock();

	//Notify
	cv.notify_one();

	return true;
}

bool frame_queue::dequeue(pixel_t* ptr, size_t stride) {
	//Wait for a frame
	std::unique_lock<std::mutex> lk(m);
	while (!disposing && buffer_use == 0)
		cv.wait(lk);
	if (disposing)
		return false;

	//Go through each element and transfer it into it's respctive output location
	pixel_t* dst;
	pixel_t* src;
	for (int i = 0; i < elements_count; i++) {
		//Get output and input
		dst = &ptr[(stride * elements[i].y) + elements[i].x];
		src = &elements[i].queue[buffer_read * elements[i].width * elements[i].height];

		//Copy line-by-line
		for (int line = 0; line < elements[i].height; line++) {
			memcpy(dst, src, sizeof(pixel_t) * elements[i].width);
			dst += stride;
			src += elements[i].width;
		}
	}

	//Update state
	buffer_use--;
	buffer_read = (buffer_read + 1) % buffer_len;

	//Unlock
	lk.unlock();

	//Notify
	cv.notify_one();

	return true;
}

void frame_queue::dispose() {
	//Lock
	m.lock();

	//Only proceed if we aren't already disposing
	if (!disposing) {
		//Update
		disposing = true;

		//Dispose
		for (int i = 0; i < elements_count; i++)
			free(elements[i].queue);
		free(elements);
	}

	//Unlock
	m.unlock();

	//Notify
	cv.notify_all();
}