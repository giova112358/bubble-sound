/*
  ==============================================================================

    Bubble.h
    Created: 30 Nov 2020 2:34:48pm
    Author:  Giova

  ==============================================================================
*/

#pragma once
#include "SDTCommon.h"
#include "SDTLiquids.h"

class Bubble
{
public:
    Bubble();
    ~Bubble();

    void setBubbleParameters(double radius, double riseFactor);
    void bang();

    SDTBubble* getSDTObj();

private:
    SDTBubble* bubble;
};