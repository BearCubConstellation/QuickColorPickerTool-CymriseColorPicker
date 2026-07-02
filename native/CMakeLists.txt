cmake_minimum_required(VERSION 3.21)
project(ColorPicker VERSION 1.3.0 LANGUAGES CXX RC)

function(add_color_picker target output_name)
  add_executable(${target} WIN32
    src/main_native.cpp
    resources/app.rc
  )

  target_compile_features(${target} PRIVATE cxx_std_20)
  target_compile_definitions(${target} PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0A00 ${ARGN})
  target_link_libraries(${target} PRIVATE Dwmapi Gdi32 User32 Shell32)
  set_target_properties(${target} PROPERTIES OUTPUT_NAME ${output_name})

  if(MSVC)
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
  endif()
endfunction()

add_color_picker(ColorPicker "ColorPicker")
add_color_picker(ColorPickerEn "ColorPicker-en" COLORPICKER_LANG_EN)
