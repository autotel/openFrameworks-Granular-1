#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(255, 255, 255);
	ofSetFrameRate(30);
	sampleRate = 44100;
	bufferSize=  1024;

	//sample = Sample();
	//while (!sample.isLoaded) {}

	sample.load("Kupferberg-Tuli_No-Deposit.wav"); // supports mono or stereo .wav files
	//sample.setLooping(true);
	sample.play();
	sample.generateWaveForm(&waveForm);

	for (int s = 0; s < NUMSAMPLERS;s++)
		granular[s].setup(sample, 0, (ofGetHeight()/2)+30*s);
	
	


	

	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 4);


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
	for (int s = 0; s < NUMSAMPLERS; s++)
	granular[s].controlUpdate();

}

//--------------------------------------------------------------
void ofApp::draw() {

	/*if (granular[0].startPointDraggable.isClicked)
	sample.drawWaveForm(0, 0, ofGetWidth(), 100, &waveForm);
	*/
	sample.drawWaveForm(0, 0, ofGetWidth(), 100, &waveForm);
	ofSetBackgroundColor(0);
	for (int s = 0; s < NUMSAMPLERS; s++)
	granular[s].draw();
	/*
	ofSetColor(255);
	char reportString[255];
	/*sprintf(reportString, 
		"startZerox: %ld \nendZerox: %ld \nspeed: (%f)\nllen_fr: %ld", 
		granular[0].startZerox, granular[0].endZerox, speed,granular[0].pointStart_frame- granular[0].pointEnd_frame);
		
	ofDrawBitmapString(reportString, 80, 380);*/

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
		granular[0].sample.setPaused(toggle);
	}



	if (key == 's')
	{
		granular[0].sample.stop();
	}
	if (key == 'd')
	{
		granular[0].sample.play();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	for (int s = 0; s < NUMSAMPLERS; s++)
	granular[s].mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	/*int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height - y) / height);*/
	mouseMoved(x, y);
	for (int s = 0; s < NUMSAMPLERS; s++)
	granular[s].mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	/*bRingModulation = true;*/
	for (int s = 0; s < NUMSAMPLERS; s++)
	granular[s].mousePressed(x, y, button);
	
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	for (int s = 0; s < NUMSAMPLERS; s++)
	granular[s].mouseReleased(x,y,button);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}
//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {

	//unique_lock<mutex> lock(sampler[0].audioMutex);
	for (int i = 0; i < bufferSize; i++) {
		float monoSample = 0;
		for (int s = 0; s < NUMSAMPLERS; s++)
			monoSample += granular[s].requestNextBakedSample(0)/NUMSAMPLERS;
		output[i*nChannels] = monoSample;
		output[i*nChannels +1] = monoSample;

		/*lAudio[i+bufferSize] = lAudio[i];*/
		//lAudio[i] = output[i];
	}
	/*
	waveHeader = waveHeader%bakedWaveSize;
	output[i*nChannels] = bakedWave_left[waveHeader];
	output[i*nChannels + 1] = bakedWave_left[waveHeader];
	waveHeader++;
	*/
}

