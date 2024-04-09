#pragma once
#include <JuceHeader.h>

class SyntekWavetable;

class SyntekOscillator
{
public:

    SyntekOscillator();
    ~SyntekOscillator();
    
    void prepareToPlay(double sampleRate);
    
    void setFrequency(float freq);
    void setHarmonics(const std::vector<float> &harmonics);
    float getNextSample();
    
private:
    std::unique_ptr<SyntekWavetable> wavetable;
    double mSampleRate = -1;
    float currentIndex = 0.f;
    float currentPhaseIncrement = 0.f;
    float currentFreq = 0.f;
    
    float _calculatePhaseIncrement(float freq);
    float _getNextSampleIndex();
};
