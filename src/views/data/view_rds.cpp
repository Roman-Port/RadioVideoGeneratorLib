#include <radiovideo/views/data/view_rds.h>
#include <raptordsp/fast_atan2.h>
#include <cassert>

radio_view_data_rds::radio_view_data_rds(context_channel* context, canvas_config_bundle* bundle) : radio_view_demod(context, bundle),
	rds(context->get_canvas()->get_buffer_size()),
    mpx_buffer((float*)malloc(sizeof(float) * context->get_canvas()->get_buffer_size())),
    gain(0),
    last_sample(0)
{
    //Validate
    assert(mpx_buffer != nullptr);

    //Register modules
    context->put_module(CHANNEL_MODULE_RDS_CLIENT, &rds);
}

void radio_view_data_rds::configure_demod(float sampleRate) {
    //Set up FM baseband demodulation
    gain = sampleRate / (2 * M_PI * 75000);
    last_sample = 0;

    //Configure RDS
	rds.configure(sampleRate);
}

void radio_view_data_rds::process_demod(raptor_complex* input, int count) {
    //Demodulate baseband signal
    raptor_complex temp;
    for (int i = 0; i < count; i++) {
        //Apply conjugate
        temp = input[i] * std::conj(last_sample);

        //Estimate angle
        mpx_buffer[i] = fast_atan2f(imag(temp), real(temp)) * gain;

        //Set state
        last_sample = input[i];
    }

    //Process RDS
    rds.process(mpx_buffer, count);
}

void radio_view_data_rds::dispose() {
    free(mpx_buffer);
    delete(this);
}