/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SyntekSynth.h"
#include "SyntekConstants.h"

//==============================================================================
SyntekAudioProcessor::SyntekAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", _createParameters())
#endif
{
    synth = std::make_unique<SyntekSynth>();
    apvts.state.addListener(this);
}

SyntekAudioProcessor::~SyntekAudioProcessor()
{
    apvts.state.removeListener(this);
}

//==============================================================================
const juce::String SyntekAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SyntekAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SyntekAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SyntekAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SyntekAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SyntekAudioProcessor::getNumPrograms()
{
    return 1;
}

int SyntekAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SyntekAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SyntekAudioProcessor::getProgramName (int index)
{
    return {};
}

void SyntekAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SyntekAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 2 };
    synth->prepare(spec);
    updateAPVTS();
    
    filterChain.get<0>().setType(juce::dsp::StateVariableTPTFilterType::highpass);
    filterChain.get<1>().setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    filterChain.get<0>().setCutoffFrequency(20.0f);
    filterChain.get<1>().setCutoffFrequency(20.0f);

    filterChain.prepare({sampleRate, (juce::uint32)samplesPerBlock, static_cast<juce::uint32>(getTotalNumOutputChannels())});

}

void SyntekAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SyntekAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//==================================
void SyntekAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if(mustUpdateAPVTS.get())
        updateAPVTS();
    
    synth->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
//    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer(channel);
//
//        for (int i = 0; i < buffer.getNumSamples(); ++i)
//        {
//            float pan = *apvts.getRawParameterValue("OSC1PAN"); // Example for oscillator 1
//            float leftGain = (pan <= 0.0f) ? 1.0f : (1.0f - pan);
//            float rightGain = (pan >= 0.0f) ? 1.0f : (1.0f + pan);
//
//            if (channel == 0) // Left channel
//                channelData[i] *= leftGain;
//            else if (channel == 1) // Right channel
//                channelData[i] *= rightGain;
//        }
//    }
    
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    // Reverb
    reverbParams.roomSize = *apvts.getRawParameterValue("REVERBSIZE");
    reverbParams.damping = *apvts.getRawParameterValue("REVERBDECAY");
    float reverbDryWet = *apvts.getRawParameterValue("REVERBDRYWET");
    reverbParams.dryLevel = reverbDryWet;
    reverbParams.wetLevel = 1.0f - reverbDryWet;
    reverbParams.width = *apvts.getRawParameterValue("REVERBSTEREO");
    //    reverbParams.freezeMode = 0.0f;
    
    reverb.setParameters (reverbParams);
    reverb.process(context);

    // Filters
    filterChain.get<0>().setCutoffFrequency(*apvts.getRawParameterValue("HPFREQ"));
    filterChain.get<1>().setCutoffFrequency(*apvts.getRawParameterValue("LPFREQ"));
    filterChain.process(context);

}

//==============================================================================
bool SyntekAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SyntekAudioProcessor::createEditor()
{
    return new SyntekAudioProcessorEditor (*this);
}

//==============================================================================
void SyntekAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void SyntekAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SyntekAudioProcessor();
}

//=====================================================
void SyntekAudioProcessor::updateAPVTS()
{
    synth->updateAPVTS(apvts);
    mustUpdateAPVTS = false;
}

//=====================================================
juce::AudioProcessorValueTreeState& SyntekAudioProcessor::getValueTree()
{
    return apvts;
}


//=====================================================
juce::AudioProcessorValueTreeState::ParameterLayout SyntekAudioProcessor::_createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    for (int i = 1; i <= SyntekConstants::NUM_OSCILLATOR; ++i) {
        
        juce::String oscName = "OSC" + juce::String(i);
        
        // Harmonics
        for (int j = 1; j <= SyntekConstants::NUM_HARMONICS; ++j)
        {
            juce::String paramID = oscName + "H" + juce::String(j);
            juce::String paramName = oscName + " Harmonic" + juce::String(j);
            params.push_back(std::make_unique<juce::AudioParameterFloat>(paramID, paramName, juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
        }
        
        // ADSR Params
        params.push_back(std::make_unique<juce::AudioParameterFloat>(oscName + "ATTACK", oscName + "Attack", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 0.00f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(oscName + "DECAY", oscName + "Decay", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01), 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(oscName + "SUSTAIN", oscName + "Sustain", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(oscName + "RELEASE", oscName + "Release", juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 0.0f));
        
        // Gain
        params.push_back(std::make_unique<juce::AudioParameterFloat>(oscName + "GAIN", oscName + " Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
        
        // Panning
//        params.push_back(std::make_unique<juce::AudioParameterFloat>(oscName + "PAN", oscName + " Pan", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    }

    // Reverb
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBSIZE", "Reverb Size", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDECAY", "Reverb DECAY", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDRYWET", "Reverb Dry/Wet", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBSTEREO", "Reverb STEREO", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f, ""));
//    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBFREEZE", "Reverb Freeze", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));

    
    // Filters
    params.push_back(std::make_unique<juce::AudioParameterFloat>("HPFREQ", "High Pass Frequency", juce::NormalisableRange<float> { 20.0f, 20000.0f, 1.0f, 0.25f }, 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LPFREQ", "Low Pass Frequency", juce::NormalisableRange<float> { 20.0f, 20000.0f, 1.0f, 0.25f }, 20000.0f));

    
    return { params.begin(), params.end() };
}


//=====================================
void SyntekAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    mustUpdateAPVTS = true;
}
