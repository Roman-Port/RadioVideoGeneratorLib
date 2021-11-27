# CMakeList.txt : CMake project for LibRadioVideoGenerator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("LibRadioVideoGenerator")

# Get required dependencies
add_library(volk SHARED IMPORTED)
set_target_properties(volk PROPERTIES IMPORTED_LOCATION C:/Users/Roman/source/repos/volk/out/install/x64-Debug/bin/volk.dll)
set_target_properties(volk PROPERTIES IMPORTED_IMPLIB C:/Users/Roman/source/repos/volk/out/install/x64-Debug/lib/volk.lib)
set_target_properties(volk PROPERTIES INTERFACE_INCLUDE_DIRECTORIES C:/Users/Roman/source/repos/volk/out/install/x64-Debug/include/)

add_library(fftw3f SHARED IMPORTED)
set_target_properties(fftw3f PROPERTIES IMPORTED_LOCATION C:/Users/Roman/source/lib/libfftw3f-3.dll)
set_target_properties(fftw3f PROPERTIES IMPORTED_IMPLIB C:/Users/Roman/source/lib/libfftw3f-3.lib)
set_target_properties(fftw3f PROPERTIES INTERFACE_INCLUDE_DIRECTORIES C:/Users/Roman/source/lib/include/)

add_library(x264 SHARED IMPORTED)
set_target_properties(x264 PROPERTIES IMPORTED_LOCATION C:/Users/Roman/source/lib/x264/libx264-164.dll)
set_target_properties(x264 PROPERTIES IMPORTED_IMPLIB C:/Users/Roman/source/lib/x264/libx264.lib)
set_target_properties(x264 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES C:/Users/Roman/source/lib/x264/include/)

# Set up options
#set(RAPTORRDS_DEBUG_OUTPUT ON)

# Add built-in libs
add_subdirectory(lib/raptordsp)
add_subdirectory(lib/raptorrds)
add_subdirectory(lib/picodes)

# Make
add_library (LibRadioVideoGenerator SHARED   "include/radiovideo/config/config_channel.h" "include/radiovideo/config/config_view_video.h"  "include/radiovideo/config/config_view_audio_fm.h" "include/radiovideo/config/config_view_video_fft.h" "include/radiovideo/views/view.h" "include/radiovideo/views/view_video.h" "include/radiovideo/pixel.h" "include/radiovideo/views/video/view_fft.h" "src/views/view.cpp" "src/views/view_video.cpp" "src/views/video/view_fft.cpp"   "include/radiovideo/paint.h" "include/radiovideo/views/video/view_spectrum.h" "src/views/video/view_spectrum.cpp" "include/radiovideo/config/config_view_video_spectrum.h" "include/radiovideo/contexts/context_channel.h"  "include/radiovideo/contexts/context_canvas.h" "include/radiovideo/contexts/context_canvas_impl.h" "include/radiovideo/contexts/context_channel_impl.h" "src/contexts/context_channel_impl.cpp" "src/contexts/context_canvas_impl.cpp" "include/radiovideo/graphics/paint_solid.h" "src/graphics/paint_solid.cpp" "include/radiovideo/outputs/output.h" "include/radiovideo/outputs/video/output_video_raw.h" "src/output/video/output_video_raw.cpp" "include/radiovideo/outputs/video/output_video_h264.h" "src/output/video/output_video_h264.cpp" "include/radiovideo/graphics/paint_gradient.h" "src/graphics/paint_gradient.cpp" "include/radiovideo/api.h" "src/api.cpp" "include/radiovideo/outputs/video/output_video_null.h" "src/output/video/output_video_null.cpp" "include/radiovideo/views/audio/view_fm.h" "include/radiovideo/views/view_audio.h" "src/views/view_audio.cpp" "include/radiovideo/outputs/audio/output_audio_wav.h" "src/output/audio/output_audio_wav.cpp" "src/views/audio/view_fm.cpp" "include/radiovideo/config/config_view_audio.h" "include/radiovideo/outputs/audio/output_audio_null.h" "src/output/audio/output_audio_null.cpp" "include/radiovideo/views/video/view_spectrum_waterfall.h" "src/views/video/view_spectrum_waterfall.cpp"  "include/radiovideo/config/config_view_video_spectrum_waterfall.h"  "include/radiovideo/graphics/text/font_renderer.h" "include/radiovideo/graphics/text/font.h" "src/graphics/text/font_renderer.cpp" "include/radiovideo/views/video/view_station_label.h" "include/radiovideo/config/config_view_video_station_label.h" "src/views/video/view_station_label.cpp" "include/radiovideo/views/data/view_markers.h" "include/radiovideo/config/config_view_data_markers.h" "src/views/data/view_markers.cpp" "include/radiovideo/graphics/frame_queue.h" "src/graphics/frame_queue.cpp" "include/radiovideo/misc/error.h" "src/misc/error.cpp" "include/radiovideo/graphics/rectangle.h" "include/radiovideo/views/video/view_rds_strip.h" "include/radiovideo/config/config_view_video_rds_strip.h" "src/views/video/views_rds_strip.cpp" "include/radiovideo/misc/bundle.h" "src/misc/bundle.cpp" "include/radiovideo/graphics/util.h" "src/graphics/util.cpp" "include/radiovideo/misc/writer.h" "src/misc/writer.cpp" "include/radiovideo/views/view_demod.h" "src/views/view_demod.cpp" "include/radiovideo/views/data/view_rds.h" "src/views/data/view_rds.cpp" "include/radiovideo/views/video/view_pi_id.h" "src/views/video/view_pi_id.cpp")
target_include_directories (LibRadioVideoGenerator PUBLIC include)

# Link
target_link_libraries (LibRadioVideoGenerator volk fftw3f raptordsp raptorrds x264 picodes)

# Add test objects

add_subdirectory(test)