#include <radiovideo/views/view.h>

radio_view::radio_view(context_channel* context) {
	this->context = context;
}

context_channel* radio_view::get_context() {
	return context;
}