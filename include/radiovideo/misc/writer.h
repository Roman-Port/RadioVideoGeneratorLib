#pragma once

#include <stdint.h>

#define WRITER_DEFINE(name, type) void write_##name##(type value) { write_bytes(&value, sizeof(type)); }

class canvas_config_bundle_writer {

public:
	canvas_config_bundle_writer(size_t len = 1024);
	canvas_config_bundle_writer(const canvas_config_bundle_writer&);
	~canvas_config_bundle_writer();

	size_t get_size();
	void read(void* output, size_t size);

	void write_bytes(void* bytes, size_t len);

	WRITER_DEFINE(int, int32_t)
	WRITER_DEFINE(size, size_t)

private:
	uint8_t* buffer;
	size_t len;
	size_t use;

};