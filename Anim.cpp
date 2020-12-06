#include "interface.h"

Animation::Animation(int time){
	ref = 1;
	timeMill = time;
	counter = 0;
	max = (int)(((float)timeMill)*((float)FPS)/1000.0);
	omit = true;

	if(max <= 0)
		max = 1;

	GetWindows()->GetAnimationManager()->AddAnimation(this);
}

Animation::~Animation(){
	if(ref != 0)
		MessageBox(NULL, TEXT("delete ではなく Release で破棄して下さい。"), TEXT("エラー Animation"), MB_OK);
}

Animation* Animation::Retain(){
	ref+=1;

	return this;
}

bool Animation::Release(){
	ref-=1;

	if(ref == 0){
		delete this;
		return true;
	}
	return false;
}

int Animation::GetTime(){
	return timeMill;
}

void Animation::SetAttRate(float ps){
	if(ps >= 1.0)
		counter = max;
	else
		counter = (int)(((float)max) * ps);
}

float Animation::GetAttRate(){
	return ((float)counter) / ((float)max);
}

void Animation::SetOmit(bool om){
	omit = om;
}

bool Animation::GetOnit(){
	return omit;
}

void Animation::WaitEnd(){
	while(GetAttRate() != 1.0)
		GetWindows()->Run();
}

bool Animation::Next(){
	if(counter >= max){
		counter = max;
		return false;
	}

	counter+=1;
	return true;
}