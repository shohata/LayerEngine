#include "interface.h"

TextAnimation::TextAnimation(MessageLayer *layer, PCTSTR str, int time) : Animation(time){
	targetLayer = (MessageLayer*)layer->Retain();
	text = (PTSTR)malloc(sizeof(TCHAR) * (lstrlen(str) + 1));
	lstrcpy(text, str);
}

TextAnimation::~TextAnimation(){
	Animation::~Animation();
	targetLayer->Release();
	free(text);
}

bool TextAnimation::Next(){
	int textlen;
	PTSTR newtext;

	if(GetKeyDown(VK_RETURN) && GetOnit())
		SetAttRate(1.0);

	textlen = (int)((float)lstrlen(text) * GetAttRate())+1;
	newtext = (PTSTR)malloc(sizeof(TCHAR) * (textlen + 1));
	lstrcpyn(newtext, text, textlen);
	newtext[textlen] = '\0';

	targetLayer->Clear();
	targetLayer->SetText(newtext);
	free(newtext);

	return Animation::Next();
}