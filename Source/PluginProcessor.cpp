/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BubbleModelAudioProcessor::BubbleModelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    apvts.addParameterListener("VOL", this);
    apvts.addParameterListener("RAD", this);
    apvts.addParameterListener("RISE", this);
    apvts.addParameterListener("BANG", this);

    init();
}

BubbleModelAudioProcessor::~BubbleModelAudioProcessor()
{
}

//==============================================================================
const juce::String BubbleModelAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BubbleModelAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BubbleModelAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BubbleModelAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BubbleModelAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BubbleModelAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BubbleModelAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BubbleModelAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BubbleModelAudioProcessor::getProgramName (int index)
{
    return {};
}

void BubbleModelAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BubbleModelAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    prepare(sampleRate);
    updateVolume();
    updateBubbleParameters();
    reset();
    isActive = true;
}

void BubbleModelAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BubbleModelAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void BubbleModelAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!isActive)
        return;
    if (mustUpdateVolume)
        updateVolume();
    if (mustUpdateBubblePrameters)
        updateBubbleParameters();
    if (mustBang)
        bang();

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            channelData[sample] = 0.25*model[channel]->process();
            channelData[sample] = juce::jlimit(-1.0f, 1.0f, channelData[sample]);
        }

        mVolume[channel].applyGain(channelData, numSamples);
    }
}

//==============================================================================
bool BubbleModelAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BubbleModelAudioProcessor::createEditor()
{
    /*return new ImpactModelAudioProcessorEditor(*this);*/
    return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void BubbleModelAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    /*juce::ValueTree copyState = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml = copyState.createXml();
    copyXmlToBinary(*xml.get(), destData);*/
}

void BubbleModelAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
   /* std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    juce::ValueTree copyState = juce::ValueTree::fromXml(*xml.get());
    apvts.replaceState(copyState);*/
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BubbleModelAudioProcessor();
}

//===============================================================================
void BubbleModelAudioProcessor::init()
{
    model[0] = std::make_unique<DspProcessor>();
    model[1] = std::make_unique<DspProcessor>();
}

//===============================================================================
void BubbleModelAudioProcessor::prepare(double sampleRate)
{
    for (int channel = 0; channel < numChannels; ++channel) {
        model[channel]->setSampleRate(sampleRate);
    }
}

//===============================================================================
void BubbleModelAudioProcessor::updateVolume()
{
    mustUpdateVolume = false;

    auto volume = apvts.getRawParameterValue("VOL");

    for (int channel = 0; channel < numChannels; ++channel) {
        mVolume[channel].setTargetValue(juce::Decibels::decibelsToGain(volume->load()));
    }
}

void BubbleModelAudioProcessor::updateBubbleParameters()
{
    mustUpdateBubblePrameters = false;
    auto radius = apvts.getRawParameterValue("RAD");

    auto rise = apvts.getRawParameterValue("RISE");

    for (int channel = 0; channel < numChannels; ++channel) {
        model[channel]->bubbleModel->setBubbleParameters(radius->load(), rise->load());
    }
}

//===============================================================================
void BubbleModelAudioProcessor::bang()
{
    mustBang = false;
    auto vel = apvts.getRawParameterValue("BANG");

    for (int channel = 0; channel < numChannels; ++channel) {
        model[channel]->bubbleModel->bang();
    }

}

//===============================================================================
void BubbleModelAudioProcessor::reset()
{
    for (int channel = 0; channel < numChannels; ++channel) {
        mVolume[channel].reset(getSampleRate(), 0.050);
    }
}

//===============================================================================
void BubbleModelAudioProcessor::addBubbleParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    std::function<juce::String(float, int)> valueToTextFunction = [](float x, int l) {return juce::String(x, 4);  };
    std::function<float(const juce::String&)> textToValueFunction = [](const juce::String& str) {return str.getFloatValue(); };

    auto radius = std::make_unique<juce::AudioParameterFloat>("RAD", "Radius", juce::NormalisableRange<float>(0.15, 150.0), 5.588581,
        "k", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction);

    auto riseFactor = std::make_unique<juce::AudioParameterFloat>("RISE", "RiseFactor", juce::NormalisableRange<float>(0.0, 3.0), 0.144675,
                "alpha", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction);
    

    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("sdt.bubble", "BUBBLE PARAMETERS", "|",
        std::move(radius), std::move(riseFactor));

    layout.add(std::move(group));                                                                        
}

void BubbleModelAudioProcessor::addGainParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    std::function<juce::String(float, int)> valueToTextFunction = [](float x, int l) {return juce::String(x, 4);  };
    std::function<float(const juce::String&)> textToValueFunction = [](const juce::String& str) {return str.getFloatValue(); };

    auto bang = std::make_unique<juce::AudioParameterBool>("BANG", "bang", false);

    auto volume = std::make_unique<juce::AudioParameterFloat>("VOL", "Volume", juce::NormalisableRange< float >(-40.0f, 10.0f), 0.0f,
        "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction,
        textToValueFunction);

    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("sdt.gain", "VOLUME CONTROLS", "|",
        std::move(volume), std::move(bang));

    layout.add(std::move(group));

}

juce::AudioProcessorValueTreeState::ParameterLayout BubbleModelAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    BubbleModelAudioProcessor::addBubbleParameters(layout);
    BubbleModelAudioProcessor::addGainParameters(layout);
    return layout;
}

juce::OwnedArray<juce::RangedAudioParameter>& BubbleModelAudioProcessor::getCurrentParameters()
{
    juce::OwnedArray<juce::RangedAudioParameter> currentParameter{ apvts.getParameter("RAD"), apvts.getParameter("RISE") };

    return currentParameter;
}

void BubbleModelAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "VOL") 
        mustUpdateVolume = true;
    if (parameterID == "RAD")
        mustUpdateBubblePrameters = true;
    if (parameterID == "BANG")
        mustBang = true;

}

void BubbleModelAudioProcessor::setPresetStateValueTree(std::unique_ptr<juce::XmlElement> xmlState)
{
    juce::ValueTree copyState = juce::ValueTree::fromXml(*xmlState.get());
    apvts.replaceState(copyState);

}

juce::XmlElement BubbleModelAudioProcessor::getAndSavePresetStateValueTree()
{

    juce::ValueTree copyState = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml = copyState.createXml();
    juce::XmlElement xmlState = *xml.get();

    return xmlState;
}

