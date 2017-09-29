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
    volHistory.assign(512, 0.0);
    magnitude.assign(bufferSize, 0.0);
    power.assign(bufferSize, 0.0);
    phase.assign(bufferSize, 0.0);
    binsAmp.assign(numBin.value, 0.0);

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
        // fonts
    overPassMono10.load("overpass-mono/overpass-mono-regular.otf", 10);
    overPassMono12.load("overpass-mono/overpass-mono-regular.otf", 12);
        // gui
    maxFreq.setup(200, samplingFreq / 4, 1300, overPassMono10);
    volume.setup(0, 100, 40, overPassMono12);
    numBin.setup(1, 20, 12, overPassMono10);

    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(backgroundColor);
    ofNoFill();
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


    // check if number of bin changed
    if(numBinPrev != numBin.value)
    {
        binsAmp.assign(numBin.value, 0.0);
    }

    // update stuff
    numBinPrev = numBin.value;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(outlineColor);
    overPassMono12.drawString("SOUND ANALYZER", 32, 32);

    ofMatrix4x4 translation;

    // draw the left channel:
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(32, 64));
    ofPushStyle();
        ofPushMatrix();

            // translation
            ofMultMatrix(translation);

            // window outline
            ofSetLineWidth(1);
            ofDrawRectangle(0, 0, 512, 200);

            // window title
            ofSetColor(outlineColor);
            overPassMono10.drawString("Left Channel", 4, 18);

            // draw curve
            ofSetColor(contentColor);
            ofSetLineWidth(3);
            ofBeginShape();
                for (unsigned int i = 0; i < left.size(); i++){
                    ofVertex(ofMap(i*2, 0, left.size() * 2, 0, 512), 100 -left[i]*180.0f);
                }
            ofEndShape(false);

        ofPopMatrix();
    ofPopStyle();

    // draw the average volume:
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(32, 264));
    ofPushStyle();
        ofPushMatrix();

            // translation
            ofMultMatrix(translation);

            // window outline
            ofDrawRectangle(0, 0, 512, 200);

            // title window
            ofSetColor(outlineColor);
            overPassMono10.drawString("Volume : ", 4, 18);

            // draw volume
            volume.draw(80, 18, translation);
            overPassMono10.drawString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 35);

            // draw circle for scaled volume
            ofSetColor(contentColor);
            ofFill();
            ofDrawCircle(256, 100, scaledVol * 50.0f);

            //lets draw the volume history as a graph
            ofBeginShape();
            for (unsigned int i = 0; i < volHistory.size(); i++){
                if( i == 0 ) ofVertex(i, 200);

                ofVertex(i, 200 - volHistory[i] * 35);

                if( i == volHistory.size() -1 ) ofVertex(i, 200);
            }
            ofEndShape(false);
            ofNoFill();

        ofPopMatrix();
    ofPopStyle();

    // draw the FFt:
    float maxFreqInd = samplePerFreq * maxFreq.value;
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(544, 64));
    ofPushStyle();
        ofPushMatrix();
            // translation
            ofMultMatrix(translation);

            // title of window
            ofSetColor(outlineColor);
            overPassMono10.drawString("FFT", 4, 18);

            // window outline
            ofSetLineWidth(1);
            ofDrawRectangle(0, 0, 512, 200);

            // draw curve
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
            overPassMono10.drawString("0", 5, 220);
                // middle point
            ofDrawLine(256, 200, 256, 210);
            overPassMono10.drawString(ofToString(maxFreq.value / 2), 240, 220);
                // end
            ofDrawLine(512, 200, 512, 210);
            maxFreq.draw(430, 220, translation);
            overPassMono10.drawString("(Hz)", 470, 220);

        ofPopMatrix();
    ofPopStyle();

    // draw the bins:
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(544, 264));
    ofPushStyle();
        ofPushMatrix();
            // translation
            ofMultMatrix(translation);

            // window outline
            ofSetLineWidth(1);
            ofDrawRectangle(0, 0, 512, 200);

            // title of window
            ofSetColor(outlineColor);
            overPassMono10.drawString("BINS", 4, 35);

            // number of bins
            overPassMono10.drawString("Number :", 4, 55);
            numBin.draw(80, 55, translation);

            //
            //binsAmp.assign(numBin.value, 0.0);
            float sc_win = 512 / numBin.value;
            float sc_freqInd = maxFreqInd / numBin.value;
            float sc_freqReal = maxFreq.value / numBin.value;
            ofSetColor(contentColor);
            ofSetLineWidth(1);
            ofBeginShape();
            for(int i = 0; i < binsAmp.size(); i++)
            {
                // compute first ind and last ind of that bin
                    // first and last in freq_amp
                int startInd = i * sc_freqInd;
                int endInd = (i + 1) * sc_freqInd;
                    // first and last on window
                float startWin = i * sc_win;
                float endWin = (i + 1) * sc_win;

                // compute RMS of that bin
                float binValue = 0;
                int numCounted = 0;
                for(int j = startInd; j < endInd; j++)
                {
                    binValue += freq_amp[j] * freq_amp[j];
                    numCounted += 1;
                }
                binValue /= numCounted;
                binValue = sqrt(binValue);

                binsAmp[i] *= 0.99;
                binsAmp[i] += 0.01 * binValue;

                // draw bin
                ofVertex(startWin, 200);
                ofVertex(startWin, 200 - binsAmp[i]);
                ofVertex(endWin, 200 - binsAmp[i]);
                ofVertex(endWin, 200);

                // draw number
                ofSetColor(outlineColor);
                ofDrawLine(startWin, 200, startWin, 210);
                overPassMono10.drawString(ofToString((int) (i * sc_freqReal)), startWin - 10, (i % 2) ? 225 : 240);
                if(i == binsAmp.size() - 1)
                {
                    ofDrawLine(endWin, 200, endWin, 210);
                    overPassMono10.drawString(ofToString((int) ((i+1) * sc_freqReal)), endWin - 10, ((i+1) % 2) ? 225 : 240);
                }
            }
            ofSetLineWidth(3);
            ofSetColor(contentColor);
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
        left[i]		= input[i*2] * volume.value;
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

