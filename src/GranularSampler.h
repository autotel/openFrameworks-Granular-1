#pragma once

#include "ofMain.h"
#include "GranularSampler.h"
#include "draggable.h"
#include "Sample.h"
#include "globals.h"

class GranularSampler
{
private:

	draggable startPointDraggable;
public:
	//constructor and destructor
	GranularSampler();
	~GranularSampler();
	//runtime events
	void setup(Sample & _sample, int n);
	void controlUpdate();
	void draw();
	//user interaction listeners
	bool mouseMoved(int x, int y);
	bool mouseDragged(int x, int y, int button);
	bool mousePressed(int x, int y, int button);
	void retriggerEnvelope();
	bool mouseReleased(int x, int y, int button);
	//wave memory
	float requestNextBakedSample(unsigned int);

	Sample sample;
	mutex audioMutex;//avoids small sound loop to be read by two threads at the same time


	long loopStartFrame;
	long loopEndFrame;
	long loopTargetLength=1000;
	long loopMaxLength_frames;

	void setStartFrame(long);
	void setLength(long);

	//draggable endPointDraggable;

	void applyDraggableConstraints();

	vector<double> bakedWave_left;
	vector<double> bakedWaveCopy_left;

	long bakedWaveSize = 0;
	bool updateBakedWave_flag = false;
	int waveHeader = 0;

	int drawingCenter[2];
	int drawingSize = 300;

	long pointStart_frame;
	long pointEnd_frame;

	int myNumber = 0;

	float currentVolume = 0;
	int envPosition = 0;
	int envLength = 300000;
	bool envelopeTriggered = false;

	char myTriggerKey;
	
};

