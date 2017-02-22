#pragma once
class draggable
{
public:
	draggable();
	draggable(float, float);
	draggable(float[]);
	~draggable();
	void onMouseMoved(int, int);
	void onMouseEnter();
	void onMouseLeave();
	bool onClick();
	void onCRelease();
	void onDrag(int, int);
	void draw();
	bool isClicked=false;
	bool isUnderMouse = false;
	float position[2];
	float radius;
};

