#include "customsliderb.h"

CustomSliderB::CustomSliderB()
{

}

void CustomSliderB::setup(float _value, ofColor _rectColor, ofColor _ellColor)
{
    value = _value;
    rectColor = _rectColor;
    ellColor = _ellColor;
    ellColorHovered = _ellColor;
    ellColorHovered.setBrightness(255);

    w_slider = 100;
    h_slider = 3;
    r_slider = 7;
}

void CustomSliderB::draw(float posX, float posY, ofMatrix4x4 transMatrix)
{
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
        float valueUpdate = valueStart + diff / w_slider;
        value = ofClamp(valueUpdate, 0, 1);
    }

    // compute position of slider
    ofPoint posSlider = ofPoint(posX + value * w_slider, posY + h_slider / 2);
    ofPoint posSliderAbs = posSlider * transMatrix;

    // check if hovered
    hovered = (ofGetMouseX() < posSliderAbs.x + r_slider) &&
              (ofGetMouseX() > posSliderAbs.x - r_slider) &&
              (ofGetMouseY() < posSliderAbs.y + r_slider) &&
              (ofGetMouseY() > posSliderAbs.y - r_slider);

    // set style
    ofPushStyle();

        ofFill();

        // draw slider
        ofSetColor(rectColor);
        ofDrawRectRounded(posX, posY, w_slider, h_slider, 10);

        // draw ellipse
        ofSetColor((hovered || dragged)? ellColorHovered : ellColor);
        ofDrawCircle(posSlider, r_slider);

    // reset style
    ofPopStyle();

    // update stuff
    draggedPrev = dragged;
    mousePressedPrev = ofGetMousePressed();
}
