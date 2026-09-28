cmake_minimum_required(VERSION 3.22)
project(VVS_TUNES VERSION 0.2.0)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.12)
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(VVS_TUNES COMPANY_NAME "VVS" BUNDLE_ID "com.vvs.vvstunes"
 PLUGIN_MANUFACTURER_CODE Vvst PLUGIN_CODE Vvtn FORMATS VST3 PRODUCT_NAME "VVS TUNES"
 IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE IS_MIDI_EFFECT FALSE)
target_sources(VVS_TUNES PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(VVS_TUNES PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(VVS_TUNES PRIVATE juce::juce_audio_utils juce::juce_dsp)
