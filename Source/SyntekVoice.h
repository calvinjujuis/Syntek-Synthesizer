#pragma once
#include <JuceHeader.h>
#include "SyntekConstants.h"

class SyntekOscillator;

class SyntekVoice : public juce::MPESynthesiserVoice
{
public:
    SyntekVoice();
    ~SyntekVoice();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void noteStarted() override;
    void notePitchbendChanged() override;
    void noteStopped(bool allowTailOff) override;
    void notePressureChanged() override ;
    void noteTimbreChanged() override ;
    void noteKeyStateChanged() override ;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void updateAPVTS(juce::AudioProcessorValueTreeState& apvts);
private:
//    std::unique_ptr<SyntekOscillator> oscillator;
//    std::unique_ptr<SyntekOscillator> osc1;
//    std::unique_ptr<SyntekOscillator> osc2;
    std::array<std::unique_ptr<SyntekOscillator>, SyntekConstants::NUM_OSCILLATOR> oscList;
    std::array<std::unique_ptr<juce::ADSR>, SyntekConstants::NUM_OSCILLATOR> adsrList;
    std::array<float, SyntekConstants::NUM_OSCILLATOR> gainList;
};
