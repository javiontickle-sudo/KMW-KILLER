#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <algorithm>

static juce::AudioProcessorValueTreeState::ParameterLayout layout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>("retune","Retune Speed",1.f,200.f,20.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("humanize","Humanize",0.f,100.f,15.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("formant","Formant",-6.f,6.f,0.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("mix","Mix",0.f,100.f,100.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("amount","Correction Amount",0.f,100.f,100.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("gate","Vocal Gate",-60.f,-20.f,-48.f));
    p.push_back(std::make_unique<juce::AudioParameterBool>("bypass","Bypass",false));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("key","Key",
        juce::StringArray{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},0));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("scale","Scale",
        juce::StringArray{"Chromatic","Major","Minor"},1));
    return {p.begin(),p.end()};
}

KmwTunesAudioProcessor::KmwTunesAudioProcessor()
: AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true)
                                  .withOutput("Output",juce::AudioChannelSet::stereo(),true)),
  parameters(*this,nullptr,"PARAMS",layout()) {}

void KmwTunesAudioProcessor::prepareToPlay(double s,int block) {
    sr=s; hist.assign(8192,0); hw=counter=0; currentShift=0; heldTarget=-1; lastMidi=-1; stableFrames=0;
    stretch.presetDefault(getTotalNumOutputChannels(),sr);
    stretch.setFormantBase(0);
    wet.setSize(getTotalNumOutputChannels(),juce::jmax(block,8192));
    dry.setSize(getTotalNumOutputChannels(),juce::jmax(block,8192));
    setLatencySamples(stretch.inputLatency()+stretch.outputLatency());
}

bool KmwTunesAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
    return l.getMainInputChannelSet()==l.getMainOutputChannelSet() && !l.getMainInputChannelSet().isDisabled();
}

// Normalized autocorrelation with parabolic lag interpolation.
// Designed for one lead vocal, roughly 65-1000 Hz.
KmwTunesAudioProcessor::PitchResult KmwTunesAudioProcessor::detectPitch() {
    const int N=(int)hist.size(), W=2048;
    const int minLag=juce::jmax(2,(int)(sr/1000.0));
    const int maxLag=juce::jmin(W-2,(int)(sr/65.0));
    std::vector<float> scores(maxLag+2,0);
    float best=0; int bestLag=0;

    double rms=0;
    for(int i=0;i<W;++i){ int a=(hw-1-i+N)%N; rms += hist[a]*hist[a]; }
    rms=std::sqrt(rms/W);
    float db=juce::Decibels::gainToDecibels((float)rms,-100.f);
    if(db < *parameters.getRawParameterValue("gate")) return {};

    for(int lag=minLag;lag<=maxLag;++lag){
        double c=0,e1=0,e2=0;
        for(int i=0;i<W;++i){
            int a=(hw-1-i+N)%N, b=(a-lag+N)%N;
            float x=hist[a],y=hist[b]; c+=x*y;e1+=x*x;e2+=y*y;
        }
        float v=(float)(c/(std::sqrt(e1*e2)+1e-12));
        scores[lag]=v;
        if(v>best){best=v;bestLag=lag;}
    }
    if(best<0.62f || bestLag<=minLag || bestLag>=maxLag) return {0,best};

    float y1=scores[bestLag-1],y2=scores[bestLag],y3=scores[bestLag+1];
    float den=(y1-2*y2+y3);
    float delta=std::abs(den)>1e-6f ? 0.5f*(y1-y3)/den : 0.f;
    delta=juce::jlimit(-0.5f,0.5f,delta);
    return {(float)(sr/(bestLag+delta)),best};
}

float KmwTunesAudioProcessor::snap(float m,int key,int scale) const {
    static const int maj[]={0,2,4,5,7,9,11}, min[]={0,2,3,5,7,8,10};
    if(scale==0) return std::round(m);
    const int* ns=scale==1?maj:min;
    float best=m,bd=1e9f;
    int octave=(int)std::floor(m/12.f);
    for(int o=octave-2;o<=octave+2;++o) for(int i=0;i<7;++i){
        float n=12.f*o+key+ns[i], d=std::abs(n-m);
        if(d<bd){bd=d;best=n;}
    }
    return best;
}

void KmwTunesAudioProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) {
    juce::ScopedNoDenormals nd;
    const int n=b.getNumSamples(), chs=b.getNumChannels();
    dry.makeCopyOf(b,true);

    // Always feed analysis so bypass can be switched cleanly.
    for(int i=0;i<n;++i){
        float mono=0;
        for(int c=0;c<chs;++c) mono+=b.getSample(c,i);
        mono/=juce::jmax(1,chs);
        hist[hw]=mono; hw=(hw+1)%(int)hist.size();

        if(++counter>=128){
            counter=0;
            auto pr=detectPitch(); confidence.store(pr.confidence);
            if(pr.hz>0){
                float md=69.f+12.f*std::log2(pr.hz/440.f);
                // reject implausible octave jumps unless they persist
                if(lastMidi<0 || std::abs(md-lastMidi)<7.f){ stableFrames++; lastMidi=md; }
                else { stableFrames=0; lastMidi=md; }

                detectedMidi.store(md);
                float t=snap(md,(int)*parameters.getRawParameterValue("key"),
                                (int)*parameters.getRawParameterValue("scale"));

                // Hysteresis prevents target-note chatter near boundaries.
                if(heldTarget<0 || std::abs(t-heldTarget)>=1.f || stableFrames>3) heldTarget=t;
                targetMidi.store(heldTarget);

                float amount=*parameters.getRawParameterValue("amount")/100.f;
                float desired=(heldTarget-md)*amount;
                float ret=*parameters.getRawParameterValue("retune");
                float human=*parameters.getRawParameterValue("humanize")/100.f;

                // Faster at low retune values; humanize relaxes sustained correction.
                float base=juce::jlimit(0.015f,0.95f,2.5f/(ret+1.f));
                float sustainRelax=stableFrames>12 ? (1.f-0.75f*human) : 1.f;
                float alpha=base*sustainRelax;
                currentShift += (desired-currentShift)*alpha;
                currentShift=juce::jlimit(-12.f,12.f,currentShift);
            } else {
                // Unvoiced/breath region: glide correction toward zero.
                stableFrames=0;
                currentShift += (0.f-currentShift)*0.08f;
            }
        }
    }

    if(*parameters.getRawParameterValue("bypass")>.5f){ b.makeCopyOf(dry,true); return; }

    float form=*parameters.getRawParameterValue("formant");
    stretch.setTransposeSemitones(currentShift,8000.f/(float)sr);
    stretch.setFormantSemitones(form,true);
    stretch.setFormantBase(0);

    wet.setSize(chs,n,false,false,true);
    std::vector<float*> in(chs),out(chs);
    for(int c=0;c<chs;++c){in[c]=b.getWritePointer(c);out[c]=wet.getWritePointer(c);}
    stretch.process(in.data(),n,out.data(),n);

    float mx=*parameters.getRawParameterValue("mix")/100.f;
    for(int c=0;c<chs;++c) for(int i=0;i<n;++i)
        b.setSample(c,i,dry.getSample(c,i)*(1.f-mx)+wet.getSample(c,i)*mx);
}

void KmwTunesAudioProcessor::getStateInformation(juce::MemoryBlock& d){
    if(auto x=parameters.copyState().createXml()) copyXmlToBinary(*x,d);
}
void KmwTunesAudioProcessor::setStateInformation(const void*d,int n){
    if(auto x=getXmlFromBinary(d,n)) parameters.replaceState(juce::ValueTree::fromXml(*x));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new KmwTunesAudioProcessor();}
