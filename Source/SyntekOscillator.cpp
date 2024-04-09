#include "SyntekOscillator.h"
#include "SyntekWavetable.h"

//===============================
SyntekOscillator::SyntekOscillator()
{
    wavetable = std::make_unique<SyntekWavetable>();
}

//===============================
SyntekOscillator::~SyntekOscillator()
{
    wavetable.release();
}

//===============================
void SyntekOscillator::prepareToPlay(double sampleRate)
{
    mSampleRate = sampleRate;
    jassert(mSampleRate > 0);
}

//==============================
void SyntekOscillator::setFrequency(float freq)
{
    currentFreq = freq;
    currentPhaseIncrement = _calculatePhaseIncrement(currentFreq);
}

void SyntekOscillator::setHarmonics(const std::vector<float> &harmonics)
{
    wavetable->setHarmonics(harmonics);
}

//==============================
float SyntekOscillator::_calculatePhaseIncrement(float freq)
{
    int tableSize = wavetable->getTableSize();
    float tableSizeOverSampleRate =  tableSize / mSampleRate;
    float phaseIncrement = freq * tableSizeOverSampleRate;
    return phaseIncrement;
}

//==============================
float SyntekOscillator::getNextSample()
{
    float index = _getNextSampleIndex();
    float sample = wavetable->getSampleAtIndex(index);
    return sample;
}

//==============================
float SyntekOscillator::_getNextSampleIndex()
{
    float nextSampleIndex = currentIndex;
    
    currentIndex += currentPhaseIncrement;
    
    // wrap around table size
    if(currentIndex >= wavetable->getTableSize())
        currentIndex -= (float)wavetable->getLastIndex();
    
    return nextSampleIndex;

}
