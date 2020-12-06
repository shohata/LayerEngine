#include "interface.h"

AnimationManager::AnimationManager(){
	animCount = 0;
	animObject[0] = NULL;
	animObject[1] = NULL;
	animObject[2] = NULL;
}

AnimationManager::~AnimationManager(){
	for(int i=0; i<animCount; i++)
		animObject[i]->Release();
}

int AnimationManager::AddAnimation(Animation *animation){
	if (animCount == MAX_ANIM) {
		/*エラー処理*/
		return -1;
	}
	else animObject[animCount] = animation->Retain();
	animCount++;

	return animCount;
}

bool AnimationManager::RemoveAtAnimation(int index){
	int i;

	if (index >= MAX_ANIM || !animObject[index]) {
		/*エラー処理*/
		return false;
	}
	animObject[index]->Release();
	animCount--;

	for(i = index ; animObject[i] ; i++) {
		if (i == MAX_ANIM - 1) {
			animObject[i] = NULL;
			break;
		}
		animObject[i] = animObject[i + 1];
	}

	return true;
}

int AnimationManager::GetAnimationLength(){
	return animCount;
}

Animation* AnimationManager::GetAnimation(int index){
	return animObject[index];
}
