#include <radiovideo/contexts/context_channel_impl.h>
#include <cassert>
#include <volk/volk.h>
#include <cmath>
#include <raptordsp/filter/builder/builder_lowpass.h>

context_channel_impl::context_channel_impl(context_canvas* canvas, config_channel_t* config) : context_channel_impl(canvas, config->freq_offset, config->bandwidth, config->transition_width) {

}

context_channel_impl::context_channel_impl(context_canvas* canvas, int64_t freq_offset, float bandwidth, float transition_width) : queue(CHANNEL_MAX_OUTPUTS, 10) {
	//Set
	this->canvas = canvas;
	this->bandwidth = bandwidth;
	this->transition_width = transition_width;
	this->freq_offset = freq_offset;

	//Reset
	this->configured = false;
	this->disposing = false;
	this->disposed = false;

	//Open input file
	input = fopen(canvas->get_filename(), "rb");
	assert(input != nullptr);

	//Clear modules
	memset(modules, 0, sizeof(void*) * CHANNEL_MAX_MODULES);

	//Clear outputs
	memset(modules, 0, sizeof(radio_view*) * CHANNEL_MAX_OUTPUTS);
	outputs_count = 0;
}

context_canvas* context_channel_impl::get_canvas() {
	return canvas;
}

void context_channel_impl::put_module(int id, void* mod) {
	assert(!configured);
	assert(id < CHANNEL_MAX_MODULES);
	assert(mod != nullptr);
	if (modules[id] != nullptr)
		throw std::runtime_error("Attempted to add two of the same components!");
	modules[id] = mod;
}

bool context_channel_impl::get_module(int id, void** output) {
	if (id < CHANNEL_MAX_MODULES)
		(*output) = modules[id];
	else
		(*output) = nullptr;
	return (*output) != nullptr;
}

void context_channel_impl::put_output(radio_view* output) {
	assert(!configured);
	assert(outputs_count < CHANNEL_MAX_OUTPUTS);
	outputs[outputs_count++] = output;
	
	//Add to video queue
	pixel_t* canvas;
	int x;
	int y;
	int width;
	int height;
	if (output->query_video_data(&canvas, &x, &y, &width, &height))
		queue.add_element(canvas, x, y, width, height);
}

void context_channel_impl::layout(int* width, int* height) {
	for (int i = 0; i < outputs_count; i++)
		outputs[i]->layout(width, height);
}

void context_channel_impl::configure(float sampleRate, int fps) {
	//Allocate the buffer to store our rotated samples
	buffer = (raptor_complex*)volk_malloc(get_canvas()->get_buffer_size() * sizeof(raptor_complex), volk_get_alignment());
	buffer_input = (short*)volk_malloc(get_canvas()->get_buffer_size() * sizeof(short) * 2, volk_get_alignment());

	//Configure rotator, this shifts the signal we want to the center
	float angle = 2.0f * 3.1415926535897932384626433f * freq_offset / sampleRate;
	phase = lv_cmake(1.f, 0.0f);
	phase_inc = lv_cmake(std::cos(angle), std::sin(angle));

	//Set some values to defaults if needed
	if (bandwidth <= 0)
		bandwidth = sampleRate;
	if (transition_width <= 0)
		transition_width = bandwidth * 0.1f;

	//Build the lowpass filter for decimation
	raptor_filter_builder_lowpass builder(sampleRate, bandwidth * 0.5f);
	builder.automatic_tap_count(transition_width);
	decimation = builder.calculate_decimation(&sampleRate);
	if (decimation > 1)
		filter.configure(builder.build_taps_real(), decimation);

	//Dispatch to all outputs
	for (int i = 0; i < outputs_count; i++)
		outputs[i]->configure(sampleRate, fps);

	//Set flag
	configured = true;

	//Spin up worker
	worker = new std::thread(worker_thread, this);
}

void context_channel_impl::process_sync(raptor_complex* input, int count) {
	//Make sure we're configured
	assert(configured);

	//Rotate
	volk_32fc_s32fc_x2_rotator_32fc(buffer, input, phase_inc, &phase, count);

	//Filter
	if (decimation > 1)
		count = filter.process(buffer, buffer, count);

	//Process all
	for (int i = 0; i < outputs_count; i++)
		outputs[i]->process(buffer, count);
}

bool context_channel_impl::read(pixel_t* frame, size_t stride) {
	return queue.dequeue(frame, stride);
}

void* context_channel_impl::worker_thread(void* c) {
	context_channel_impl* ctx = (context_channel_impl*)c;
	int count;
	bool exit = false;
	do {
		//Read from input file
		count = fread(ctx->buffer_input, sizeof(short) * 2, ctx->get_canvas()->get_buffer_size(), ctx->input);

		//Exit if we've reached the end of file
		if (count != ctx->get_canvas()->get_buffer_size())
			break;

		//Convert from short-complex to float-complex
		volk_16i_s32f_convert_32f((float*)ctx->buffer, ctx->buffer_input, 32768, count * 2);

		//Process as normal
		try {
			ctx->process_sync(ctx->buffer, count);
		}
		catch (std::runtime_error ex) {
			ctx->get_canvas()->report_error(canvas_error("UNEXPECTED ERROR processing channel", ex.what()));
		}

		//Push to queue
		ctx->queue.enqueue();

		//Check if it is time to exit
		ctx->exiting_m.lock();
		exit = ctx->disposing;
		ctx->exiting_m.unlock();
	} while (!exit);

	//Notify that we're exiting
	ctx->exiting_m.lock();
	ctx->disposing = true;
	ctx->disposed = true;
	ctx->exiting_m.unlock();
	ctx->exiting_cv.notify_all();

	return 0;
}

void context_channel_impl::dispose() {
	//Dispose of the frame queue. This'll cause all write/reads from this to be cancelled, signaling to all parts that we're stopping
	queue.dispose();

	//Notify
	exiting_m.lock();
	disposing = true;
	exiting_m.unlock();
	exiting_cv.notify_all();

	//Wait for the worker to exit
	std::unique_lock<std::mutex> lk(exiting_m);
	while (!disposed)
		exiting_cv.wait(lk);

	//Destroy outputs
	for (int i = 0; i < outputs_count; i++)
		outputs[i]->dispose();
}