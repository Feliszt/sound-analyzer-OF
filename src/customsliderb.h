#ifndef CUSTOMSLIDERB_H
#define CUSTOMSLIDERB_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"

class CustomSliderB
{
public:
    void setup();
    void setup(float _value);
    void setup(float _value, int _wSlider, int _hSlider, int _rSlider);
    void setup(float _value, ofColor _rectColor, ofColor _ellColor);
    void setup(float _value, int _wSlider, int _hSlider, int _rSlider, ofColor _rectColor, ofColor _ellColor);

    void draw(float posX, float poxY, ofMatrix4x4 transMatrix);
    float value;
    ofColor rectColor;
    ofColor ellColor;
    ofColor ellColorHovered;

    CustomSliderB();

    private:
    int     wSlider;
    int     hSlider;
    int     rSlider;
    bool    rectHovered;
    bool    ellHovered;
    bool    dragged;
    bool    draggedPrev;
    float   valueStart;
    ofVec2f mousePosStart;
    float   diff;
    bool    mousePressedPrev;
};

#endif // CUSTOMSLIDERB_H
