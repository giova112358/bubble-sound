/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BubbleModelAudioProcessorEditor  : public juce::AudioProcessorEditor,
    public juce::Button::Listener
{
public:
    BubbleModelAudioProcessorEditor (BubbleModelAudioProcessor&);
    ~BubbleModelAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked(juce::Button* button) override;

private:
    struct MainPanel : public Component
    {
        MainPanel(BubbleModelAudioProcessor& p) : audioProcessor(p)
        {
            volumeSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(volumeSlider.get());
            volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "VOL", *volumeSlider);
            volumeLabel = std::make_unique<juce::Label>("", "Volume");
            addAndMakeVisible(volumeLabel.get());
            volumeLabel->attachToComponent(volumeSlider.get(), false);
            volumeLabel->setJustificationType(juce::Justification::centredTop);

            radiusSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(radiusSlider.get());
            radiusAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "RAD", *radiusSlider);
            radiusLabel = std::make_unique<juce::Label>("", "Radius");
            addAndMakeVisible(radiusLabel.get());
            radiusLabel->attachToComponent(radiusSlider.get(), false);
            radiusLabel->setJustificationType(juce::Justification::centredTop);

            riseFactorSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
            addAndMakeVisible(riseFactorSlider.get());
            riseFactorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "RISE", *riseFactorSlider);
            riseFactorLabel = std::make_unique<juce::Label>("", "Rise Factor");
            addAndMakeVisible(riseFactorLabel.get());
            riseFactorLabel->attachToComponent(riseFactorSlider.get(), false);
            riseFactorLabel->setJustificationType(juce::Justification::centredTop);

            playButton = std::make_unique<juce::TextButton>("Play");
            playButton->setClickingTogglesState(true);
            playButton.get()->onClick = [this]() {};
            addAndMakeVisible(playButton.get());
            playAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BANG", *playButton);

            theLFLight.setColourScheme(juce::LookAndFeel_V4::getMidnightColourScheme());

            juce::LookAndFeel::setDefaultLookAndFeel(&theLFLight);
        }

        void paint(juce::Graphics& g) override
        {
            /*g.fillAll(juce::Colours::grey);*/
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            juce::Grid grid;
            using Track = juce::Grid::TrackInfo;
            using Fr = juce::Grid::Fr;
            bounds.removeFromTop(20);

            grid.items.add((juce::GridItem(playButton.get())).withSize(60, 60).withJustifySelf(juce::GridItem::JustifySelf::center));
            grid.items.add((juce::GridItem(volumeSlider.get())));
            grid.items.add((juce::GridItem(radiusSlider.get())));
            grid.items.add((juce::GridItem(riseFactorSlider.get())));

            grid.templateRows = { Track(Fr(1)) };
            grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
            grid.columnGap = juce::Grid::Px(10);
            grid.rowGap = juce::Grid::Px(50);

            grid.performLayout(bounds);
        }

        std::unique_ptr<juce::Slider> volumeSlider, radiusSlider, riseFactorSlider;

        std::unique_ptr<juce::Label> volumeLabel,
            radiusLabel, riseFactorLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment,
            radiusAttachment, riseFactorAttachment;

        std::unique_ptr<juce::TextButton> playButton;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> playAttachment;

        juce::LookAndFeel_V4 theLFLight;

        BubbleModelAudioProcessor& audioProcessor;
    };

    MainPanel mainPanel;

    std::unique_ptr<juce::TextButton> presButton;
    int currentPres = 1;
    int loadCurrentPres = 1;
    juce::XmlElement xmlState{ juce::XmlElement("MYPLUGINSETTINGS") };
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BubbleModelAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BubbleModelAudioProcessorEditor)
};
