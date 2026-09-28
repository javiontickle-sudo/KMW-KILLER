#include "PluginEditor.h"
#include <cmath>
KmwTunesAudioProcessorEditor::KmwTunesAudioProcessorEditor(KmwTunesAudioProcessor&x):AudioProcessorEditor(&x),p(x){
 setSize(1040,650); for(auto*s:{&r,&h,&f,&m,&amount,&gate})knob(*s);
 key.addItemList({"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},1);
 scale.addItemList({"Chromatic","Major","Minor"},1); addAndMakeVisible(key);addAndMakeVisible(scale);
 bypass.setButtonText("BYPASS");addAndMakeVisible(bypass);
 for(auto*l:{&live,&target,&conf}){l->setJustificationType(juce::Justification::centred);addAndMakeVisible(*l);}
 ra=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"retune",r);
 ha=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"humanize",h);
 fa=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"formant",f);
 ma=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"mix",m);
 aa=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"amount",amount);
 ga=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,"gate",gate);
 ka=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.parameters,"key",key);
 sa=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.parameters,"scale",scale);
 ba=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.parameters,"bypass",bypass); startTimerHz(20);
}
void KmwTunesAudioProcessorEditor::knob(juce::Slider&s){s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,72,19);s.setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(225,10,28));addAndMakeVisible(s);}
void KmwTunesAudioProcessorEditor::paint(juce::Graphics&g){
 g.fillAll(juce::Colour(5,5,7));g.setColour(juce::Colour(30,31,34));g.fillRoundedRectangle(18,18,1004,614,18);g.setColour(juce::Colour(220,10,28));g.drawRoundedRectangle(18,18,1004,614,18,3);
 g.setFont(34);g.drawText("KMW TUNES",40,34,260,46,juce::Justification::left);
 g.fillEllipse(385,82,270,245);g.setColour(juce::Colour(8,8,10));g.fillEllipse(440,150,52,46);g.fillEllipse(548,150,52,46);juce::Path n;n.addTriangle(520,195,500,238,540,238);g.fillPath(n);g.fillRoundedRectangle(460,250,120,48,14);
 g.setColour(juce::Colours::white);g.setFont(12);const char*names[]={"RETUNE","HUMANIZE","FORMANT","MIX","AMOUNT","GATE"};
 for(int i=0;i<6;i++)g.drawText(names[i],25+i*168,515,150,20,juce::Justification::centred);
 g.drawText("LIVE",405,340,90,18,juce::Justification::centred);g.drawText("TARGET",505,340,90,18,juce::Justification::centred);g.drawText("TRACK",605,340,90,18,juce::Justification::centred);
}
void KmwTunesAudioProcessorEditor::resized(){
 juce::Slider* ks[]={&r,&h,&f,&m,&amount,&gate};for(int i=0;i<6;i++)ks[i]->setBounds(25+i*168,435,150,145);
 key.setBounds(55,345,120,32);scale.setBounds(190,345,145,32);bypass.setBounds(870,345,100,32);
 live.setBounds(405,365,90,28);target.setBounds(505,365,90,28);conf.setBounds(605,365,90,28);
}
void KmwTunesAudioProcessorEditor::timerCallback(){
 auto nn=[](float x){if(x<0)return juce::String("--");static const char*n[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};int q=(int)std::round(x);return juce::String(n[(q%12+12)%12])+juce::String(q/12-1);};
 live.setText(nn(p.getDetectedMidi()),juce::dontSendNotification);target.setText(nn(p.getTargetMidi()),juce::dontSendNotification);
 conf.setText(juce::String((int)std::round(p.getConfidence()*100.f))+"%",juce::dontSendNotification);
}
juce::AudioProcessorEditor*KmwTunesAudioProcessor::createEditor(){return new KmwTunesAudioProcessorEditor(*this);}