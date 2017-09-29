#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // load settings
    ofxXmlSettings settings;
    settings.loadFile("SoundAnalyzerSettings.xml");
    float volumeSetting = settings.getValue("settings:volume", 40);
    float maxFreqSetting = settings.getValue("settings:maxFreq", 1300);
    float numBinsSetting = settings.getValue("settings:numBins", 14);
    float smoothingSetting = settings.getValue("settings:smoothing", 0.5);

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
    volHistory.assign(WW, 0.0);
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
    overPassMono14.load("overpass-mono/overpass-mono-regular.otf", 14);
        // gui
    maxFreq.setup(400, samplingFreq / 4, maxFreqSetting, overPassMono10, outlineColor, contentColor);
    volume.setup(0, 100, volumeSetting, overPassMono12, outlineColor, contentColor);
    numBin.setup(1, 50, numBinsSetting, overPassMono10, outlineColor, contentColor);
    smoothBin.setup(smoothingSetting, outlineColor, contentColor);

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
    overPassMono14.drawString("SOUND ANALYZER", LW, UH / 2);

    ofMatrix4x4 translation;

    // first window (1, 1)
    // draw the left channel:
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(LW, UH));
    ofPushStyle();
        ofPushMatrix();

            // translation
            ofMultMatrix(translation);

            // window outline
            ofSetLineWidth(2);
            ofDrawRectangle(0, 0, WW, HW);

            // window title
            ofSetColor(outlineColor);
            overPassMono12.drawString("Left Channel", 0, TH);

            // draw curve
            ofSetColor(contentColor);
            ofSetLineWidth(3);
            ofBeginShape();
                for (unsigned int i = 0; i < left.size(); i++){
                    ofVertex(ofMap(i*2, 0, left.size() * 2, 0, WW), HW / 2 -left[i]*180.0f);
                }
            ofEndShape(false);

        ofPopMatrix();
    ofPopStyle();

    // second window (2, 1)
    // draw the average volume:
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(LW, UH + HW + INTW));
    ofPushStyle();
        ofPushMatrix();

            // translation
            ofMultMatrix(translation);

            // window outline
            ofSetLineWidth(2);
            ofDrawRectangle(0, 0, WW, HW);

            // title window
            ofSetColor(outlineColor);
            overPassMono12.drawString("Volume : ", 0, TH);

            // draw volume
            volume.draw(100, TH, translation);
            overPassMono10.drawString("Scaled average vol (0-100): " + ofToString(scaledVol * HW / 4, 0), 4, 18);

            // draw circle for scaled volume
            ofSetColor(contentColor);
            ofFill();
            ofDrawCircle(WW / 2, HW / 2, scaledVol * HW / 4);

            //lets draw the volume history as a graph
            ofBeginShape();
            for (unsigned int i = 0; i < volHistory.size(); i++){
                if( i == 0 ) ofVertex(i, HW);

                ofVertex(i, HW - volHistory[i] * 35);

                if( i == volHistory.size() -1 ) ofVertex(i, HW);
            }
            ofEndShape(false);
            ofNoFill();

        ofPopMatrix();
    ofPopStyle();

    // third window (2, 1)
    // draw the FFT:
    float maxFreqInd = samplePerFreq * maxFreq.value;
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(LW + WW + INTW, UH));
    ofPushStyle();
        ofPushMatrix();
            // translation
            ofMultMatrix(translation);

            // title of window
            ofSetColor(outlineColor);
            overPassMono12.drawString("FFT", 0, TH);

            // window outline
            ofSetLineWidth(2);
            ofDrawRectangle(0, 0, WW, HW);

            // draw curve
            ofSetColor(contentColor);
            ofSetLineWidth(3);
            ofBeginShape();
                for (unsigned int i = 0; i < maxFreqInd; i++){
                    ofVertex(ofMap(i*2, 0, maxFreqInd * 2, 0, WW), HW - freq_amp[i]);
                }
            ofEndShape(false);

            // display freq info
            ofSetColor(outlineColor);
            ofSetLineWidth(2);
                // origin
            ofDrawLine(0, HW, 0, HW + 10);
            overPassMono10.drawString("0", -10, HW + 25);
                // middle point
            ofDrawLine(WW / 2, HW, WW / 2, HW + 10);
            overPassMono10.drawString(ofToString(maxFreq.value / 2), WW / 2 - 10, HW + 25);
                // end
            ofDrawLine(WW, HW, WW, HW + 10);
            maxFreq.draw(WW - 30, HW + 25, translation);
            overPassMono10.drawString("(Hz)", WW + 15, HW + 25);

        ofPopMatrix();
    ofPopStyle();

    // fourth window (2, 2)
    // draw the bins:
    translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(LW + WW + INTW, UH + HW + INTW));
    ofPushStyle();
        ofPushMatrix();
            // translation
            ofMultMatrix(translation);

            // window outline
            ofSetLineWidth(2);
            ofDrawRectangle(0, 0, WW, HW);

            // title of window
            ofSetColor(outlineColor);
            overPassMono12.drawString("BINS", 0, TH);

            // number of bins
            overPassMono10.drawString("Number :", 4, 18);
            numBin.draw(110, 18, translation);

            // smoothing value
            overPassMono10.drawString("Smoothing :", 4, 40);
            smoothBin.draw(110, 35, translation);
            overPassMono10.drawString(ofToString(smoothBin.value), 225, 40);

            // compute energy of each bin
            int numBinInt = (int) numBin.value;
            float sc_win = WW / numBinInt;
            float sc_freqInd = maxFreqInd / numBinInt;
            float sc_freqReal = maxFreq.value / numBinInt;
            bool up = true;
            ofSetColor(contentColor);
            ofSetLineWidth(2);
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

                binsAmp[i] *= smoothBin.value;
                binsAmp[i] += (1 - smoothBin.value) * binValue;

                // draw bin
                ofVertex(startWin, HW);
                ofVertex(startWin, HW - binsAmp[i]);
                ofVertex(endWin, HW - binsAmp[i]);
                ofVertex(endWin, HW);

                // draw number
                ofSetColor(outlineColor);

                int numPlotFreq = ofMap(binsAmp.size(), numBin.minValue, numBin.maxValue, 2, 5);
                if(i % numPlotFreq)
                {
                    ofSetLineWidth(2);
                    overPassMono10.drawString(ofToString((int) (i * sc_freqReal)), startWin - 10, up ? HW + 25 : HW + 40);

                    // update up
                    up = !up;
                } else {
                    ofSetLineWidth(1);
                }

                // draw line
                ofDrawLine(startWin, HW, startWin, HW + 10);

                // always draw last frequency
                if(i == binsAmp.size() - 1)
                {
                    overPassMono10.drawString(ofToString((int) ((i + 1) * sc_freqReal)), endWin - 10, up ? HW + 25 : HW + 40);
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

void ofApp::exit()
{
    ofxXmlSettings settings;
    settings.setValue("settings:volume", volume.value);
    settings.setValue("settings:maxFreq", maxFreq.value);
    settings.setValue("settings:numBins", numBin.value);
    settings.setValue("settings:smoothing", smoothBin.value);
    settings.saveFile("SoundAnalyzerSettings.xml");
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

