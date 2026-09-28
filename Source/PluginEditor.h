#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class VvsTunesAudioProcessorEditor:public juce::AudioProcessorEditor{
public: explicit VvsTunesAudioProcessorEditor(VvsTunesAudioProcessor&); void paint(juce::Graphics&)override; void resized()override;
private:
 VvsTunesAudioProcessor& p; juce::Slider r,h,f,m; juce::ComboBox key,scale; juce::ToggleButton bypass;
 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ra,ha,fa,ma;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ka,sa;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ba;
};