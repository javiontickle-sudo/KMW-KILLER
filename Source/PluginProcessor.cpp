#include "PluginProcessor.h"
#include "PluginEditor.h"
static juce::AudioProcessorValueTreeState::ParameterLayout layout(){
 std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
 p.push_back(std::make_unique<juce::AudioParameterFloat>("retune","Retune",0.f,200.f,20.f));
 p.push_back(std::make_unique<juce::AudioParameterFloat>("humanize","Humanize",0.f,100.f,10.f));
 p.push_back(std::make_unique<juce::AudioParameterFloat>("formant","Formant",-6.f,6.f,0.f));
 p.push_back(std::make_unique<juce::AudioParameterFloat>("mix","Mix",0.f,100.f,100.f));
 p.push_back(std::make_unique<juce::AudioParameterBool>("bypass","Bypass",false));
 p.push_back(std::make_unique<juce::AudioParameterChoice>("key","Key",juce::StringArray{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},0));
 p.push_back(std::make_unique<juce::AudioParameterChoice>("scale","Scale",juce::StringArray{"Chromatic","Major","Minor"},1));
 return {p.begin(),p.end()};
}
VvsTunesAudioProcessor::VvsTunesAudioProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),parameters(*this,nullptr,"PARAMS",layout()){}
bool VvsTunesAudioProcessor::isBusesLayoutSupported(const BusesLayout& l)const{return l.getMainInputChannelSet()==l.getMainOutputChannelSet();}
void VvsTunesAudioProcessor::processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&){}
void VvsTunesAudioProcessor::getStateInformation(juce::MemoryBlock& d){if(auto x=parameters.copyState().createXml())copyXmlToBinary(*x,d);}
void VvsTunesAudioProcessor::setStateInformation(const void* d,int n){if(auto x=getXmlFromBinary(d,n))parameters.replaceState(juce::ValueTree::fromXml(*x));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new VvsTunesAudioProcessor();}
