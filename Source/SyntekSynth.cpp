#include "SyntekSynth.h"
#include "SyntekVoice.h"


static constexpr size_t maxNumVoices = 5;

//======================
SyntekSynth::SyntekSynth()
{
    _initVoices(maxNumVoices);
}

//======================
SyntekSynth::~SyntekSynth()
{
}

//======================
void SyntekSynth::prepare(juce::dsp::ProcessSpec& spec)
{
    this->setCurrentPlaybackSampleRate(spec.sampleRate);
    
    for(auto* v : voices)
    {
        auto voice = dynamic_cast<SyntekVoice*>(v);
        voice->prepare(spec);
    }
}


//======================
void SyntekSynth::_initVoices(int numVoices)
{
    for(size_t i = 0; i <= maxNumVoices; ++i)
    {
        this->addVoice(new SyntekVoice());
    }
    
    setVoiceStealingEnabled(true);

}

//======================
void SyntekSynth::updateAPVTS(juce::AudioProcessorValueTreeState& apvts)
{
    for(size_t i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SyntekVoice*>(getVoice(i)))
        {
            voice->updateAPVTS(apvts);
        }
    }

}
