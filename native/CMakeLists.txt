cmake_minimum_required(VERSION 3.21)
project(CymriseColorPicker VERSION 1.2.0 LANGUAGES CXX RC)

add_executable(ColorPicker WIN32
  src/main.cpp
  resources/app.rc
)

target_compile_features(ColorPicker PRIVATE cxx_std_20)
target_compile_definitions(ColorPicker PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(ColorPicker PRIVATE Dwmapi Gdi32 User32 Shell32)

if(MSVC)
  set_property(TARGET ColorPicker PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  target_compile_options(ColorPicker PRIVATE /W4 /permissive- /utf-8)
endif()
