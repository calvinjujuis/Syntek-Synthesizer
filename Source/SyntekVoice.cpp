#include "SyntekVoice.h"
#include "SyntekOscillator.h"

//======================
SyntekVoice::SyntekVoice()
{
    for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
    {
        oscList[oscIndex] = std::make_unique<SyntekOscillator>();
        adsrList[oscIndex] = std::make_unique<juce::ADSR>();
    }
}

//======================
SyntekVoice::~SyntekVoice()
{
    for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
    {
        oscList[oscIndex].release();
        adsrList[oscIndex].release();
    }
}

//======================
void SyntekVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
    {
        oscList[oscIndex]->prepareToPlay(spec.sampleRate);
        adsrList[oscIndex]->setSampleRate(spec.sampleRate);
    }
//    oscillator->prepareToPlay(spec.sampleRate);
//    env->setSampleRate(spec.sampleRate);
}

//======================
void SyntekVoice::noteStarted()
{
    auto freqHz = (float)getCurrentlyPlayingNote().getFrequencyInHertz();
    
//    oscillator->setFrequency(freqHz);
//
//    env->noteOn();
    
    for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
    {
        oscList[oscIndex]->setFrequency(freqHz);
        adsrList[oscIndex]->noteOn();
    }
}

//======================
void SyntekVoice::notePitchbendChanged()
{
    
}

//======================
void SyntekVoice::noteStopped(bool allowTailOff)
{
//    env->noteOff();
    for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
    {
        adsrList[oscIndex]->noteOff();
    }
}

//======================
void SyntekVoice::notePressureChanged()
{
    
}

//======================
void SyntekVoice::noteTimbreChanged()
{
    
}

//======================
void SyntekVoice::noteKeyStateChanged()
{
    
}

//======================
void SyntekVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    bool anyActive = false;
    for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
    {
        if (adsrList[oscIndex]->isActive())
        {
            anyActive = true;
            break;
        }
    }
    if (!anyActive) return;
    
    auto buffWrite =  outputBuffer.getArrayOfWritePointers();
    int endSample = startSample + numSamples;
    
    for(int sampleIndex = startSample; sampleIndex < endSample; ++sampleIndex)
    {
        float sample = 0.0f;
        for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
        {
            float oscSample = 0.0f;
            oscSample += oscList[oscIndex]->getNextSample();
            oscSample *= adsrList[oscIndex]->getNextSample();
            oscSample *= gainList[oscIndex];
            
            
//            auto totalNumOutputChannels = getTotalNumOutputChannels();
//            
//            for (int channel = 0; channel < totalNumOutputChannels; ++channel)
//            {
//                auto* channelData = buffer.getWritePointer(channel);
//
//                for (int i = 0; i < buffer.getNumSamples(); ++i)
//                {
//                    float pan = *apvts.getRawParameterValue("OSC1PAN"); // Example for oscillator 1
//                    float leftGain = (pan <= 0.0f) ? 1.0f : (1.0f - pan);
//                    float rightGain = (pan >= 0.0f) ? 1.0f : (1.0f + pan);
//
//                    if (channel == 0) // Left channel
//                        channelData[i] *= leftGain;
//                    else if (channel == 1) // Right channel
//                        channelData[i] *= rightGain;
//                }
//            }
            sample += oscSample;
        }
        sample /= SyntekConstants::NUM_OSCILLATOR;
        
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            buffWrite[channel][sampleIndex] += sample;
        }
    }
    
    
//    if( ! env->isActive() )
//        return;
//
//    auto buffWrite =  outputBuffer.getArrayOfWritePointers();
//    int endSample = startSample + numSamples;
//
//    for(int sampleIndex = startSample; sampleIndex < endSample; ++sampleIndex)
//    {
//        float oscSample = oscillator->getNextSample();
//        oscSample *= 0.5f;
//        oscSample *= env->getNextSample();
//
//        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
//        {
//            buffWrite[channel][sampleIndex] += oscSample;
//        }
//    }
}

//=========================
void SyntekVoice::updateAPVTS(juce::AudioProcessorValueTreeState &apvts)
{
    
    
//    auto harmonic1 = apvts.getRawParameterValue("OSC1H1")->load();
//    auto harmonic2 = apvts.getRawParameterValue("OSC1H2")->load();
//    auto harmonic3 = apvts.getRawParameterValue("OSC1H3")->load();
//    auto harmonic4 = apvts.getRawParameterValue("OSC1H4")->load();
//    auto harmonic5 = apvts.getRawParameterValue("OSC1H5")->load();
//    auto harmonic6 = apvts.getRawParameterValue("OSC1H6")->load();
//    auto harmonic7 = apvts.getRawParameterValue("OSC1H7")->load();
//    auto harmonic8 = apvts.getRawParameterValue("OSC1H8")->load();
//
    for (int oscIndex = 0; oscIndex < SyntekConstants::NUM_OSCILLATOR; ++oscIndex)
    {
        juce::String oscName = "OSC" + juce::String(oscIndex + 1);
        
        // Harmonics
        std::vector<float> harmonics;

        for (int harmonicIndex = 1; harmonicIndex <= SyntekConstants::NUM_HARMONICS; ++harmonicIndex) {
            juce::String paramName = oscName + "H" + juce::String(harmonicIndex);
            auto harmonicValue = apvts.getRawParameterValue(paramName)->load();
            harmonics.push_back(harmonicValue);
        }

        oscList[oscIndex]->setHarmonics(harmonics);
        
        // ADSR
        auto attack = apvts.getRawParameterValue(oscName + "ATTACK")->load();
        auto decay = apvts.getRawParameterValue(oscName + "DECAY")->load();
        auto sustain = apvts.getRawParameterValue(oscName + "SUSTAIN")->load();
        auto release = apvts.getRawParameterValue(oscName + "RELEASE")->load();
        
        juce::ADSR::Parameters adsrParam { attack, decay, sustain, release };
        adsrList[oscIndex]->setParameters(adsrParam);
        
        // Gain
        auto gain = apvts.getRawParameterValue(oscName + "GAIN")->load();
        gainList[oscIndex] = gain;
    }
    
//    std::vector harmonics = { harmonic1, harmonic2, harmonic3, harmonic4, harmonic5, harmonic6, harmonic7, harmonic8};
//    oscillator->setHarmonics(harmonics);
////
//    juce::ADSR::Parameters newParameters { atk1, dec1, sus1, rel1 };
//    env->setParameters(newParameters);
}


