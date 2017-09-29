#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "fft.h"
#include "customslider.h"

class ofApp : public ofBaseApp{

    public:

        void setup();
        void update();
        void draw();

        ofRectangle getBitmapStringBoundingBox(string text);

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

        void audioIn(float * input, int bufferSize, int nChannels);

        vector <float> left;
        vector <float> right;
        vector <float> volHistory;
        vector <float> freq_amp;
        vector <float> magnitude;
        vector <float> phase;
        vector <float> power;
        vector <float> binsAmp;

        // frequency analysis
        FFT fftoperator;

        int 	bufferCounter;
        int 	drawCounter;

        float curVol;
        float smoothedVol;
        float scaledVol;

         // UI variables
            // panel and sliders
        CustomSlider maxFreq;
        CustomSlider volume;
        CustomSlider numBin;
        float        numBinPrev;
            // colors
        ofColor backgroundColor;
        ofColor outlineColor;
        ofColor contentColor;
            // font
        ofTrueTypeFont overPassMono10;
        ofTrueTypeFont overPassMono12;

        // Sound stream variable
        ofSoundStream soundStream;
        int bufferSize;
        int samplingFreq;
        float samplePerFreq;
};
