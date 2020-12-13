/*
  ==============================================================================

    Processor.h
    Created: 13 Dec 2020 11:04:57am
    Author:  Giova

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Generator.h"
//==============================================================================
class Processor
{
public:
    Processor()
    {
        auto& masterGain = processorChain.get<masterGainIndex>();
        masterGain.setGainLinear(1.0f);
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize);
        processorChain.prepare(spec);
    }

    //==============================================================================
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) 
    {
        auto block = tempBlock.getSubBlock(0, (size_t)numSamples);
        block.clear();
        juce::dsp::ProcessContextReplacing<float> context(block);
        processorChain.process(context);

        juce::dsp::AudioBlock<float>(outputBuffer)
            .getSubBlock((size_t)startSample, (size_t)numSamples)
            .add(tempBlock);
    }

    //==============================================================================
    std::unique_ptr<Bubble> getModel()
    {
        return processorChain.template get<osc1Index>().getBubble();
    }

private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        osc1Index,
        masterGainIndex
    };

    juce::dsp::ProcessorChain<Generator<float>, juce::dsp::Gain<float>> processorChain;
};