#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "GranularSampler.h"
#include "globals.h"

class ofApp : public ofBaseApp {

public:


	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	void audioRequested(float * input, int bufferSize, int nChannels);

	float 	pan;
	int		sampleRate;
	int bufferSize;
	bool 	bRingModulation;
	float 	volume;
	float   speed;
	int		bufferSamples;


	float 	* lAudio;
	float   * rAudio;
	//sample loader is passed to granulars
	Sample sample;
	
	GranularSampler granular[NUMSAMPLERS];

	vector<MiniMaxima> waveForm;

	

	float deltax;
	float deltapos;
	float widthPct;

};

#endif
