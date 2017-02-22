#include "draggable.h"
#include "ofApp.h"


draggable::draggable()
{
	position[0] = 0;
	position[1] = 0;
}

draggable::draggable(float x, float y)
{
	position[0] = x;
	position[1] = y;
}

draggable::draggable(float pos [])
{
	position[0] = pos[0];
	position[1] = pos[1];
}



draggable::~draggable()
{
}

void draggable::onMouseMoved(int x, int y) {
	if ((abs(position[0] - x) > radius) && (abs(position[0] - y) > radius)) {
		onMouseEnter();
		if (isUnderMouse) { 
			onDrag(x, y);
		}
	}
	else if(isUnderMouse){
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

void draggable::onClick()
{
	if (isUnderMouse) {
		isClicked = true;
	}
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
	//pendant: draw instead should be a pointer so the user can define it's own drawing procedure.
	ofDrawCircle(position[0], position[1], radius);

}
