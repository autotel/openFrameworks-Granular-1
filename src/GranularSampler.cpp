#include "GranularSampler.h"

GranularSampler::GranularSampler()
{
}

void GranularSampler::setup(Sample & _sample, int x, int y)
{

	sample = _sample;
	printf("mypath: %s", sample.myPath);

	loopMaxLength_frames = 10000;

	sample.pointStart_frame = 0;
	sample.pointEnd_frame = min(loopMaxLength_frames, sample.getLength());

	applyDraggableConstraints();
	unique_lock<mutex> lock(audioMutex);
	bakedWave_left = { 0, 0 };
	updateBakedWave_flag = true;

	drawingCenter[0] = x;
	drawingCenter[1] = y;
}

void GranularSampler::controlUpdate() {
	if (updateBakedWave_flag&&sample.pointStart_frame>0) {
		unique_lock<mutex> lock(audioMutex);
		bakedWave_left.clear();
		long waveSize_spls = (sample.pointEnd_frame - sample.pointStart_frame) * 2;
		unsigned int channs = sample.getChannels();
		long startPoint_spl = sample.pointStart_frame * 2;
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
	// draw straigth waveform 
	ofSetColor(0,40,40);
	
	ofTranslate(drawingCenter[0], drawingCenter[1]);
		
		
		//draw original wave
		ofSetColor(0, 50, 50);
		ofBeginShape();
		long displayLength_frames = sample.pointEnd_frame - sample.pointStart_frame;
		unsigned int channs = sample.getChannels();
		for (long a = 0; a < displayLength_frames; a += channs) {
			long a_frame = sample.pointStart_frame + a;
			int rad = sample.getSampleN(a_frame * 2) * (2*drawingSize/3) + drawingSize;
			float th = (2 * PI * a) / displayLength_frames;
			ofVertex(sin(th)*rad, cos(th)*rad);
		}
		ofEndShape();
		ofSetColor(255);
		//draw baked wave

		ofBeginShape();
		int displayLength = bakedWaveCopy_left.size();
		for (int a = 0; a < displayLength; a++) {
			int rad = bakedWaveCopy_left[a] * (2*drawingSize/3) + drawingSize;
			float th = (2 * PI * a) / displayLength;
			ofVertex(sin(th)*rad, cos(th)*rad);
		}
		ofEndShape();

		//draw zero crossings
		//ofCircle(0,0,300);
		ofSetColor(210, 0, 0);
		for (int i = 0; i<sample.zeroxsLeft.size(); i++) {
			if ((sample.zeroxsLeft[i] < sample.pointEnd_frame * sample.getChannels())
				&& (sample.zeroxsLeft[i] > sample.pointStart_frame * sample.getChannels())) {
				//´pendant: no need to recalc rad.
				int rada = drawingSize-7;
				int radb = drawingSize+7;
				float th = (2 * PI * (sample.zeroxsLeft[i] / sample.getChannels() - sample.pointStart_frame)) / bakedWaveSize;
				ofDrawLine(sin(th)*rada, cos(th)*rada, sin(th)*radb, cos(th)*radb);
			}
		}
	ofPopMatrix();
	ofPopStyle();

	startPointDraggable.draw();
	
	
}

void GranularSampler::applyDraggableConstraints() {

	startPointDraggable.position[0] = drawingCenter[0]+drawingSize*sample.pointStart_frame / sample.getLength_frames();
	startPointDraggable.position[1] = drawingCenter[1];
}

void GranularSampler::setStartFrame(long to) {
	//set startpoint
	if (to<sample.getLength_frames())
		sample.pointStart_frame = to;

	long maxEnd = loopMaxLength_frames + sample.pointStart_frame;
	maxEnd = min(maxEnd, sample.getLength()*sample.getChannels());
	if (sample.pointEnd_frame > maxEnd) {
		sample.pointEnd_frame = maxEnd;
	}

	//set endpoint
	long targetEnd = to + loopTargetLength;
	if (targetEnd < sample.getLength_frames())
		sample.pointEnd_frame = targetEnd;
	
	sample.snapPoints();
	updateBakedWave_flag = true;
};

void GranularSampler::setLength(long to) {
	loopTargetLength = to;
	setStartFrame(sample.pointStart_frame);
};

void GranularSampler::mouseMoved(int x, int y) {

	startPointDraggable.onMouseMoved(x, y);
}

void GranularSampler::mouseDragged(int x, int y, int button)
{
	
	int width = ofGetWidth();

	long ssf = ((long)(sample.getLength_frames()*startPointDraggable.position[0]- drawingCenter[0])) / width;

	setStartFrame(ssf);

	applyDraggableConstraints();
}

void GranularSampler::mousePressed(int x, int y, int button)
{

	if (startPointDraggable.onClick()) {
	}
}

void GranularSampler::mouseReleased(int x, int y, int button)
{

	startPointDraggable.onCRelease();
}

float GranularSampler::requestNextBakedSample(unsigned int nChannels)
{
	//unique_lock<mutex> lock(audioMutex);
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
	return ret;
}

GranularSampler::~GranularSampler()
{
}
