#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {

	ofBackground(255, 255, 255);
	ofSetFrameRate(30);

	sampleRate = 44100;
	volume = 0.4f;
	bRingModulation = false;
	lAudio = new float[256];
	rAudio = new float[256];

	sample.load("Kupferberg-Tuli_No-Deposit.wav"); // supports mono or stereo .wav files
	sample.setLooping(true);
	sample.play();

	pan = 0.5;

	sample.generateWaveForm(&waveForm);



	ofSoundStreamSetup(2, 0, this, sampleRate, 256, 4);

	loopMaxLength_frames = 100000;

	sample.pointStart_frame = 0;
	sample.pointEnd_frame = min(loopMaxLength_frames, sample.getLength());


	applyDraggableConstraints();

	

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



}

//--------------------------------------------------------------
void ofApp::draw() {

	

	ofSetBackgroundColor(0);
	// draw waveform
	sample.drawWaveForm(5, 500, ofGetWidth() - 10, 100, &waveForm);

	// draw the left:

	ofSetColor(255);
	
	for (int i = 0; i < 256; i++) {
		ofLine(100 + i, 200, 100 + i, 200 + lAudio[i] * 200.0f);
	}
	ofPushStyle();
	ofPushMatrix();
		ofNoFill();
		ofTranslate(400, 400);
		ofBeginShape();
			long displayLength_frames = sample.pointEnd_frame - sample.pointStart_frame;
			unsigned int channs = sample.getChannels();
			for (long a = 0; a < displayLength_frames; a += channs) {
				long a_frame = sample.pointStart_frame + a;
				int rad = sample.getSampleN(a_frame*2)*200 + 300;
				float th = (2 * PI * a) / displayLength_frames;
				ofVertex(sin(th)*rad, cos(th)*rad);
			}
		ofEndShape();
	ofPopMatrix();
	ofPopStyle();
	// draw the right:

	ofSetColor(255);
	for (int i = 0; i < 256; i++) {
		ofLine(600 + i, 200, 600 + i, 200 + rAudio[i] * 200.0f);
	}


	ofSetColor(255);
	char reportString[255];
	sprintf(reportString, "volume: (%f) modify with -/+ keys\npan: (%f)\nspeed: (%f)\nplayback: %s\nposition: %f\npaused: %s", volume, pan, speed, bRingModulation ? "ring modulation" : "normal", sample.getPosition(), sample.getIsPaused() ? "yes" : "no");

	ofDrawBitmapString(reportString, 80, 380);

	startPointDraggable.draw();
	endPointDraggable.draw();

}

void ofApp::applyDraggableConstraints() {
	int width = ofGetWidth();
	startPointDraggable.position[0] = width*sample.pointStart_frame /sample.getLength_frames();
	endPointDraggable.position[0] = width*sample.pointEnd_frame / sample.getLength_frames();
	startPointDraggable.position[1] = 400;
	endPointDraggable.position[1] = 400;
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
		sample.setPaused(toggle);
	}



	if (key == 's')
	{
		sample.stop();
	}
	if (key == 'd')
	{
		sample.play();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

void ofApp::setStartFrame(long to) {
	if(to<sample.getLength_frames())
	sample.pointStart_frame = to;
	
	long maxEnd = loopMaxLength_frames + sample.pointStart_frame;
	maxEnd = min(maxEnd, sample.getLength()*sample.getChannels());
	if (sample.pointEnd_frame > maxEnd) {
		sample.pointEnd_frame = maxEnd;
	}
};
void ofApp::setEndFrame(long to) {
	if (to<sample.getLength_frames())
	sample.pointEnd_frame = to;
	long minstart = sample.pointEnd_frame - loopMaxLength_frames;
	minstart = max((long)0, minstart);

	if (minstart > sample.pointStart_frame) {
		sample.pointStart_frame = minstart;
	}
	
};
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	//*
	int width = ofGetWidth();
	

	startPointDraggable.onMouseMoved(x,y);
	endPointDraggable.onMouseMoved(x, y);
	//applyDraggableConstraints();
	//setStartFrame((sample.getLength()*x / width) / sample.getChannels());//*/
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	/*int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height - y) / height);*/
	mouseMoved(x, y);
	int width = ofGetWidth();
	long ssf = ((long)(sample.getLength_frames()*startPointDraggable.position[0])) / width;
	setStartFrame(ssf);
	setEndFrame(((long)(sample.getLength_frames()*endPointDraggable.position[0])) / width);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	/*bRingModulation = true;*/
	if (startPointDraggable.onClick()) {
	}else if(endPointDraggable.onClick()){}
	
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	/*bRingModulation = false;*/
	startPointDraggable.onCRelease();
	endPointDraggable.onCRelease();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}
//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {

	float leftScale = 1 - pan;
	float rightScale = pan;

	for (int i = 0; i < bufferSize; i++) {
		// mono
		if (sample.getChannels() == 1) {
			float smp = sample.update();
			lAudio[i] = output[i*nChannels] = smp * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = smp * volume * rightScale;
		}
		// stereo
		else if (sample.getChannels() == 2) {
			lAudio[i] = output[i*nChannels] = sample.update() * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = sample.update() * volume * rightScale;
		}
		else
		{
			lAudio[i] = output[i*nChannels] = 0.0;
			rAudio[i] = output[i*nChannels + 1] = 0.0;
		}
	}
}

