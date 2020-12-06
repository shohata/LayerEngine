#include "interface.h"

FeedAnimation::FeedAnimation(Layer *layer, int time, BYTE value) : Animation(time){
	targetLayer = layer->Retain();
	targetValue = value;
	defaultValue = layer->GetOpaque();
}

FeedAnimation::~FeedAnimation(){
	Animation::~Animation();
	targetLayer->Release();
}

bool FeedAnimation::Next(){
	if(GetKeyDown(VK_RETURN) && GetOnit())
		SetAttRate(1.0);

	targetLayer->SetOpaque((BYTE)((float)(defaultValue) + (float)(targetValue - defaultValue) * GetAttRate()));
	
	return Animation::Next();
}