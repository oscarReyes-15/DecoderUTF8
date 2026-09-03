cmake_minimum_required(VERSION 3.20)

project(DecoderUTF8 LANGUAGES CXX)

add_executable(decoder_utf8
    main.cpp
    Decoder.cpp
)

set_target_properties(decoder_utf8 PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(MSVC)
    target_compile_options(decoder_utf8 PRIVATE /W4)
else()
    target_compile_options(decoder_utf8 PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()