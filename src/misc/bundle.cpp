#include <radiovideo/misc/bundle.h>
#include <malloc.h>
#include <cassert>
#include <algorithm>

canvas_config_bundle::canvas_config_bundle() : head(nullptr), count(0) {

}

canvas_config_bundle::~canvas_config_bundle() {
	//Travel down the linked list and destroy each element
	canvas_config_bundle_element_t* cursor = head;
	canvas_config_bundle_element_t* next_cursor;
	while (cursor != nullptr) {
		//Free associated buffer
		free(cursor->value);

		//Get the next object in the list
		next_cursor = cursor->next;

		//Free the object itself
		delete(cursor);

		//Move to the next
		cursor = next_cursor;
	}
}

int canvas_config_bundle::get_count() {
	return count;
}

void canvas_config_bundle::put(const char* key, int type, size_t valueSize, const void* valueCopySrc) {
	//Allocate a buffer for the value and transfer it in
	void* buffer = malloc(valueSize);
	assert(buffer != 0);
	memcpy(buffer, valueCopySrc, valueSize);

	//Allocate a new object and put values in it 
	canvas_config_bundle_element_t* obj = new canvas_config_bundle_element_t();
	obj->next = nullptr;
	obj->type = type;
	obj->value = buffer;
	obj->value_len = valueSize;

	//Copy key into this
	size_t keyLen = std::min((size_t)32, strlen(key));
	memcpy(obj->key, key, keyLen);
	obj->key[keyLen] = 0;

	//Advance state
	count++;

	//If we have no head set, set this there
	if (head == nullptr) {
		head = obj;
		return;
	}

	//Go down the linked list until we find a null next reference
	canvas_config_bundle_element_t* cursor = head;
	while (cursor->next != nullptr)
		cursor = cursor->next;

	//Set
	cursor->next = obj;
}

bool canvas_config_bundle::get(const char* key, int type, size_t dstMaxSize, void* dst) {
	//Go down the linked list until we find a value with a matching key
	canvas_config_bundle_element_t* cursor = head;
	while (cursor != nullptr) {
		//Check if this has a matching key and type
		if (cursor->type == type && strcmp(cursor->key, key) == 0) {
			//Determine how much we can copy
			size_t len = std::min(dstMaxSize, cursor->value_len);

			//Copy
			memcpy(dst, cursor->value, len);

			return true;
		}

		//Get next element
		cursor = cursor->next;
	}

	return false;
}

bool canvas_config_bundle::enumerate(canvas_config_bundle_element_t** cursor) {
	if (*cursor == nullptr) {
		//This is the first...get the head
		(*cursor) = head;
	}
	else {
		//Advance to the next
		(*cursor) = (*cursor)->next;
	}
	return (*cursor) != nullptr;
}

#define SANITY_CHECK_BUNDLE_START 0xAAABACAD
#define SANITY_CHECK_PROPERTY_START 0xFAFBFCFD

void canvas_config_bundle::save(canvas_config_bundle_writer* writer) {
	//Add sanity check
	writer->write_int(SANITY_CHECK_BUNDLE_START);

	//Write count
	writer->write_int(count);

	//Write each property
	canvas_config_bundle_element_t* cursor = head;
	while (cursor != nullptr) {
		//Add sanity check
		writer->write_int(SANITY_CHECK_PROPERTY_START);

		//Write header data
		writer->write_bytes(cursor->key, sizeof(cursor->key));
		writer->write_int(cursor->type);
		writer->write_size(cursor->value_len);

		//Write the payload
		switch (cursor->type) {
		case CANVAS_CONFIG_BUNDLE_TYPE_BUNDLE:
			((canvas_config_bundle**)cursor->value)[0]->save(writer);
			break;
		default:
			writer->write_bytes(cursor->value, cursor->value_len);
			break;
		}

		//Advance
		cursor = cursor->next;
	}
}