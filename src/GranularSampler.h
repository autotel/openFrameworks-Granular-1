#pragma once

#include "ofMain.h"
#include "GranularSampler.h"
#include "draggable.h"
#include "Sample.h"

class GranularSampler
{
private:
public:
	//constructor and destructor
	GranularSampler();
	~GranularSampler();
	//runtime events
	void setup(string path,int x,int y);
	void controlUpdate();
	void draw();
	//user interaction listeners
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	//wave memory
	float requestNextBakedSample(unsigned int);
	vector<MiniMaxima> waveForm;

	Sample sample;
	mutex audioMutex;//avoids small sound loop to be read by two threads at the same time


	long loopStartFrame;
	long loopEndFrame;
	long loopTargetLength=1000;
	long loopMaxLength_frames;

	void setStartFrame(long);
	void setLength(long);

	draggable startPointDraggable;
	//draggable endPointDraggable;

	void applyDraggableConstraints();

	vector<double> bakedWave_left;
	vector<double> bakedWaveCopy_left;

	long bakedWaveSize = 0;
	bool updateBakedWave_flag = false;
	int waveHeader = 0;

	int drawingCenter[2];
	int drawingSize = 300;
	
};

