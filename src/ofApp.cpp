#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Sound stream setup
    samplingFreq    = 48000; // 48      kHz
    bufferSize      = 2048;  // 2048    samples
    samplePerFreq   = (float) bufferSize / (float) samplingFreq;
    soundStream.printDeviceList();
    soundStream.setup(this, 0, 2, samplingFreq, bufferSize, 3);

    // Signals setup
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    freq_amp.assign((int) bufferSize / 2, 0.0);
    volHistory.assign(400, 0.0);
    magnitude.assign(bufferSize, 0.0);
    power.assign(bufferSize, 0.0);
    phase.assign(bufferSize, 0.0);

    // Floats setup
    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    curVol          = 0.0;
    scaledVol		= 0.0;

    // UI setup
        // colors
    backgroundColor = ofColor(216, 216, 217);
    outlineColor    = ofColor(42);
    contentColor    = ofColor(212, 81, 19);
        // panel
    gui.setup();
    gui.setPosition(32, 64);
        // sliders
    gui.add(volume.setup("Input signal volume", 1.0, 0.0, 1.0));
    gui.add(maxFreq.setup("Max freq (Hz)", samplingFreq / 4, 200, samplingFreq / 4 ));

    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(backgroundColor);
}

//--------------------------------------------------------------
void ofApp::update(){
    //lets scale the vol up to a 0-1 range
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

    //lets record the volume into an array
    volHistory.push_back( scaledVol );

    //if we are bigger the the size we want to record - lets drop the oldest value
    if( volHistory.size() >= 400 ){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    // draw GUI
    gui.draw();

    ofSetColor(outlineColor);
    ofDrawBitmapString("SOUND ANALYZER", 32, 32);

    ofNoFill();

    // draw the left channel:
    ofPushStyle();
        ofPushMatrix();
        ofTranslate(32, 170, 0);

        ofSetColor(outlineColor);
        ofDrawBitmapString("Left Channel", 4, 18);

        ofSetLineWidth(1);
        ofDrawRectangle(0, 0, 512, 200);

        ofSetColor(contentColor);
        ofSetLineWidth(3);

            ofBeginShape();
            for (unsigned int i = 0; i < left.size(); i++){
                ofVertex(ofMap(i*2, 0, left.size() * 2, 0, 512), 100 -left[i]*180.0f);
            }
            ofEndShape(false);

        ofPopMatrix();
    ofPopStyle();

    // draw the FFt:
    float maxFreqInd = samplePerFreq * maxFreq;

    ofPushStyle();
        ofPushMatrix();
        ofTranslate(32, 370, 0);

        ofSetColor(outlineColor);
        ofDrawBitmapString("FFT", 4, 18);

        ofSetLineWidth(1);
        ofDrawRectangle(0, 0, 512, 200);

        ofSetColor(contentColor);
        ofSetLineWidth(3);

            ofBeginShape();
            for (unsigned int i = 0; i < maxFreqInd; i++){
                ofVertex(ofMap(i*2, 0, maxFreqInd * 2, 0, 512), 200 - freq_amp[i]);
            }
            ofEndShape(false);

        // display freq info
        ofSetColor(outlineColor);
        ofSetLineWidth(1);
            // origin
        ofDrawLine(0, 200, 0, 210);
        ofDrawBitmapString("0", -5, 220);
            // middle point
        ofDrawLine(256, 200, 256, 210);
        ofDrawBitmapString(maxFreq / 2, 240, 220);
            // end
        ofDrawLine(512, 200, 512, 210);
        ofDrawBitmapString(maxFreq.getParameter().toString(), 490, 220);
        ofPopMatrix();
    ofPopStyle();

    // draw the average volume:
    ofPushStyle();
        ofPushMatrix();
        ofTranslate(565, 170, 0);

        ofSetColor(outlineColor);
        ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
        ofDrawRectangle(0, 0, 400, 400);

        ofSetColor(contentColor);
        ofFill();
        ofDrawCircle(200, 200, scaledVol * 190.0f);

        //lets draw the volume history as a graph
        ofBeginShape();
        for (unsigned int i = 0; i < volHistory.size(); i++){
            if( i == 0 ) ofVertex(i, 400);

            ofVertex(i, 400 - volHistory[i] * 70);

            if( i == volHistory.size() -1 ) ofVertex(i, 400);
        }
        ofEndShape(false);

        ofPopMatrix();
    ofPopStyle();

    drawCounter++;
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){

    float avg_power = 0.0f;
    curVol = 0.0;

    // samples are "interleaved"
    int numCounted = 0;

    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2] * volume;
        //right[i]	= input[i*2+1];

        curVol += left[i] * left[i];
        //curVol += right[i] * right[i];
        numCounted+=1;
    }

    // compute mean of rms
    curVol /= (float)numCounted;

    // compute root of rms
    curVol = sqrt( curVol );

    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;

    // compute fft
    fftoperator.powerSpectrum(0, (int) bufferSize / 2, &left[0], bufferSize, &magnitude[0], &phase[0], &power[0], &avg_power);

    // store fft
    for(int i = 1; i < bufferSize / 2; i++) {
        freq_amp[i-1] = magnitude[i];
    }

    bufferCounter++;
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

