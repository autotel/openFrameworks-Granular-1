#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Sample.h"
#include "draggable.h"

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
	bool 	bRingModulation;
	float 	volume;
	float   speed;
	int		bufferSamples;

	float 	* lAudio;
	float   * rAudio;

	

	vector<MiniMaxima> waveForm;

	Sample sample;

	float deltax;
	float deltapos;
	float widthPct;


	long loopStartFrame;
	long loopEndFrame;
	long loopMaxLength_frames;

	void setStartFrame(long);
	void setEndFrame(long);

	draggable startPointDraggable;
	draggable endPointDraggable;

	void applyDraggableConstraints();

	vector<double> bakedWave_left;
	vector<double> bakedWaveCopy_left;

	long bakedWaveSize = 0;
	bool updateBakedWave_flag = false;
	unsigned int waveHeader = 0;
	mutex audioMutex;
};

#endif
