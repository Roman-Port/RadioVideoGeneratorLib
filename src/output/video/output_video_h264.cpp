#include <radiovideo/outputs/video/output_video_h264.h>
#include <cassert>
#include <cstring>
#include <stdexcept>

radio_output_video_h264::radio_output_video_h264(const char* filename) {
    //Open file
    file = fopen(filename, "wb");
    assert(file != 0);

    //Reset
    num_nals = 0;
    frame_count = 0;
}

void radio_output_video_h264::configure(int width, int height, int fps) {
    //Set
    this->width = width;
    this->height = height;

    //Configure H264
    assert(prepare_h264(fps));
}

void radio_output_video_h264::push(pixel_t* frame) {
    //Transfer image
    memcpy(pic_in.img.plane[0], frame, sizeof(pixel_t) * width * height);

    //Encode and store into pic_out
    pic_in.i_pts = pts;

    int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
    if (frame_size) {
        if (!fwrite(nals[0].p_payload, frame_size, 1, file)) {
            throw std::runtime_error("Failed to write to disk! Is it full?");
        }
    }
    ++pts;
}

void radio_output_video_h264::close() {
    x264_picture_clean(&pic_in);
    memset((char*)&pic_in, 0, sizeof(pic_in));
    memset((char*)&pic_out, 0, sizeof(pic_out));

    x264_encoder_close(encoder);
    encoder = NULL;

    fclose(file);
    file = NULL;

    //Destroy
    delete(this);
}

bool radio_output_video_h264::prepare_h264(int fps) {
    //Allocate picture
    if (x264_picture_alloc(&pic_in, X264_CSP_RGB, width, height) < 0)
        throw std::runtime_error("Failed to allocate picture!");

    //Get default params for preset/tuning
    if (x264_param_default_preset(&params, "veryfast", 0) < 0)
        throw std::runtime_error("Failed to set preset!");

    //Apply
    if (x264_param_apply_profile(&params, "high444") < 0)
        throw std::runtime_error("Failed to set profile!");

    //Configure
    params.i_csp = X264_CSP_RGB;
    params.i_threads = 1;
    params.i_width = width;
    params.i_height = height;
    params.i_fps_num = fps;
    params.i_fps_den = 1;
    params.rc.i_rc_method = X264_RC_CRF;
    params.rc.f_rf_constant = 0;
    params.rc.f_rf_constant_max = 0;

    //Open encoder
    encoder = x264_encoder_open(&params);
    if (encoder == 0)
        throw std::runtime_error("Failed to open X264 encoder.");

    //Write headers
    int nheader;
    int r = x264_encoder_headers(encoder, &nals, &nheader);
    if (r < 0)
        throw std::runtime_error("Failed to create headers!");

    int header_size = nals[0].i_payload + nals[1].i_payload + nals[2].i_payload;
    if (!fwrite(nals[0].p_payload, header_size, 1, file))
        throw std::runtime_error("Failed to write headers to disk!");

    pts = 0;


    return true;
}