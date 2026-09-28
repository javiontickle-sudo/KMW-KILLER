#include "PluginEditor.h"
VvsTunesAudioProcessorEditor::VvsTunesAudioProcessorEditor(VvsTunesAudioProcessor& x):AudioProcessorEditor(&x),p(x){
 setSize(820,460);
 for(auto* s:{&r,&h,&f,&m}){s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s->setTextBoxStyle(juce::Slider::TextBoxBelow,false,90,22);s->setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(220,20,35));addAndMakeVisible(*s);}
 key.addItemList({"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},1); scale.addItemList({"Chromatic","Major","Minor"},1); addAndMakeVisible(key);addAndMakeVisible(scale);
 bypass.setButtonText("BYPASS");addAndMakeVisible(bypass);
 ra=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"retune",r);ha=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"humanize",h);fa=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"formant",f);ma=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"mix",m);
 ka=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.parameters,"key",key);sa=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.parameters,"scale",scale);ba=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.parameters,"bypass",bypass);
}
void VvsTunesAudioProcessorEditor::paint(juce::Graphics& g){g.fillAll(juce::Colour(8,8,10));g.setColour(juce::Colour(215,20,32));g.drawRoundedRectangle(15,15,790,430,18,4);g.setFont(36);g.drawText("VVS TUNES",40,30,300,50,juce::Justification::left);g.setColour(juce::Colours::white);g.setFont(16);g.drawText("REDLINE VOCAL TUNING",42,75,300,25,juce::Justification::left);g.setColour(juce::Colour(215,20,32));g.fillEllipse(700,35,55,50);g.setColour(juce::Colour(8,8,10));g.fillEllipse(712,48,10,10);g.fillEllipse(733,48,10,10);}
void VvsTunesAudioProcessorEditor::resized(){r.setBounds(25,130,180,170);h.setBounds(215,130,180,170);f.setBounds(405,130,180,170);m.setBounds(595,130,180,170);key.setBounds(65,350,120,34);scale.setBounds(225,350,150,34);bypass.setBounds(430,350,120,34);}
juce::AudioProcessorEditor* VvsTunesAudioProcessor::createEditor(){return new VvsTunesAudioProcessorEditor(*this);}
