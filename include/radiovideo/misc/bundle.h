#pragma once

#include <cstring>
#include <radiovideo/pixel.h>
#include <radiovideo/graphics/text/font.h>
#include <radiovideo/misc/writer.h>
#include <stdexcept>

#define CANVAS_CONFIG_BUNDLE_TYPE_INT 1
#define CANVAS_CONFIG_BUNDLE_TYPE_STRING 2
#define CANVAS_CONFIG_BUNDLE_TYPE_FONT 3
#define CANVAS_CONFIG_BUNDLE_TYPE_COLOR 4
#define CANVAS_CONFIG_BUNDLE_TYPE_BUNDLE 5
#define CANVAS_CONFIG_BUNDLE_TYPE_FLOAT 6
#define CANVAS_CONFIG_BUNDLE_TYPE_LONG 7

struct canvas_config_bundle_element_t {

	char key[33];
	int type;
	size_t value_len;
	void* value;
	canvas_config_bundle_element_t* next;

};

#define BUNDLE_DECLARE_NORMAL_TYPE(name, structType, idType) \
void put_##name##(const char* key, structType value) { put(key, idType, sizeof(structType), &value); } \
structType get_##name##(const char* key) { structType value; if(!get(key, idType, sizeof(structType), &value)) { throw std::runtime_error("Required bundle item ("#idType") wasn't found: " + std::string(key)); } return value; }

class canvas_config_bundle {

public:
	canvas_config_bundle();
	~canvas_config_bundle();

	int get_count();
	bool enumerate(canvas_config_bundle_element_t** cursor);

	void save(canvas_config_bundle_writer* writer);

	BUNDLE_DECLARE_NORMAL_TYPE(int, int, CANVAS_CONFIG_BUNDLE_TYPE_INT)
	BUNDLE_DECLARE_NORMAL_TYPE(font, font_t, CANVAS_CONFIG_BUNDLE_TYPE_FONT)
	BUNDLE_DECLARE_NORMAL_TYPE(color, pixel_t, CANVAS_CONFIG_BUNDLE_TYPE_COLOR)
	BUNDLE_DECLARE_NORMAL_TYPE(bundle, canvas_config_bundle*, CANVAS_CONFIG_BUNDLE_TYPE_BUNDLE)
	BUNDLE_DECLARE_NORMAL_TYPE(float, float, CANVAS_CONFIG_BUNDLE_TYPE_FLOAT)
	BUNDLE_DECLARE_NORMAL_TYPE(long, int64_t, CANVAS_CONFIG_BUNDLE_TYPE_LONG)

	void put_string(const char* key, const char* value) { put(key, CANVAS_CONFIG_BUNDLE_TYPE_STRING, strlen(value) + 1, value); }
	bool get_string(const char* key, size_t outSize, char* outValue) { memset(outValue, 0, outSize); return get(key, CANVAS_CONFIG_BUNDLE_TYPE_STRING, outSize - 1, outValue); }

private:
	canvas_config_bundle_element_t* head;
	int count;

	void put(const char* key, int type, size_t valueSize, const void* valueCopySrc);
	bool get(const char* key, int type, size_t dstMaxSize, void* dst);

};