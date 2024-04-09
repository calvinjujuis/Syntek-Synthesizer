#include "SyntekWavetable.h"
#include "SyntekConstants.h"

//==============================
SyntekWavetable::SyntekWavetable()
{
    setHarmonics(harmonics);
    _generateWave();
}

//==============================
SyntekWavetable::~SyntekWavetable()
{
    
}

void SyntekWavetable::setHarmonics(std::vector<float> newHarmonics)
{
    harmonics = newHarmonics;
    _generateWave();
}

//==============================
void SyntekWavetable::_generateWave()
{
    int tableSize = SyntekConstants::TABLE_SIZE;
    waveBuffer.setSize(1, tableSize);
    waveBuffer.clear();
    
    auto* buffWrite = waveBuffer.getWritePointer(0);
    for (auto harmonic = 0; harmonic < SyntekConstants::NUM_HARMONICS; ++harmonic)
    {
        auto angleDelta = juce::MathConstants<double>::twoPi / (double) (tableSize - 1) * (harmonic + 1);
        auto currentAngle = 0.0;
        for (unsigned int i = 0; i < tableSize; ++i)
        {
            float sample = std::sin(currentAngle);
            buffWrite[i] += sample * harmonics[harmonic];
            currentAngle += angleDelta;
        }
    }
    buffWrite[tableSize] = buffWrite[0];
}


//==============================
float SyntekWavetable::getSampleAtIndex(float index)
{
    jassert(index < SyntekConstants::TABLE_SIZE);
    
    int lowIndex  = (int)index;
    int highIndex = lowIndex + 1;
    
    if(highIndex >= SyntekConstants::TABLE_SIZE)
        highIndex = 0;
    
    float spillOver         = index - lowIndex;
    float inverseSpillOver  = 1.f   - spillOver;
    
    float sample1 = waveBuffer.getSample(0, lowIndex) * spillOver;
    float sample2 = waveBuffer.getSample(0, highIndex) * inverseSpillOver;
    
    float sample = (sample1 + sample2) / 2.f;
    
    return sample;
}


//=================================
int SyntekWavetable::getTableSize()
{
    int tableSize = waveBuffer.getNumSamples();
    return tableSize;
}

//=================================
int SyntekWavetable::getLastIndex()
{
    return waveBuffer.getNumSamples() - 1;
}
