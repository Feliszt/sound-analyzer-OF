#include "customslider.h"

CustomSlider::CustomSlider()
{

}

void CustomSlider::setup(float _minValue, float _maxValue, float _value, ofTrueTypeFont _font, ofColor _hoverColor, ofColor _dragColor)
{
    minValue = _minValue;
    maxValue = _maxValue;
    value = _value;
    textFont = _font;
    hoverColor = _hoverColor;
    dragColor = _dragColor;
    dragColorBoundary = _dragColor;
    dragColorBoundary.setBrightness(100);

    posCenterString = getCenterOfString(ofToString(value));
}

void CustomSlider::draw(float posStringX, float posStringY, ofMatrix4x4 transMatrix)
{
    ofPoint posStringAbs = ofPoint(posStringX + posCenterString.x, posStringY - posCenterString.y) * transMatrix;

    // get string from value
    valueString = ofToString((int) value);

    // check if hovered
    hovered = (ofGetMouseX() < posStringAbs.x + 40) &&
              (ofGetMouseX() > posStringAbs.x - 40) &&
              (ofGetMouseY() < posStringAbs.y + 10) &&
              (ofGetMouseY() > posStringAbs.y - 10);

    // check if dragged
    if(hovered && ofGetMousePressed() && !mousePressedPrev)
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
        float valueUpdate = valueStart + diff / ofGetWidth() * (maxValue - minValue);
        value = ofClamp(valueUpdate, minValue, maxValue);
    }

    // get boundingbox of string
    numDigit = valueString.length();
    if(numDigit != numDigitPrev) posCenterString = getCenterOfString(valueString);

    // update stuff
    draggedPrev = dragged;
    mousePressedPrev = ofGetMousePressed();
    numDigitPrev = numDigit;

    // draw value
    textFont.drawString(valueString, posStringX, posStringY);

    // translate
    ofPushStyle();
    ofFill();
    ofPushMatrix();
    ofTranslate(posStringX + posCenterString.x, posStringY - posCenterString.y, 0);

        if((hovered && !dragged) || (dragged && diff == 0))
        {
            ofSetColor(hoverColor);
            ofDrawTriangle(20, - 5, 20, 5, 25, 0);
            ofDrawTriangle(- 20, -5, - 20, 5, - 25, 0);
        }

        if(dragged && (diff > 0))
        {
            ofSetColor(dragColor);
            if(value == maxValue)
            {
                ofSetColor(dragColorBoundary);

            }
            ofDrawTriangle(20, - 5, 20, 5, 25, 0);
        }

        if(dragged && (diff < 0))
        {
            ofSetColor(dragColor);
            if(value == minValue)
            {
                ofSetColor(dragColorBoundary);

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
