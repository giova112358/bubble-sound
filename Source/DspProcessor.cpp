/*
  ==============================================================================

    DspProcessor.cpp
    Created: 27 Nov 2020 4:00:33pm
    Author:  Giova

  ==============================================================================
*/

#include "DspProcessor.h"

DspProcessor::DspProcessor()
{
    bubbleModel = std::make_unique<Bubble>();
}

DspProcessor::~DspProcessor()
{
}

void DspProcessor::setSampleRate(double sampleRate)
{
    SDT_setSampleRate(sampleRate);
}

double DspProcessor::process()
{
    return SDTBubble_dsp(bubbleModel.get()->getSDTObj());
}

//======================================================================================
void DspProcessor::reset()
{

}
