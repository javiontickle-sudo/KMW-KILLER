cmake_minimum_required(VERSION 3.22)

project(KMW_TUNES VERSION 3.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# JUCE
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
)
FetchContent_MakeAvailable(JUCE)

# Signalsmith Stretch
FetchContent_Declare(
    signalsmith_stretch
    GIT_REPOSITORY https://github.com/Signalsmith-Audio/signalsmith-stretch.git
    GIT_TAG main
)
FetchContent_MakeAvailable(signalsmith_stretch)

# Signalsmith Linear must exist INSIDE the Stretch source tree because
# signalsmith-stretch.h includes "signalsmith-linear/stft.h".
FetchContent_Declare(
    signalsmith_linear
    GIT_REPOSITORY https://github.com/Signalsmith-Audio/linear.git
    GIT_TAG main
    SOURCE_DIR ${signalsmith_stretch_SOURCE_DIR}/signalsmith-linear
)
FetchContent_MakeAvailable(signalsmith_linear)

juce_add_plugin(KMW_TUNES
    COMPANY_NAME "KMW"
    BUNDLE_ID "com.kmw.kmwtunes"

    PLUGIN_MANUFACTURER_CODE Kmwt
    PLUGIN_CODE Km03

    FORMATS VST3
    PRODUCT_NAME "KMW TUNES"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(KMW_TUNES
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

juce_generate_juce_header(KMW_TUNES)

target_include_directories(KMW_TUNES
    PRIVATE
        ${signalsmith_stretch_SOURCE_DIR}
)

target_compile_definitions(KMW_TUNES
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(KMW_TUNES
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
