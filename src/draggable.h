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
	void onClick();
	void onCRelease();
	void onDrag(int, int);
	void draw();
	bool isClicked;
	bool isUnderMouse;
	float position[2];
	float radius;
};

