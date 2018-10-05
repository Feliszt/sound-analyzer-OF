#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // load settings
    ofxXmlSettings settings;
    settings.loadFile("SoundAnalyzerSettings.xml");
    float volumeSetting = settings.getValue("settings:volume", 40);
    float volThreshSetting = settings.getValue("settings:volThresh", 0.5);
    float volScaleSetting = settings.getValue("settings:volScale", 1);
    float maxFreqSetting = settings.getValue("settings:maxFreq", 1300);
    int numBinsSetting = settings.getValue("settings:numBins", 14);
    float smoothingSetting = settings.getValue("settings:smoothing", 0.5);
    float oscAdressSetting = settings.getValue("settings:oscAdress", 8000);

    // Sound stream setup
    samplingFreq    = 48000; // 48      kHz
    bufferSize      = 2048;  // 2048    samples
    samplePerFreq   = (float) bufferSize / (float) samplingFreq;
    soundStream.printDeviceList();
    //soundStream.setup(this, 0, 2, samplingFreq, bufferSize, 3);


    // Signals setup
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    freq_amp.assign((int) bufferSize / 2, 0.0);
    volHistory.assign(WW, 0.0);
    volHistory.assign(WW, 0.0);
    magnitude.assign(bufferSize, 0.0);
    power.assign(bufferSize, 0.0);
    phase.assign(bufferSize, 0.0);
    binsAmp.assign(numBinsSetting, 0.0);

    // Floats setup
    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    curVol          = 0.0;
    curVolPrev      = 0.0;
    divVol          = 0.0;
    maxDivVol       = - 99;
    numHist         = 50;

    // UI setup
        // colors
    backgroundColor = ofColor(224, 224, 224);
    outlineColor    = ofColor(42);
    contentColor    = ofColor(212, 81, 19);
        // fonts
    overPassMono10.load("overpass-mono/overpass-mono-regular.otf", 10);
    overPassMono12.load("overpass-mono/overpass-mono-regular.otf", 12);
    overPassMono14.load("overpass-mono/overpass-mono-regular.otf", 14);
        // gui
    maxFreq.setup(400, samplingFreq / 2, maxFreqSetting, overPassMono10, outlineColor, contentColor);
    volume.setup(0, 100, volumeSetting, overPassMono12, outlineColor, contentColor);
    volThresh.setup(volThreshSetting, outlineColor, contentColor);
    volScale.setup(volScaleSetting, outlineColor, contentColor);
    numBin.setup(1, 50, numBinsSetting, overPassMono10, outlineColor, contentColor);
    smoothBin.setup(smoothingSetting, outlineColor, contentColor);
    oscAdress.setup(ofToString(oscAdressSetting), 6, overPassMono12, outlineColor);

    // OSC setup
    oscSender.setup("localhost", oscAdressSetting);
    oscAdressAsIntPrev = oscAdressSetting;

    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(backgroundColor);
    ofNoFill();
}

//--------------------------------------------------------------
void ofApp::update(){
    //record the volume into an array
    volHistory.push_back(smoothedVol);


    //if we are bigger the the size we want to record - lets drop the oldest value
    if( volHistory.size() >= numHist ){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }

    numBinInt = (int) numBin.getValue();
    // check if number of bin changed
    if(numBinPrev != numBinInt)
    {
        binsAmp.assign(numBinInt, 0.0);
    }

    // onset detection
    float actVolThresh = ofMap(volThresh.getValue(), 0, 1, 0, volScale.getValue());
    above = divVol > actVolThresh;
    onSet = above && !abovePrev;
    if(onSet) ofLog() << "UNTSS\t" << divVol;
    abovePrev = above;


    // check for changes of OSC adress
    oscAdressAsInt = std::stoi(oscAdress.getValue());
    if(oscAdressAsInt != oscAdressAsIntPrev) {
        ofLog() << "Changing osc port to " << oscAdressAsInt;
        oscSender.setup("localhost", oscAdressAsInt);
    }

    // send OSC
    ofxOscMessage m;
        // send onSet
    if(onSet) {
        m.setAddress("/onSet");
        m.addFloatArg(divVol);
        oscSender.sendMessage(m);
        m.clear();
    }
        // send volume
    m.setAddress("/volume");
    m.addFloatArg(smoothedVol);
    oscSender.sendMessage(m);
    m.clear();
        // send number of bins
    m.setAddress("/numBins");
    m.addInt32Arg(numBinInt);
    oscSender.sendMessage(m);
    m.clear();
        // send each bin amplitude
    m.setAddress("/binsFreq");
    for(int i = 0; i < binsAmp.size(); i++)
    {
        m.addFloatArg(binsAmp[i]);
    }
    oscSender.sendMessage(m);
    m.clear();


    // update stuff
    numBinPrev = numBinInt;
    oscAdressAsIntPrev = oscAdressAsInt;
}

//--------------------------------------------------------------
void ofApp::draw(){
    // init trans matrix
    ofMatrix4x4 translation;

    // title of app
    ofSetColor(outlineColor);
    overPassMono14.drawString("SOUND ANALYZER", LW, 30);

    // osc control
    overPassMono12.drawString("OSC adress : ", LW, 70);
    oscAdress.draw(LW + 125, 52, translation);

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
                    ofVertex(ofMap(i*2, 0, left.size() * 2, 0, WW), HW / 2 - ofClamp(left[i] * 180.0f, - HW /2, HW / 2));
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
            volume.draw(100, TH - 12, translation);

            // draw volume
            //overPassMono10.drawString("Scaled average vol (0-100): " + ofToString(smoothedVol * 100, 0), 4, 18);

            // draw scale
            overPassMono10.drawString("Scale :", 4, 18);
            volScale.draw(110, 12, translation);
            overPassMono10.drawString(ofToString(volScale.getValue(), 2), 225, 18);

            // draw volume threshold
            overPassMono10.drawString("Threshold :", 4, 40);
            volThresh.draw(110, 34, translation);
            float actVolThresh = ofMap(volThresh.getValue(), 0, 1, 0, volScale.getValue());
            overPassMono10.drawString(ofToString(actVolThresh, 2), 225, 40);

            // draw circle for scaled volume
            ofSetColor(contentColor);
            ofFill();
            ofDrawCircle(WW / 2, HW / 2, ofClamp(smoothedVol * 100, 0, HW /2));

            //lets draw the volume history as a graph
            ofBeginShape();
            for (unsigned int i = 0; i < derVolHistory.size(); i++){
                float abs = ofMap(i, 0, derVolHistory.size(), 0, WW);
                if( i == 0 ) ofVertex(i, HW);
                float value = ofClamp(ofMap(derVolHistory[i], 0, volScale.getValue(), 0, HW), 0, HW);
                ofVertex(abs, HW - value);
                if( i == derVolHistory.size() -1 ) ofVertex(abs, HW);
            }
            ofEndShape(false);
            ofNoFill();

            // draw threshold
            float volThreshHeight = ofMap(actVolThresh, 0, volScale.getValue(), 0, HW);
            ofDrawLine(0, HW - volThreshHeight, WW, HW - volThreshHeight);


        ofPopMatrix();
    ofPopStyle();

    // third window (2, 1)
    // draw the FFT:
    float maxFreqFloat = samplePerFreq * maxFreq.getValue();
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
                for (unsigned int i = 0; i < maxFreqFloat; i++){
                    ofVertex(ofMap(i*2, 0, maxFreqFloat * 2, 0, WW), HW - ofClamp(freq_amp[i], 0, HW));
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
            overPassMono10.drawString(ofToString(maxFreq.getValue() / 2), WW / 2 - 10, HW + 25);
                // end
            ofDrawLine(WW, HW, WW, HW + 10);
            maxFreq.draw(WW - 30, HW + 15, translation);
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
            numBin.draw(110, 9, translation);

            // smoothing value
            overPassMono10.drawString("Smoothing :", 4, 40);
            smoothBin.draw(110, 34, translation);
            overPassMono10.drawString(ofToString(smoothBin.getValue(), 1), 225, 40);


            float sc_win = WW / (float) numBinInt;
            float sc_freqInd = maxFreqFloat / numBinInt;
            float sc_freqReal = maxFreq.getValue() / numBinInt;

            // draw freq per bin
            overPassMono10.drawString(ofToString(sc_freqReal, 0) + " Hz/bin", WW - 100, 18);

            // compute energy of each bin
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

                binsAmp[i] *= smoothBin.getValue();
                binsAmp[i] += (1 - smoothBin.getValue()) * binValue;

                float displayedValue = ofClamp(binsAmp[i], 0, HW - 50);
                // draw bin
                ofVertex(startWin, HW);
                ofVertex(startWin, HW - displayedValue);
                ofVertex(endWin, HW - displayedValue);
                ofVertex(endWin, HW);
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
        left[i]	= input[i*2] * volume.getValue();
        curVol += left[i] * left[i];
        numCounted+=1;
    }

    // compute mean of rms
    curVol /= (float)numCounted;

    // compute root of rms
    curVol = sqrt( curVol );

    // normalize to 1
    curVol = ofMap(curVol, 0, 0.707 * 1, 0, 1);

    // compute diff
    divVol = ofClamp(curVol - curVolPrev, 0, 999);
    if (divVol > maxDivVol) maxDivVol = divVol;
    derVolHistory.push_back(divVol);
    if(derVolHistory.size() >= numHist){
        derVolHistory.erase(derVolHistory.begin(), derVolHistory.begin()+1);
    }
    curVolPrev = curVol;

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
    settings.setValue("settings:volume", volume.getValue());
    settings.setValue("settings:maxFreq", maxFreq.getValue());
    settings.setValue("settings:numBins", numBin.getValue());
    settings.setValue("settings:smoothing", smoothBin.getValue());
    settings.setValue("settings:oscAdress", oscAdressAsInt);
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

