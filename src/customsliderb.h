#ifndef CUSTOMSLIDERB_H
#define CUSTOMSLIDERB_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"

class CustomSliderB
{
public:
    void setup(float _value, ofColor _rectColor, ofColor _ellColor);
    void draw(float posX, float poxY, ofMatrix4x4 transMatrix);
    float value;
    ofColor rectColor;
    ofColor ellColor;
    ofColor ellColorHovered;

    CustomSliderB();

    private:
    int     w_slider;
    int     h_slider;
    int     r_slider;
    bool    hovered;
    bool    dragged;
    bool    draggedPrev;
    float   valueStart;
    ofVec2f mousePosStart;
    float diff;
    bool    mousePressedPrev;
};

#endif // CUSTOMSLIDERB_H
