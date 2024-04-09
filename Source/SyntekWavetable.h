#pragma once
#include <JuceHeader.h>

class SyntekWavetable
{
public:
    SyntekWavetable();
    ~SyntekWavetable();
    
    
    void prepareToPlay(double sampleRate);
    
    float getSampleAtIndex(float index);
    
    int getTableSize();
    
    int getLastIndex();
    void setHarmonics(std::vector<float> newHarmonics);
    
private:
    juce::AudioBuffer<float> waveBuffer;
    std::vector<float> harmonics = {0.0f, 0.0f, 0.0f, 0.0f};
    
    void _generateWave();

};
