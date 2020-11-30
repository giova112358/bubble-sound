/*
  ==============================================================================

    DspProcessor.h
    Created: 27 Nov 2020 4:00:33pm
    Author:  Giova

    The processing is done by retriving the displacement of the modal resonator
    in the first pickup point. The processing is a sample-by-sample processing.
    The only processing information needed by the library is the sample rate.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SDTCommon.h"
#include "SDTLiquids.h"
#include "Bubble.h"

class DspProcessor
{
public:
    DspProcessor();
    ~DspProcessor();

    void setSampleRate(double sampleRate);
    double process();
    void reset();

    std::unique_ptr<Bubble> bubbleModel;
};
