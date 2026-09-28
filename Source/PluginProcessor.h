#pragma once
#include <JuceHeader.h>
#include "signalsmith-stretch.h"
#include <atomic>
#include <vector>
#include <array>

class KmwTunesAudioProcessor : public juce::AudioProcessor {
public:
    KmwTunesAudioProcessor();
    void prepareToPlay(double,int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "KMW TUNES"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int,const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*,int) override;

    juce::AudioProcessorValueTreeState parameters;
    float getDetectedMidi() const { return detectedMidi.load(); }
    float getTargetMidi() const { return targetMidi.load(); }
    float getConfidence() const { return confidence.load(); }

private:
    double sr=44100.0;
    signalsmith::stretch::SignalsmithStretch<float> stretch;
    juce::AudioBuffer<float> wet, dry;
    std::vector<float> hist;
    int hw=0, counter=0;
    float currentShift=0.0f, heldTarget=-1.0f, lastMidi=-1.0f;
    int stableFrames=0;
    std::atomic<float> detectedMidi{-1}, targetMidi{-1}, confidence{0};

    struct PitchResult { float hz=0, confidence=0; };
    PitchResult detectPitch();
    float snap(float,int,int) const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KmwTunesAudioProcessor)
};