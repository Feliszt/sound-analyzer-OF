#include "customslider.h"

CustomSlider::CustomSlider()
{

}

void CustomSlider::setup(float _minValue, float _maxValue, float _value, ofTrueTypeFont _font)
{
    minValue = _minValue;
    maxValue = _maxValue;
    value = _value;
    textFont = _font;

    posCenterString = getCenterOfString(ofToString(value));
}

void CustomSlider::draw(float posStringX, float posStringY, ofMatrix4x4 transMatrix)
{
    ofPoint posStringAbs = ofPoint(posStringX + posCenterString.x, posStringY - posCenterString.y) * transMatrix;

    // get string from value
    valueString = ofToString(value);

    // check if hovered
    hovered = (ofGetMouseX() < posStringAbs.x + 40) &&
              (ofGetMouseX() > posStringAbs.x - 40) &&
              (ofGetMouseY() < posStringAbs.y + 20) &&
              (ofGetMouseY() > posStringAbs.y - 20);

    // check if dragged
    if(hovered && ofGetMousePressed())
    {
        dragged = true;
    }
    dragged &= ofGetMousePressed();

    // store mouse posString and value when dragging starts
    if(dragged && !draggedPrev)
    {
        mousePosStart = ofVec2f(ofGetMouseX(), ofGetMouseY());
        valueStart = value;
    }

    // compare mouse posStringition to stored posStringition and update value accordingly
    if(dragged)
    {
        diff = ofGetMouseX() - mousePosStart.x;
        float valueUpdate = valueStart + diff;
        if((valueUpdate >= minValue) && (valueUpdate <= maxValue))
        {
            valueUpdatable = true;
            value = valueUpdate;
        } else {
            valueUpdatable = false;
        }
    }

    // get boundingbox of string
    numDigit = valueString.length();
    if(numDigit != numDigitPrev) posCenterString = getCenterOfString(valueString);

    // update stuff
    draggedPrev = dragged;
    numDigitPrev = numDigit;

    // draw value
    textFont.drawString(ofToString(value), posStringX, posStringY);

    // translate
    ofPushStyle();
    ofFill();
    ofPushMatrix();
    ofTranslate(posStringX + posCenterString.x, posStringY - posCenterString.y, 0);

        if((hovered && !dragged) || (dragged && diff == 0))
        {
            ofSetColor(ofColor::black);
            ofDrawTriangle(20, - 5, 20, 5, 25, 0);
            ofDrawTriangle(- 20, -5, - 20, 5, - 25, 0);
        }

        if(dragged && (diff > 0))
        {
            if(valueUpdatable)
            {
                ofSetColor(ofColor::darkGreen);
            } else {
                ofSetColor(ofColor::darkRed);
            }
            ofDrawTriangle(20, - 5, 20, 5, 25, 0);
        }

        if(dragged && (diff < 0))
        {
            if(valueUpdatable)
            {
                ofSetColor(ofColor::darkGreen);
            } else {
                ofSetColor(ofColor::darkRed);
            }
            ofDrawTriangle(- 20, -5, - 20, 5, - 25, 0);
        }

    ofPopMatrix();
    ofNoFill();
    ofPopStyle();
}


ofVec2f CustomSlider::getCenterOfString(string text)
{
    ofRectangle stringBoundingBox = textFont.getStringBoundingBox(text, 0, 0);
    return ofVec2f(stringBoundingBox.getWidth() / 2, stringBoundingBox.getHeight() / 2);
}
