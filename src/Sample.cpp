#include "Sample.h"
#include "ofMain.h"


Sample::~Sample()
{
    //delete myPath;
    delete myData;
    myChunkSize = NULL;
    mySubChunk1Size = NULL;
    myFormat = NULL;
    myChannels = NULL;
    mySampleRate = NULL;
    myByteRate = NULL;
    myBlockAlign = NULL;
    myBitsPerSample = NULL;
    myDataSize = NULL;
}

// empty constructor
Sample::Sample()
{
	position = 0;
	speed = 1.0;
	soundStatus = NONE;
}

// constructor takes a wav path
Sample::Sample(string tmpPath)
{
    position = 0;
    speed = 1.0;
    soundStatus = NONE;

	//pointStart_frame = 0;

    myPath = tmpPath;
    read();
}

void Sample::setPath(string tmpPath)
{
    myPath = tmpPath;

}
/*
void Sample::setLooping(bool loop)
{
    if(loop) {
        soundStatus |= LOOPING;
		if (pointEnd_frame == 0)
			pointEnd_frame = getLength()/myChannels;
    }
    else {
        soundStatus &= ~LOOPING;
    }
}*/

bool Sample::getIsLooping()
{
    if(soundStatus & LOOPING) return true;
    else return false;
}

bool Sample::getIsLoaded()
{
    if(soundStatus & LOADED) return true;
    else return false;
}

bool Sample::getIsPlaying()
{
    if(soundStatus & PLAYING) return true;
    else return false;
}

bool Sample::getIsPaused()
{
    if(soundStatus & PAUSED) return true;
    else return false;
}

bool Sample::load(string tmpPath) {
    myPath = tmpPath;
	bool result = read();
	return result;
}
/*
void Sample::returnSamples(vector<float> * _sample) {
	_sample->clear();
	//pendant: I want to try mutex instead of pausing/playing later. Does it make sense?
	bool loopState = getIsLooping();
	setLooping(false);
	bool playState = getIsPlaying();
	double tmpSpeed = getSpeed();
	setSpeed(1.0f);
	play();
	double sampleL, sampleR;
	while ((long)position<getLength()) {
		float mm=0;
		_sample->push_back(update());
		position++;
	}
	position = 0;
	setLooping(loopState);
	setSpeed(tmpSpeed);
	if (playState) play();
}*/
void Sample::calculateZeroxs() {
	double currentSample = 0;
	double lastSample = 0;
	unsigned int chans = getChannels();
	for (long a = 0; a < getLength(); a += chans) {
		long a_frame = a/chans;
		currentSample = getSampleN(a);
		//pendant: faster positive zero cross calculation
		if (((currentSample - lastSample > 0) && (currentSample > 0) && (lastSample < 0))||(currentSample==0)) {
			zeroxsLeft.push_back(a_frame * chans);
			//printf("zerox @ %lld\n", a_frame*chans);
		}
		lastSample = currentSample;
	}
}

void Sample::snapPoints(long&pointStart_frame, long&pointEnd_frame) {

	long nearest_index_st = 30;
	long nearest_index_end = 30;

	long pointEnd_spl = pointEnd_frame * 2;
	long pointStart_spl =pointStart_frame * 2;

	for (int a = 0; a < zeroxsLeft.size(); a++) {
		//snap pointStart to nearest zero crossing in sample position
		if (abs((long)zeroxsLeft[a] - pointStart_spl) < abs((long)zeroxsLeft[nearest_index_st] - pointStart_spl)) {
			nearest_index_st = a;
		}
		//same for the pointEnd
		if (abs((long)zeroxsLeft[a] - pointEnd_spl) < abs((long)zeroxsLeft[nearest_index_end] - pointEnd_spl)) {
			nearest_index_end = a;
		}
	}
	/*int startZerox = nearest_index_st;
	int endZerox = nearest_index_end;*/
	if (nearest_index_st > 0 && nearest_index_st < zeroxsLeft.size()) {
		pointStart_frame = zeroxsLeft[nearest_index_st] / myChannels;
		pointEnd_frame = zeroxsLeft[nearest_index_end] / myChannels;
	}
}

void Sample::generateWaveForm(vector<MiniMaxima> * _waveForm)
{

	_waveForm->clear();

	//bool loopState = getIsLooping();
	//setLooping(false);
	//bool playState = getIsPlaying();
	//double tmpSpeed = getSpeed();
	iterate_start();
	//pendant: this is overhead, the iterator fnc iterate_next() should not vary according to the playback speed
	setSpeed(1.0f);
    play();
	// waveform display method based on this discussion http://answers.google.com/answers/threadview/id/66003.html

    double sampleL, sampleR;
	while ((long)position<getLength()) {

		MiniMaxima mm;
		mm.minL = mm.minR = mm.maxL = mm.maxR = 0;

		for (int i = 0; i < 256; i++){

		    if(myChannels == 1) {
                sampleL = iterate_next();
                sampleR = sampleL;
		    }
		    else {
                sampleL = iterate_next()*0.5;
                sampleR = iterate_next()*0.5;
		    }

			mm.minL = MIN(mm.minL, sampleL);
			mm.minR = MIN(mm.minR, sampleR);
			mm.maxL = MAX(mm.maxL, sampleL);
			mm.maxR = MAX(mm.maxR, sampleR);

		}

		_waveForm->push_back(mm);

 		//cout << mm.minR << " :: " << mm.maxR << " :: " << mm.minL << " :: " << mm.maxL << endl;
	}

	//position = 0;
	//setLooping(loopState);
	//setSpeed(tmpSpeed);
	//if(playState) play();
}

void Sample::drawWaveForm(int _x, int _y, int _w, int _h, vector<MiniMaxima> * _waveForm)
{
	//ofSetColor(0, 255, 0);
	float waveFormZoomX = (float)_waveForm->size()/(float)_w;

	glPushMatrix();

	glTranslated(_x, _y-_h, 0);

	for (unsigned int i = 1; i < _waveForm->size(); i++){
	    /*if(myChannels == 1) {*/
            ofLine((i-1)/waveFormZoomX, _h + (int)(_waveForm->at(i-1).minR*_h), i/waveFormZoomX, _h +  (int)(_waveForm->at(i).maxR*_h));
            ofLine(i/waveFormZoomX, _h + (int)(_waveForm->at(i).maxR*_h), i/waveFormZoomX, _h + (int) (_waveForm->at(i).minR*_h));
	    /*} else {
            ofLine((i-1)/waveFormZoomX, (int)(_waveForm->at(i-1).minL*_h), i/waveFormZoomX, (int)(_waveForm->at(i).maxL*_h));
            ofLine(i/waveFormZoomX, (int)(_waveForm->at(i).maxL*_h), i/waveFormZoomX, (int) (_waveForm->at(i).minL*_h));

            ofLine((i-1)/waveFormZoomX, _h + (int)(_waveForm->at(i-1).minR*_h), i/waveFormZoomX, _h +  (int)(_waveForm->at(i).maxR*_h));
            ofLine(i/waveFormZoomX, _h + (int)(_waveForm->at(i).maxR*_h), i/waveFormZoomX, _h + (int) (_waveForm->at(i).minR*_h));
	    }*/
	}

	

	float waveFormDisplayScale = getLength()/_w;
	/*
    if(myChannels == 1) {
        ofLine(position/waveFormDisplayScale, -(float)_h*0.0, position/waveFormDisplayScale, (float)_h*2.0);
    }
    else
    {
        ofLine(position/waveFormDisplayScale, -(float)_h*0.5, position/waveFormDisplayScale, (float)_h*1.5);
    }*/
	ofNoFill();
	//ofDrawRectangle(2*pointStart_frame/waveFormDisplayScale, -(float)_h*0.5, 2 * (pointEnd_frame-pointStart_frame) / waveFormDisplayScale, (float)_h*2.0);
	glPopMatrix();
}

void Sample::stop()
{
    position = 0;
    soundStatus &= ~PAUSED;
    soundStatus &= ~PLAYING;
}

void Sample::play()
{
    if(speed > 0)
        position = 0;
    else
        position = getLength();
    soundStatus |= PLAYING;
}

void Sample::setPaused(bool bPaused)
{
    if(bPaused) {
        soundStatus |= PAUSED;
    }
    else {
        soundStatus &= ~PAUSED;
    }
}

void Sample::setSpeed(double spd)
{
    speed = spd;
}

double Sample::getSpeed()
{
    return speed;
}

int Sample::getSampleRate()
{
    return mySampleRate;
}
void Sample::iterate_start() {
	position = 0;
};
double Sample::iterate_next()
{
	long length = getLength();
	double remainder;
	short* buffer = (short *)myData;

	position = (position + speed);
	remainder = position - (long)position;

	// check if reached EOF
	if ((long)position>length) {
		soundStatus &= ~PLAYING;
		return 0;
	}

	output = (double)((1.0 - remainder) * buffer[1 + (long)position] + remainder * buffer[2 + (long)position]) / 32767.0;//linear interpolation

	return(output);
}
/*
double Sample::update()
{
    if(!(soundStatus & PLAYING)) return 0;
    if(soundStatus & PAUSED) return 0;
    if(!getIsLoaded()) return 0;

    long length=getLength();
	double remainder;
	short* buffer = (short *)myData;

	position=(position+speed);
	remainder = position - (long) position;

    // check if reached EOF
	if (((long) position>length)|| ((long)position>(pointEnd_frame*myChannels))) {
	    if(getIsLooping()) {
            position=pointStart_frame*myChannels;
	    }
        else {
            soundStatus &= ~PLAYING;
            return 0;
	    }
	}

    //check if position less than pointStart_frame (reverse)
	if ((long) position < pointStart_frame*myChannels) {
	    if(getIsLooping()) {
            position = pointEnd_frame*myChannels;
	    }
        else {
            soundStatus &= ~PLAYING;
            return 0;
	    }
	}
	//if(getLength()>= 2 + (long)position)
	output = (double)((1.0 - remainder) * buffer[1 + (long)position] + remainder * buffer[2 + (long)position]) / 32767.0;//linear interpolation

	return(output);
}
*/
double Sample::getSampleN(long pos) {

	short* buffer = (short *)myData;
	long remainder = pos - (long)pos;
	//if (soundStatus&LOADED) {
		output = (double)((1.0 - remainder) * buffer[1 + (long)pos] + remainder * buffer[2 + (long)pos]) / 32767.0;//linear interpolation
		return(output);
	//}
	//else { return(0); }

}

//double Sample::stretch(double pitch, double frequency) {
//	short* buffer = (short *)myData;
//
//
//
//}


long Sample::getLength()
{

		long length;
		length = myDataSize*0.5;
		return(length);
		return 0;

}
long Sample::getLength_frames()
{

		long length;
		length = myDataSize*0.5;
		if (myChannels != 0) {
			return(length / myChannels);
		}
		return 0;
}
double Sample::getPosition()
{
    double pos = position/getLength();
    pos = ofClamp(pos,0.0,1.0);
	return pos;
}
void Sample::setPosition(double _position)
{
	double pct = ofClamp(_position,0.0,1.0);
	position = pct * getLength();
}


// return a printable summary of the wav file
char* Sample::getSummary()
{
    char *summary = new char[250];
    sprintf(summary, " Format: %d\n Channels: %d\n SampleRate: %d\n ByteRate: %d\n BlockAlign: %d\n BitsPerSample: %d\n DataSize: %d\n", myFormat, myChannels, mySampleRate, myByteRate, myBlockAlign, myBitsPerSample, myDataSize);
    std::cout << myDataSize;
    return summary;
}

int Sample::getChannels()
{
    return myChannels;
}

// write out the wav file
bool Sample::save()
{
    ofToDataPath(myPath);
    fstream myFile (myPath.c_str(), ios::out | ios::binary);

    // write the wav file per the wav file format
    myFile.seekp (0, ios::beg);
    myFile.write ("RIFF", 4);
    myFile.write ((char*) &myChunkSize, 4);
    myFile.write ("WAVE", 4);
    myFile.write ("fmt ", 4);
    myFile.write ((char*) &mySubChunk1Size, 4);
    myFile.write ((char*) &myFormat, 2);
    myFile.write ((char*) &myChannels, 2);
    myFile.write ((char*) &mySampleRate, 4);
    myFile.write ((char*) &myByteRate, 4);
    myFile.write ((char*) &myBlockAlign, 2);
    myFile.write ((char*) &myBitsPerSample, 2);
    myFile.write ("data", 4);
    myFile.write ((char*) &myDataSize, 4);
    myFile.write (myData, myDataSize);

    return true;
}



// read a wav file into this class
bool Sample::read()
{
    myPath = ofToDataPath(myPath,true).c_str();
    ifstream inFile( myPath.c_str(), ios::in | ios::binary);

    ofLog(OF_LOG_NOTICE, "Reading file %s",myPath.c_str());

    if(!inFile.is_open())
    {
        ofLog(OF_LOG_ERROR,"Error opening file. File not loaded.");
        return false;
    }

    char id[4];
    inFile.read((char*) &id, 4);
    if(strncmp(id,"RIFF",4) != 0)
    {
        ofLog(OF_LOG_ERROR,"Error reading sample file. File is not a RIFF (.wav) file");
        return false;
    }

    inFile.seekg(4, ios::beg);
    inFile.read( (char*) &myChunkSize, 4 ); // read the ChunkSize

    inFile.seekg(16, ios::beg);
    inFile.read( (char*) &mySubChunk1Size, 4 ); // read the SubChunk1Size

    //inFile.seekg(20, ios::beg);
    inFile.read( (char*) &myFormat, sizeof(short) ); // read the file format.  This should be 1 for PCM
    if(myFormat != 1) {
        ofLog(OF_LOG_ERROR, "File format should be PCM, sample file failed to load.");
        return false;
    }

    //inFile.seekg(22, ios::beg);
    inFile.read( (char*) &myChannels, sizeof(short) ); // read the # of channels (1 or 2)

    //inFile.seekg(24, ios::beg);
    inFile.read( (char*) &mySampleRate, sizeof(int) ); // read the Samplerate

    //inFile.seekg(28, ios::beg);
    inFile.read( (char*) &myByteRate, sizeof(int) ); // read the byterate

    //inFile.seekg(32, ios::beg);
    inFile.read( (char*) &myBlockAlign, sizeof(short) ); // read the blockalign

    //inFile.seekg(34, ios::beg);
    inFile.read( (char*) &myBitsPerSample, sizeof(short) ); // read the bitsperSample

    inFile.seekg(40, ios::beg);
    inFile.read( (char*) &myDataSize, sizeof(int) ); // read the size of the data
    //cout << myDataSize << endl;

    // read the data chunk
    myData = new char[myDataSize];
    inFile.seekg(44, ios::beg);
    inFile.read(myData, myDataSize);

    inFile.close(); // close the input file

    soundStatus |= LOADED;
	isLoaded = true;

	calculateZeroxs();

    return true; // this should probably be something more descriptive
}
