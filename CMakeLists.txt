cmake_minimum_required(VERSION 3.22)

project(VVS_TUNES VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(VVS_TUNES
    COMPANY_NAME "VVS"
    BUNDLE_ID "com.vvs.vvstunes"

    PLUGIN_MANUFACTURER_CODE Vvst
    PLUGIN_CODE Vvtn

    FORMATS VST3
    PRODUCT_NAME "VVS TUNES"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(VVS_TUNES
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Generates JuceHeader.h for our source files
juce_generate_juce_header(VVS_TUNES)

target_compile_definitions(VVS_TUNES
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(VVS_TUNES
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
)

target_link_libraries(VVS_TUNES
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
