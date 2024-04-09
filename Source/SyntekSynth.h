#pragma once
#include <JuceHeader.h>

class SyntekSynth : public juce::MPESynthesiser
{
public:
    SyntekSynth();
    ~SyntekSynth();
    
    void prepare(juce::dsp::ProcessSpec& spec);
    void updateAPVTS(juce::AudioProcessorValueTreeState& apvts);

private:
    void _initVoices(int numVoices);

};
