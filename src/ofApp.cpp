#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(255, 255, 255);
	ofSetFrameRate(30);
	sampleRate = 44100;
	bufferSize=  1024;
	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 4);
	sampler[0].setup("Kupferberg-Tuli_No-Deposit.wav");
	lAudio = new float[bufferSize * 2];
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofSoundStreamStop();
	ofSoundStreamClose();
	delete lAudio;
	delete rAudio;

}

//--------------------------------------------------------------
void ofApp::update() {

	static int last_x = 0;
	static int curr_x = 0;

	last_x = curr_x;
	curr_x = mouseX;
	deltax = (1.0f + fabs(curr_x - last_x)) / 1.0f;

	sampler[0].controlUpdate();

}

//--------------------------------------------------------------
void ofApp::draw() {

	

	ofSetBackgroundColor(0);
	
	sampler[0].draw();

	ofSetColor(255);
	char reportString[255];
	sprintf(reportString, 
		"startZerox: %ld \nendZerox: %ld \nspeed: (%f)\n", 
		sampler[0].sample.startZerox, sampler[0].sample.endZerox, speed);

	ofDrawBitmapString(reportString, 80, 380);

	ofSetColor(127);
	for (int a = 0; a < bufferSize*2; a++) {
		ofLine(600 + a, 200, 600 + a, 200 + lAudio[a] * 200.0f);
	}
		

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	static bool toggle = false;

	if (key == '-') {
		volume -= 0.05;
		volume = MAX(volume, 0);
	}
	else if (key == '+') {
		volume += 0.05;
		volume = MIN(volume, 1);
	}

	if (key == ' ')
	{
		toggle = !toggle;
		sampler[0].sample.setPaused(toggle);
	}



	if (key == 's')
	{
		sampler[0].sample.stop();
	}
	if (key == 'd')
	{
		sampler[0].sample.play();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	sampler[0].mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	/*int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height - y) / height);*/
	mouseMoved(x, y);
	sampler[0].mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	/*bRingModulation = true;*/
	sampler[0].mousePressed(x, y, button);
	
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	sampler[0].mouseReleased(x,y,button);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}
//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {

	unique_lock<mutex> lock(sampler[0].audioMutex);
	for (int i = 0; i < bufferSize; i++) {
		float monoSample = sampler[0].requestNextBakedSample(0);
		output[i*nChannels] = monoSample;
		output[i*nChannels +1] = monoSample;

		lAudio[i+bufferSize] = lAudio[i];
		lAudio[i] = output[i];
	}
	/*
	waveHeader = waveHeader%bakedWaveSize;
	output[i*nChannels] = bakedWave_left[waveHeader];
	output[i*nChannels + 1] = bakedWave_left[waveHeader];
	waveHeader++;
	*/
}

