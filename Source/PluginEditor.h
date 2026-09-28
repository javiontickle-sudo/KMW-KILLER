#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class KmwTunesAudioProcessorEditor:public juce::AudioProcessorEditor,private juce::Timer{
public:
 explicit KmwTunesAudioProcessorEditor(KmwTunesAudioProcessor&);
 void paint(juce::Graphics&)override; void resized()override;
private:
 KmwTunesAudioProcessor&p;
 juce::Slider r,h,f,m,amount,gate;
 juce::ComboBox key,scale; juce::ToggleButton bypass; juce::Label live,target,conf;
 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>ra,ha,fa,ma,aa,ga;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>ka,sa;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>ba;
 void timerCallback()override; void knob(juce::Slider&);
};