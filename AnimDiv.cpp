#include "interface.h"

DivAnimation::DivAnimation(Layer *layer, int time, int during) : Animation(time){
	targetLayer = layer->Retain();
	maxCount = (int)(((float)during)*((float)FPS)/1000.0);
	count = 0;
	x = 0;
	y = 0;
	targetLayer->SetBMP(0, 0);
}

DivAnimation::~DivAnimation(){
	Animation::~Animation();
	targetLayer->Release();
}

bool DivAnimation::Next(){
	count++;
	if(count >= maxCount){
		count = 0;
		x++;
		if(x >= targetLayer->GetBMPXNum()){
			x = 0;
			y++;
			if(y >= targetLayer->GetBMPYNum()){
				x = 0;
				y = 0;
			}
		}
		targetLayer->SetBMP(x, y);
	}

	return Animation::Next();
}
