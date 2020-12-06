#include "interface.h"

ZoomAnimation::ZoomAnimation(Layer *layer, int time, int w, int h) : Animation(time){
	targetLayer = layer->Retain();
	startW = layer->GetWidth();
	startH = layer->GetHeight();
	endW = w;
	endH = h;
}

ZoomAnimation::~ZoomAnimation(){
	targetLayer->Release();
}

bool ZoomAnimation::Next(){
	float ps;
	int w,h;

	if(GetKeyDown(VK_RETURN) && GetOnit())
		SetAttRate(1.0);

	ps = GetAttRate();
	w = (int)(((float)startW) + ((float)endW - startW) * ps);
	h = (int)(((float)startH) + ((float)endH - startH) * ps);
	targetLayer->SetSize(w, h);

	return Animation::Next();
}