set(TARGET "test_bgfx")

juce_add_gui_app(${TARGET}
    PRODUCT_NAME "Test BGFX"
    VERSION "1.0.0"
    COMPANY_NAME "Arthur Benilov"
)

target_link_libraries(${TARGET}
    PRIVATE
        juce::juce_core
        juce::juce_data_structures
        juce::juce_audio_basics
        juce::juce_audio_utils
        juce::juce_audio_plugin_client
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
)

juce_generate_juce_header(${TARGET})

file(GLOB_RECURSE src
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

target_sources(${TARGET} PRIVATE ${src})

file(GLOB_RECURSE res "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*")
juce_add_binary_data(${TARGET}_res SOURCES ${res})
target_link_libraries(${TARGET} PUBLIC ${TARGET}_res)

target_compile_definitions(${TARGET}
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(${TARGET}
    PRIVATE
        bx
        bimg
        bgfx
        nanovg
)
