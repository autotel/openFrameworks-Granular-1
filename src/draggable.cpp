#include "draggable.h"
#include "ofApp.h"


draggable::draggable()
{
	position[0] = 0;
	position[1] = 0;
	radius = 10;
	isClicked = false;
}

draggable::draggable(float x, float y)
{
	position[0] = x;
	position[1] = y;
	radius = 10;
	isClicked = false;
}

draggable::draggable(float pos [])
{
	position[0] = pos[0];
	position[1] = pos[1];
	radius = 10;
	isClicked = false;
}



draggable::~draggable()
{
}

void draggable::onMouseMoved(int x, int y) {
	if (isClicked) {
		onDrag(x, y);
	}

	if ((abs(position[0] - x) < radius) && (abs(position[1] - y) < radius)) {
		onMouseEnter();
	}else if(isUnderMouse){
		onMouseLeave();
	}
}

void draggable::onMouseEnter()
{
	isUnderMouse = true;
}

void draggable::onMouseLeave()
{
	isUnderMouse = false;
}

bool draggable::onClick()
{
	if (isUnderMouse) {
		isClicked = true;
	}
	return isClicked;
}

void draggable::onCRelease() {
	isClicked = false;
}

void draggable::onDrag(int x, int y)
{
	position[0] = x;
	position[1] = y;
}

void draggable::draw()
{
	ofPushStyle();
	if (isClicked) {
		ofSetColor(255);
	}
	else if (isUnderMouse) {
		ofSetColor(198);
	} else  {
		ofSetColor(64);
	}
	

	//pendant: draw instead should be a pointer so the user can define it's own drawing procedure.
	ofDrawCircle(position[0], position[1], radius);
	ofPopStyle();

}
