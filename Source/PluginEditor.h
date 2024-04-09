/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SyntekConstants.h"

//==============================================================================
/**
*/
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel() {
        // Assuming knob.png is added to the BinaryData namespace via Projucer
        knobImage = juce::ImageCache::getFromMemory(BinaryData::knob_image_png, BinaryData::knob_image_pngSize);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float startAngle, const float endAngle, juce::Slider& slider) override {


        const float valueRatio = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
        const float angle = startAngle * 1.8f + valueRatio * (endAngle - startAngle * 1.2f);

        const int imgWidth = knobImage.getWidth();
        const int imgHeight = knobImage.getHeight();
        const float scaleFactor = juce::jmin(width / (float)imgWidth, height / (float)imgHeight) * 0.8f;
        
        juce::Rectangle<float> destRect((float)x, (float)y, (float)width, (float)height);
        g.drawImageTransformed(knobImage,
                               juce::AffineTransform::rotation(angle, imgWidth / 2.0f, imgHeight / 2.0f)
                               .scaled(scaleFactor, scaleFactor, imgWidth / 2.0f, imgHeight / 2.0f)
                               .translated(destRect.getCentreX() - imgWidth / 2.0f, destRect.getCentreY() - imgHeight / 2.0f),
                               false);
        
    }
    
    void drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height,
                                   float sliderPos, float minSliderPos, float maxSliderPos,
                                   const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        const int thumbWidth = knobImage.getWidth();
        const int thumbHeight = knobImage.getHeight();
        const float thumbX = sliderPos - (thumbWidth * 0.5f);
        const float thumbY = (height - thumbHeight) * 0.5f;

        g.drawImage(knobImage, thumbX, y + thumbY, thumbWidth, thumbHeight, 0, 0, thumbWidth, thumbHeight);
    }
    
private:
    juce::Image knobImage;
};

class SyntekAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    SyntekAudioProcessorEditor (SyntekAudioProcessor&);
    ~SyntekAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override
    {
        for (auto& harmonicSlider : harmonicSliders) {
            if (slider == &harmonicSlider) {
                repaint();
                break;
            }
        }
    }

private:

    SyntekAudioProcessor& audioProcessor;
    
    juce::Image backgroundImage;
    
    const static int harmonicsArraySize = SyntekConstants::NUM_OSCILLATOR * SyntekConstants::NUM_HARMONICS;
    std::array<juce::Slider, harmonicsArraySize> harmonicSliders;
    std::array<juce::Label, harmonicsArraySize> harmonicLabels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, harmonicsArraySize> harmonicsAttachments;
    
    const static int adsrArraySize = SyntekConstants::NUM_OSCILLATOR * 4;
    std::array<juce::Slider, adsrArraySize> adsrSliders;
    std::array<juce::Label, adsrArraySize> adsrLabels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, adsrArraySize> adsrAttachments;
    
    
    const static int gainArraySize = SyntekConstants::NUM_OSCILLATOR;
    std::array<juce::Slider, gainArraySize> gainSliders;
    std::array<juce::Label, gainArraySize> gainLabels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, gainArraySize> gainAttachments;
    
    const static int panningArraySize = SyntekConstants::NUM_OSCILLATOR;
    std::array<juce::Slider, panningArraySize> panningSliders;
    std::array<juce::Label, panningArraySize> panningLabels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, panningArraySize> panningAttachments;
    
    
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    // Reverb Controls
        juce::Slider reverbSizeSlider;
        juce::Label reverbSizeLabel;
        std::unique_ptr<SliderAttachment> reverbSizeAttachment;

        juce::Slider reverbDecaySlider;
        juce::Label reverbDecayLabel;
        std::unique_ptr<SliderAttachment> reverbDecayAttachment;

        juce::Slider reverbDryWetSlider;
        juce::Label reverbDryWetLabel;
        std::unique_ptr<SliderAttachment> reverbDryWetAttachment;

        juce::Slider reverbStereoSlider;
        juce::Label reverbStereoLabel;
        std::unique_ptr<SliderAttachment> reverbStereoAttachment;
    
 
    // Filters
    juce::Slider highPassFreqSlider;
    juce::Slider lowPassFreqSlider;
    
    juce::Label highPassFreqLabel;
    juce::Label lowPassFreqLabel;
    
    std::unique_ptr<SliderAttachment> highPassFreqSliderAttachment;
    std::unique_ptr<SliderAttachment> lowPassFreqSliderAttachment;
    
    CustomLookAndFeel customLookAndFeel;
    
    void paintWaveform(juce::Graphics& g, int oscillatorIndex, const juce::Rectangle<float>& area);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SyntekAudioProcessorEditor)
};
