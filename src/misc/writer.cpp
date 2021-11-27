#include <radiovideo/misc/writer.h>
#include <malloc.h>
#include <cstring>
#include <cassert>

canvas_config_bundle_writer::canvas_config_bundle_writer(size_t len) :
	len(len),
	use(0),
	buffer((uint8_t*)malloc(len))
{
	assert(buffer != nullptr);
}

canvas_config_bundle_writer::canvas_config_bundle_writer(const canvas_config_bundle_writer& src) :
	len(src.len),
	use(src.use),
	buffer((uint8_t*)malloc(src.len))
{
	assert(buffer != nullptr);
	memcpy(buffer, src.buffer, src.len);
}

canvas_config_bundle_writer::~canvas_config_bundle_writer() {
	free(buffer);
}

size_t canvas_config_bundle_writer::get_size() {
	return use;
}

void canvas_config_bundle_writer::read(void* output, size_t size) {
	assert(use == size);
	memcpy(output, buffer, use);
}

void canvas_config_bundle_writer::write_bytes(void* bytes, size_t count) {
	//Make sure there's enough space
	while (use + count >= len) {
		//Determine new length
		size_t newLen = len * 2;

		//Allocate new buffer
		uint8_t* newBuffer = (uint8_t*)malloc(newLen);
		assert(newBuffer != nullptr);

		//Copy over
		memcpy(newBuffer, buffer, newLen);

		//Apply
		buffer = newBuffer;
		len = newLen;
	}

	//Transfer
	memcpy(&buffer[use], bytes, count);
	use += count;
}