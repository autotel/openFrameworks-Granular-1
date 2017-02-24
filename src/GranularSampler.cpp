#include "GranularSampler.h"

GranularSampler::GranularSampler()
{
}

void GranularSampler::setup(Sample & _sample, int n)
{

	myNumber = n;
	sample = _sample;
	printf("mypath: %s", sample.myPath);

	pointStart_frame = 0;
	pointEnd_frame = min((long)1024*n, sample.getLength());

	applyDraggableConstraints();
	unique_lock<mutex> lock(audioMutex);
	bakedWave_left = { 0, 0 };
	updateBakedWave_flag = true;

	drawingCenter[0] = 0;// ofGetWidth() / 2;
	drawingCenter[1] = 0;// ofGetHeight() / 2;
	int triggerKeys[] = { 'q','w', 'e', 'r', 't','y','u', 'i', 'o', 'p','a', 's', 'd', 'f','g','h', 'j', 'k', 'l' };
	myTriggerKey = triggerKeys[n];

}

void GranularSampler::controlUpdate() {
	

	if (updateBakedWave_flag&&pointStart_frame>=0) {
		unique_lock<mutex> lock(audioMutex);
		bakedWave_left.clear();
		long waveSize_spls = (pointEnd_frame - pointStart_frame) * 2;
		unsigned int channs = sample.getChannels();
		long startPoint_spl = pointStart_frame * 2;
		for (long a = 0; a < waveSize_spls; a += channs) {
			long sample_i = a + startPoint_spl;
			double oppoSpl = sample.getSampleN(startPoint_spl + ((a + waveSize_spls / 2) % waveSize_spls));
			double thisSpl = (sample.getSampleN(sample_i) + oppoSpl) / 2;
			bakedWave_left.push_back(thisSpl);
		}
		bakedWaveCopy_left = vector<double>(bakedWave_left);
		bakedWaveSize = bakedWave_left.size();
		updateBakedWave_flag = false;
	}
}

void GranularSampler::draw()
{
	ofPushStyle();
	ofPushMatrix();
	ofNoFill();

	int dh = startPointDraggable.position[0] -drawingCenter[0];
	int dv = startPointDraggable.position[1] - drawingCenter[1];
	float dist = sqrt((dh*dh) + (dv*dv));
	drawingSize = dist;

	ofTranslate(drawingCenter[0], drawingCenter[1]);
		
		if (true||startPointDraggable.isUnderMouse) {

			//draw zero crossings
			/*
			ofSetColor(255, 0, 0,90);
			for (int i = 0; i < sample.zeroxsLeft.size(); i++) {
				if ((sample.zeroxsLeft[i] < pointEnd_frame * sample.getChannels())
					&& (sample.zeroxsLeft[i] > pointStart_frame * sample.getChannels())) {
					//´pendant: no need to recalc rad.
					int rada = drawingSize - 7;
					int radb = drawingSize + 7;
					float th = (2 * PI * (sample.zeroxsLeft[i] / sample.getChannels() - pointStart_frame)) / bakedWaveSize;
					ofDrawLine(sin(th)*rada, cos(th)*rada, sin(th)*radb, cos(th)*radb);
				}
			}*/

			//draw original wave
			ofSetColor(0, 50, 50 , currentVolume * 255);
			ofBeginShape();
			long displayLength_frames = pointEnd_frame - pointStart_frame;
			unsigned int channs = sample.getChannels();
			for (long a = 0; a < displayLength_frames; a += channs) {
				long a_frame = pointStart_frame + a;
				int rad = sample.getSampleN(a_frame * 2) * (2 * drawingSize / 3) + drawingSize;
				float th = (2 * PI * a) / displayLength_frames;
				ofVertex(sin(th)*rad, cos(th)*rad);
			}
			ofEndShape();
			ofSetColor(255, currentVolume * 255);
			//draw baked wave

			ofBeginShape();
			int displayLength = bakedWaveCopy_left.size();
			for (int a = 0; a < displayLength; a++) {
				int rad = bakedWaveCopy_left[a] * (2 * drawingSize / 3) + drawingSize;
				float th = (2 * PI * a) / displayLength;
				ofVertex(sin(th)*rad, cos(th)*rad);
			}
			ofEndShape();
			
			
		}
	ofPopMatrix();
	ofPopStyle();

	startPointDraggable.draw();
	ofSetColor(0);
	ofDrawBitmapString(myTriggerKey, startPointDraggable.position[0]-7, 5+startPointDraggable.position[1]);
	
}

void GranularSampler::applyDraggableConstraints() {
	
	//startPointDraggable.position[0] = pointStart_frame / sample.getLength_frames()*ofGetWidth();
	//startPointDraggable.position[1] = drawingCenter[1] + (myNumber)*30;
	//drawingCenter[0] = startPointDraggable.position[0];
	
}

void GranularSampler::setStartFrame(long to) {
	//set startpoint

	
	if (to < sample.getLength_frames() && to >= 0)
		pointStart_frame = to;
	long maxEnd = loopMaxLength_frames + pointStart_frame;
	maxEnd = min(maxEnd, sample.getLength()*sample.getChannels());
	if (pointEnd_frame > maxEnd) {
		pointEnd_frame = maxEnd;
	}
	
	//set endpoint

	long targetEnd = to + loopTargetLength;
	if (targetEnd < sample.getLength_frames())
		pointEnd_frame = targetEnd;
	
	sample.snapPoints(pointStart_frame, pointEnd_frame);
	
	updateBakedWave_flag = true;
};

void GranularSampler::setLength(long to) {
	loopTargetLength = to;
	//setStartFrame(pointStart_frame);
};

bool GranularSampler::mouseMoved(int x, int y) {

	startPointDraggable.onMouseMoved(x, y);
	return startPointDraggable.isClicked;
}

bool GranularSampler::mouseDragged(int x, int y, int button)
{
	if (startPointDraggable.isClicked) {
		
		int dh = x - drawingCenter[0];
		int dv = y - drawingCenter[1];
		float dist = sqrt((dh*dh) + (dv*dv));
		float factor = dist / INTERFACE_RADIUS;

		if (factor > 1)
			factor = 1;

		//long ssf = abs(dh*sample.getLength_frames()/ INTERFACE_RADIUS);
		//set starting point to radial position 
		long ssf = ((1+atan2(dh, dv) / (PI))*sample.getLength_frames())/2;
		


		setLength(factor*sample.getLength_frames()/10);

		setStartFrame(ssf);

		applyDraggableConstraints();


	}
	return startPointDraggable.isClicked;
}

bool GranularSampler::mousePressed(int x, int y, int button)
{
	bool ret = startPointDraggable.onClick();

	if (startPointDraggable.isClicked) {
		retriggerEnvelope();
	}
	return ret;
}
void GranularSampler::retriggerEnvelope() {
	envelopeTriggered = true;
	envPosition = 0;
}

bool GranularSampler::mouseReleased(int x, int y, int button)
{
	bool ret = startPointDraggable.isClicked;
	startPointDraggable.onCRelease();
	return ret;
}

float GranularSampler::requestNextBakedSample(unsigned int nChannels)
{
	unique_lock<mutex> lock(audioMutex);

	if (envelopeTriggered) {
		envPosition++;
		currentVolume = ((float)envLength-envPosition)/envLength;
	}
	if (envPosition > envLength) {
		envelopeTriggered = false;
	}

	float ret = 0;
	if (bakedWaveSize > 0) {
		waveHeader = waveHeader%bakedWaveSize;
		ret = bakedWave_left[waveHeader];
		waveHeader++;
	}
	else {
		waveHeader++;
		//ret = cos(waveHeader / 1024) / 2;
	}
	return ret*currentVolume;
}

GranularSampler::~GranularSampler()
{
}
