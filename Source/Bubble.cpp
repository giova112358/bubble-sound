/*
  ==============================================================================

    Bubble.cpp
    Created: 30 Nov 2020 2:34:48pm
    Author:  Giova

  ==============================================================================
*/

#include "Bubble.h"

Bubble::Bubble()
{
    bubble = SDTBubble_new();
}

Bubble::~Bubble()
{
}

void Bubble::setBubbleParameters(double radius, double riseFactor)
{
    SDTBubble_setDepth(bubble, 1.0);
    SDTBubble_setRadius(bubble, 0.001 *radius);
    SDTBubble_setRiseFactor(bubble, riseFactor);
}

void Bubble::bang()
{
    SDTBubble_update(bubble);
    SDTBubble_normAmp(bubble);
}

SDTBubble* Bubble::getSDTObj()
{
    return this->bubble;
}
