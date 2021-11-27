#include <radiovideo/misc/error.h>
#include <cstring>
#include <algorithm>

canvas_error::canvas_error() {
	memset(title, 0, sizeof(title));
	memset(body, 0, sizeof(body));
}

canvas_error::canvas_error(const char* title, const char* body) : canvas_error() {
	set_title(title);
	set_body(body);
}

void copy_string(char* dst, const char* src, size_t len) {
	len = std::min(len - 1, strlen(src));
	memcpy(dst, src, len);
	dst[len] = 0;
}

void canvas_error::set_title(const char* title) {
	copy_string(this->title, title, sizeof(this->title));
}

void canvas_error::set_body(const char* body) {
	copy_string(this->body, body, sizeof(this->body));
}

size_t canvas_error::stringify(char* result, size_t size) {
	return snprintf(result, size, "%s -> \n%s", title, body);
}