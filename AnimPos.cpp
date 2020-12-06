#include "interface.h"

PosAnimation::PosAnimation(Layer *layer, int time, int x, int y) : Animation(time){
	targetLayer = layer->Retain();
	startX = layer->GetX();
	startY = layer->GetY();
	endX = x;
	endY = y;
}

PosAnimation::~PosAnimation(){
	targetLayer->Release();
}

bool PosAnimation::Next(){
	float ps;
	int x,y;

	if(GetKeyDown(VK_RETURN) && GetOnit())
		SetAttRate(1.0);

	ps = GetAttRate();
	x = (int)(((float)startX) + ((float)endX - startX) * ps);
	y = (int)(((float)startY) + ((float)endY - startY) * ps);
	targetLayer->SetLocation(x, y);

	return Animation::Next();
}