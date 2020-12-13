/*
  ==============================================================================

    Generator.h
    Created: 13 Dec 2020 10:11:25am
    Author:  Giova

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SDTCommon.h"
#include "SDTLiquids.h"
#include "Bubble.h"

/**
    Generates a signal based on a user-supplied function.
*/
template <typename SampleType>
class Generator
{
public:
    /** The NumericType is the underlying primitive type used by the SampleType (which
        could be either a primitive or vector)
    */
    using NumericType = typename juce::dsp::SampleTypeHelpers::ElementType<SampleType>::Type;

    /** Creates an oscillator with a periodic input function (-pi..pi).

        If lookup table is not zero, then the function will be approximated
        with a lookup table.
    */
    Generator()
    {
        initialise();
    }

    /** Returns true if the Oscillator has been initialised. */
    bool isInitialised() const noexcept { return isInitialise; }

    /** Initialises the oscillator with a waveform. */
    void initialise()
    {
        bubbleModel = std::make_unique<Bubble>();
        isInitialise = true;

    }

    /** Called before processing starts. */
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = static_cast<NumericType> (spec.sampleRate);
        rampBuffer.resize((int)spec.maximumBlockSize);

        SDT_setSampleRate(sampleRate);

        //reset();
    }

    /** Resets the internal state of the oscillator */
    void reset() noexcept
    {
    }

    //==============================================================================
    /** Returns the result of processing a single sample. */
    SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType input) noexcept
    {
        jassert(isInitialise);
        return SDTBubble_dsp(bubbleModel.get()->getSDTObj());
    }

    /** Processes the input and output buffers supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        jassert(isInitialise);
        auto&& outBlock = context.getOutputBlock();

        // this is an output-only processor
        jassert(outBlock.getNumSamples() <= static_cast<size_t> (rampBuffer.size()));

        auto len = outBlock.getNumSamples();
        auto numChannels = outBlock.getNumChannels();

        if (context.isBypassed)
            context.getOutputBlock().clear();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* dst = outBlock.getChannelPointer(channel);

            for (int sample = 0; sample < len; ++sample) {
                dst[sample] = SDTBubble_dsp(bubbleModel.get()->getSDTObj());
            }
        }
    }

    std::unique_ptr<Bubble> getBubble() {
        return this->bubbleModel;
    }
    

private:
    //==============================================================================
    bool isInitialise{ false };
    std::unique_ptr<Bubble> bubbleModel;
    NumericType sampleRate;
    juce::Array<NumericType> rampBuffer;
};