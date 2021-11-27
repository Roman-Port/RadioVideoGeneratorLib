#pragma once

class canvas_error {

public:
	canvas_error();
	canvas_error(const char* title, const char* body);
	void set_title(const char* title);
	void set_body(const char* body);
	size_t stringify(char* result, size_t size);

private:
	char title[64];
	char body[512];

};