/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SyntekSynth;
class SyntekAudioProcessor  :  public juce::AudioProcessor,
                            public juce::ValueTree::Listener
{
public:
    //==============================================================================
    SyntekAudioProcessor();
    ~SyntekAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateAPVTS();
    
    juce::AudioProcessorValueTreeState& getValueTree();
    juce::AudioProcessorValueTreeState apvts;
    
private:
    std::unique_ptr<SyntekSynth> synth;
    
    juce::dsp::Reverb reverb;
    juce::Reverb::Parameters reverbParams;
    
    juce::dsp::ProcessorChain<juce::dsp::StateVariableTPTFilter<float>, juce::dsp::StateVariableTPTFilter<float>> filterChain;

    juce::dsp::StateVariableTPTFilter<float> highPassFilter, lowPassFilter;
    
    juce::Atomic<bool> mustUpdateAPVTS{ false };
    juce::AudioProcessorValueTreeState::ParameterLayout _createParameters();
    
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SyntekAudioProcessor)
};
