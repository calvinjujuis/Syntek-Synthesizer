/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SyntekAudioProcessorEditor::SyntekAudioProcessorEditor (SyntekAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    for (int i = 0; i < SyntekConstants::NUM_OSCILLATOR; ++i)
    {
        juce::String oscName = "OSC" + juce::String(i+1);
        
        
        // Harmonics
        for (int j = 0; j < SyntekConstants::NUM_HARMONICS; ++j)
        {
            int harmonicsIndex = SyntekConstants::NUM_HARMONICS * i + j;
            auto& slider = harmonicSliders[harmonicsIndex];
            auto& label = harmonicLabels[harmonicsIndex];
            
            addAndMakeVisible(slider);
            slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
            slider.setRange(0.0f, 1.0f, 0.05f);
            slider.setLookAndFeel(&customLookAndFeel);
            
            addAndMakeVisible(label);
            label.setText("H" + juce::String(j+1), juce::dontSendNotification);
            label.attachToComponent(&slider, true);
            label.setLookAndFeel(&customLookAndFeel);
            
            harmonicsAttachments[harmonicsIndex].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, oscName + "H" + juce::String(j+1), slider));
        }
        
        
        // add listeners for drawing waveform in real-time
        for (auto& slider : harmonicSliders) {
            slider.addListener(this);
        }
        
        // ADSR
        int adsrIndex = i * 4;
        auto& attackSlider = adsrSliders[adsrIndex];
        addAndMakeVisible(attackSlider);
        attackSlider.setRange(0.0f, 5.0f, 0.01f);
        attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        attackSlider.setLookAndFeel(&customLookAndFeel);
        attackSlider.repaint();
        auto& attackLabel = adsrLabels[adsrIndex];
        attackLabel.setText("Attack", juce::dontSendNotification);
        attackLabel.attachToComponent (&attackSlider, false);
        adsrAttachments[adsrIndex].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, oscName + "ATTACK", attackSlider));

        auto& decaySlider = adsrSliders[adsrIndex+1];
        decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        decaySlider.setRange(0.0f, 5.0f, 0.01f);
        decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        decaySlider.setLookAndFeel(&customLookAndFeel);
        auto& decayLabel = adsrLabels[adsrIndex+1];
        decayLabel.setText("Decay", juce::dontSendNotification);
        decayLabel.attachToComponent (&decaySlider, false);
        adsrAttachments[adsrIndex+1].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, oscName + "DECAY", decaySlider));

        auto& sustainSlider = adsrSliders[adsrIndex+2];
        sustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        sustainSlider.setRange(0.0f, 1.0f, 0.01f);
        sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        sustainSlider.setLookAndFeel(&customLookAndFeel);
        auto& sustainLabel = adsrLabels[adsrIndex+2];
        sustainLabel.setText("Sustain", juce::dontSendNotification);
        sustainLabel.attachToComponent (&sustainSlider, false);
        adsrAttachments[adsrIndex+2].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, oscName + "SUSTAIN", sustainSlider));

        auto& releaseSlider = adsrSliders[adsrIndex+3];
        releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        releaseSlider.setRange(0.0f, 5.0f, 0.01f);
        releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        releaseSlider.setLookAndFeel(&customLookAndFeel);
        auto& releaseLabel = adsrLabels[adsrIndex+3];
        releaseLabel.setText("Release", juce::dontSendNotification);
        releaseLabel.attachToComponent (&releaseSlider, false);
        adsrAttachments[adsrIndex+3].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, oscName + "RELEASE", releaseSlider));

//        addAndMakeVisible(attackSlider);
        addAndMakeVisible(decaySlider);
        addAndMakeVisible(sustainSlider);
        addAndMakeVisible(releaseSlider);

        addAndMakeVisible(attackLabel);
        addAndMakeVisible(decayLabel);
        addAndMakeVisible(sustainLabel);
        addAndMakeVisible(releaseLabel);
        
        // Gain
        auto& gainSlider = gainSliders[i];
        addAndMakeVisible(gainSlider);
        gainSlider.setRange(0.0f, 5.0f, 0.01f);
        gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        gainSlider.setLookAndFeel(&customLookAndFeel);
        gainSlider.repaint();
        auto& gainLabel = gainLabels[i];
        addAndMakeVisible(gainLabel);
        gainLabel.setText("Gain", juce::dontSendNotification);
        gainLabel.attachToComponent (&gainSlider, false);
        gainAttachments[i].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, oscName + "GAIN", gainSlider));
        
        // Panning
        auto& panningSlider = panningSliders[i];
        addAndMakeVisible(panningSlider);
        panningSlider.setRange(0.0f, 1.0f, 0.01f);
        panningSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        panningSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        panningSlider.setLookAndFeel(&customLookAndFeel);
        panningSlider.repaint();
        auto& panningLabel = panningLabels[i];
        addAndMakeVisible(panningLabel);
        panningLabel.setText("Panning", juce::dontSendNotification);
        panningLabel.attachToComponent (&panningSlider, false);
//        panningAttachments[i].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, oscName + "Panning", gainSlider));
    }
    
    // Reverb Size
        reverbSizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        reverbSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        reverbSizeSlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(reverbSizeSlider);
        reverbSizeLabel.setText("Size", juce::dontSendNotification);
        reverbSizeLabel.attachToComponent(&reverbSizeSlider, false);
        reverbSizeAttachment.reset(new SliderAttachment(audioProcessor.apvts, "REVERBSIZE", reverbSizeSlider));

        // Reverb Decay
        reverbDecaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        reverbDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        reverbDecaySlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(reverbDecaySlider);
        reverbDecayLabel.setText("Decay", juce::dontSendNotification);
        reverbDecayLabel.attachToComponent(&reverbDecaySlider, false);
        reverbDecayAttachment.reset(new SliderAttachment(audioProcessor.apvts, "REVERBDECAY", reverbDecaySlider));

        // Reverb Dry/Wet
        reverbDryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        reverbDryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        reverbDryWetSlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(reverbDryWetSlider);
        reverbDryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
        reverbDryWetLabel.attachToComponent(&reverbDryWetSlider, false);
        reverbDryWetAttachment.reset(new SliderAttachment(audioProcessor.apvts, "REVERBDRYWET", reverbDryWetSlider));

        // Reverb Stereo
        reverbStereoSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        reverbStereoSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        reverbStereoSlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(reverbStereoSlider);
        reverbStereoLabel.setText("Stereo", juce::dontSendNotification);
        reverbStereoLabel.attachToComponent(&reverbStereoSlider, false);
        reverbStereoAttachment.reset(new SliderAttachment(audioProcessor.apvts, "REVERBSTEREO", reverbStereoSlider));


    // Filters
    highPassFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highPassFreqSlider.setLookAndFeel(&customLookAndFeel);
    highPassFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    highPassFreqSliderAttachment.reset(new SliderAttachment(audioProcessor.apvts, "HPFREQ", highPassFreqSlider));
    addAndMakeVisible(highPassFreqSlider);
    
    highPassFreqLabel.setText("HPF", juce::dontSendNotification);
    highPassFreqLabel.attachToComponent(&highPassFreqSlider, false);

    lowPassFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowPassFreqSlider.setLookAndFeel(&customLookAndFeel);
    lowPassFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    lowPassFreqSliderAttachment.reset(new SliderAttachment(audioProcessor.apvts, "LPFREQ", lowPassFreqSlider));
    addAndMakeVisible(lowPassFreqSlider);
    
    lowPassFreqLabel.setText("LPF", juce::dontSendNotification);
    lowPassFreqLabel.attachToComponent(&lowPassFreqSlider, false);
    

    
    setSize (1000, 800);
    
}

SyntekAudioProcessorEditor::~SyntekAudioProcessorEditor()
{
}

void SyntekAudioProcessorEditor::paintWaveform(juce::Graphics& g, int oscillatorIndex, const juce::Rectangle<float>& area)
{
    g.setColour(juce::Colours::black);
    g.drawRect(area);
    g.fillRect(area);
    
    juce::Path waveformPath;
        waveformPath.startNewSubPath(area.getX(), area.getCentreY());

        float width = area.getWidth();
        float height = area.getHeight();
        
    float maxPossibleAmplitude = 0.0f;
        for (int harmonic = 1; harmonic <= SyntekConstants::NUM_HARMONICS; ++harmonic) {
            maxPossibleAmplitude += 1.0f / harmonic;
        }
        float amplitudeScaleFactor = (height / 2.0f) / maxPossibleAmplitude;

        for (int x = 0; x < int(width); ++x) {
            float normalizedX = static_cast<float>(x) / width;
            float waveY = 0.0f;

            for (int harmonic = 1; harmonic <= SyntekConstants::NUM_HARMONICS; ++harmonic) {
                float amplitude = *audioProcessor.apvts.getRawParameterValue("OSC" + juce::String(oscillatorIndex + 1) + "H" + juce::String(harmonic));
                waveY += (amplitude / harmonic) * std::sin(2.0f * juce::MathConstants<float>::pi * harmonic * normalizedX);
            }

            waveY *= amplitudeScaleFactor;
            float centerY = area.getCentreY();
            waveformPath.lineTo(area.getX() + x, centerY - waveY);
        }

        g.setColour(juce::Colours::white);
        g.strokePath(waveformPath, juce::PathStrokeType(2.0f));

}

//==============================================================================
void SyntekAudioProcessorEditor::paint (juce::Graphics& g)
{
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::silver_background_jpeg, BinaryData::silver_background_jpegSize);
    g.drawImage (backgroundImage, getLocalBounds().toFloat());

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    const int waveformWidth = 100;
    const int waveformHeight = 120;
    const int startX = 10;
    const int startY = 60;
    const int padding = 100;

    for (int osc = 0; osc < SyntekConstants::NUM_OSCILLATOR; ++osc) {
        juce::Rectangle<float> waveformArea(startX, startY + osc * (waveformHeight + padding), waveformWidth, waveformHeight);
        paintWaveform(g, osc, waveformArea);
    }
}

void SyntekAudioProcessorEditor::resized()
{
    // Harmonics
    int sliderWidth = 200;
    int sliderHeight = 12;
    int sliderStartX = 150;
    int sliderStartY = 30;
    int sliderSpacingX = 20;
    int sliderSpacingY = 12;
    
    for (int i = 0; i < SyntekConstants::NUM_OSCILLATOR; ++i) {
        for (int j = 0; j < 8; ++j) {
            int harmonicsIndex = i * 16 + j;
            auto& slider = harmonicSliders[harmonicsIndex];
            
            slider.setBounds(sliderStartX, sliderStartY + j * (sliderHeight + sliderSpacingY) + i * 220, sliderWidth, sliderHeight);
        }
    }
    
    int secondColumnStartX = sliderStartX + sliderWidth + sliderSpacingX + 10;
    for (int i = 0; i < SyntekConstants::NUM_OSCILLATOR; ++i) {
        for (int j = 8; j < 16; ++j) {
            int harmonicsIndex = i * 16 + j;
            auto& slider = harmonicSliders[harmonicsIndex];
            
            slider.setBounds(secondColumnStartX, sliderStartY + (j - 8) * (sliderHeight + sliderSpacingY) + i * 220, sliderWidth, sliderHeight);
        }
    }
    
    // Gain
    int gainKnobStartX = sliderStartX + 470;
    int gainKnobStartY = sliderStartY + 10;
    int gainKnobDiameter = 80;
    
    for (int i = 0; i < SyntekConstants::NUM_OSCILLATOR; ++i) {
        auto& gainSlider = gainSliders[i];
        auto& gainLabel = gainLabels[i];
        
        gainSlider.setBounds(gainKnobStartX, gainKnobStartY + i * 220, gainKnobDiameter, gainKnobDiameter);
        gainLabel.attachToComponent(&gainSlider, false);
    }
    
    // Panning
    
    int panningKnobStartX = gainKnobStartX;
    int panningKnobStartY = gainKnobStartY + 110;
    int panningKnobDiameter = 80;
    
    for (int i = 0; i < SyntekConstants::NUM_OSCILLATOR; ++i) {
        auto& panningSlider = panningSliders[i];
        auto& panningLabel = panningLabels[i];
        
        panningSlider.setBounds(panningKnobStartX, panningKnobStartY + i * 220, panningKnobDiameter, panningKnobDiameter);
//        panningLabel.attachToComponent(&panningSlider, false);
    }
    
    // ADSR
    int adsrKnobStartX = gainKnobStartX + gainKnobDiameter + 50;
    int adsrKnobStartY = sliderStartY + 10;
    
    for (int i = 0; i < SyntekConstants::NUM_OSCILLATOR; ++i) {
        int baseIndex = i * 4;
        for (int j = 0; j < 4; ++j) {
            int row = j / 2;
            int col = j % 2;
            adsrSliders[baseIndex + j].setBounds(adsrKnobStartX + col * (gainKnobDiameter + 30),
                                                 adsrKnobStartY + row * (gainKnobDiameter + 30) + i * 220,
                                                 gainKnobDiameter, gainKnobDiameter);
        }
    }
    
    // Reverb
    int reverbSliderDiameter = 80;
    int reverbControlStartX = sliderStartX + 10;
    int reverbControlStartY = 700;
    int reverbSpacing = 110;

    reverbSizeSlider.setBounds(reverbControlStartX, reverbControlStartY, reverbSliderDiameter, reverbSliderDiameter);
    reverbDecaySlider.setBounds(reverbControlStartX + reverbSpacing, reverbControlStartY, reverbSliderDiameter, reverbSliderDiameter);
    reverbDryWetSlider.setBounds(reverbControlStartX + 2 * reverbSpacing, reverbControlStartY, reverbSliderDiameter, reverbSliderDiameter);
    reverbStereoSlider.setBounds(reverbControlStartX + 3 * reverbSpacing, reverbControlStartY, reverbSliderDiameter, reverbSliderDiameter);
    
    
    // Filters
    int filterSliderDiameter = 80;
    int filterSliderStartX = 750;
    int filterSliderStartY = 700;

    highPassFreqSlider.setBounds(filterSliderStartX, filterSliderStartY, filterSliderDiameter, filterSliderDiameter);
    lowPassFreqSlider.setBounds(filterSliderStartX + filterSliderDiameter + 30, filterSliderStartY, filterSliderDiameter, filterSliderDiameter);

}


