#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"
#include "ofTrueTypeFont.h"

class CustomSlider
{
public:
    void setup(float _minValue, float _maxValue, float _value, ofTrueTypeFont _font, ofColor _hoverColor, ofColor _dragColor);
    void draw(float posStringX, float posStringY, ofMatrix4x4 transMatrix);

    float   value;
    float   minValue;
    float   maxValue;

    CustomSlider();

private:
    // variables
    ofVec2f posCenterString;
    ofVec2f mousePosStart;
    string  valueString;
    int     numDigit;
    int     numDigitPrev;
    float   valueStart;
    float   diff;
    bool hovered;
    bool dragged;
    bool draggedPrev;
    ofTrueTypeFont textFont;
    bool mousePressedPrev;
    ofColor hoverColor;
    ofColor dragColor;
    ofColor dragColorBoundary;

    ofVec2f getCenterOfString(string text);
};

#endif // CUSTOMSLIDER_H
