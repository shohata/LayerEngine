#include "interface.h"

LinkAnimation::LinkAnimation(LINK **link, int count) : Animation(1000){
	linkCount = count;
	decide = false;
	rect = (RECT*)malloc(sizeof(RECT) * count);
	targetLayer = (Layer**)malloc(sizeof(Layer*) * count);

	for(int i=0; i<count; i++){
		linkIndex = i;
		targetLayer[i] = ((*link)[i]).own->Retain();

		if(((*link)[i]).up)
			rect[i].top = GetIndex(((*link)[i]).up, link);
		else
			rect[i].top = i;
		if(((*link)[i]).down)
			rect[i].bottom = GetIndex(((*link)[i]).down, link);
		else
			rect[i].bottom = i;
		if(((*link)[i]).right)
			rect[i].right = GetIndex(((*link)[i]).right, link);
		else
			rect[i].right = i;
		if(((*link)[i]).left)
			rect[i].left = GetIndex(((*link)[i]).left, link);
		else
			rect[i].left = i;
	}

	linkIndex = 0;
}

LinkAnimation::~LinkAnimation(){
	Animation::~Animation();
	for(int i=0; i<linkCount; i++){
		targetLayer[i]->Release();
	}
	free((void*)targetLayer);
	free((void*)rect);
}

bool LinkAnimation::Next(){
	SetAttRate(0.0);

	//キー入力
	if(GetKeyDown(VK_UP)){
		targetLayer[linkIndex]->SetLinkEnable(false);
		linkIndex = rect[linkIndex].top;
	}
	if(GetKeyDown(VK_DOWN)){
		targetLayer[linkIndex]->SetLinkEnable(false);
		linkIndex = rect[linkIndex].bottom;	
	}
	if(GetKeyDown(VK_RIGHT)){
		targetLayer[linkIndex]->SetLinkEnable(false);
		linkIndex = rect[linkIndex].right;	
	}
	if(GetKeyDown(VK_LEFT)){
		targetLayer[linkIndex]->SetLinkEnable(false);
		linkIndex = rect[linkIndex].left;	
	}
	if(GetKeyDown(VK_RETURN)){
		decide = true;
		Update();
		SetAttRate(1.0);
	}

	//マウス入力
	int x, y;

	GetMousePoint(&x, &y);
	JudgeLayer(x, y);
	if(GetMouseInput(MOUSE_LEFT_JUSTDOWN)){
		decide = true;
		UpdateMouse();
		SetAttRate(1.0);
	}

	targetLayer[linkIndex]->SetLinkEnable(true);

	return Animation::Next();
}


Layer* LinkAnimation::GetDecideLayer(){
	if(decide == true)
		return targetLayer[linkIndex];

	return NULL;
}

int LinkAnimation::GetDecideIndex(){
	if(decide == true)
		return linkIndex;

	return NOTHING;
}

int LinkAnimation::GetLinkLength(){
	return linkCount;
}

Layer* LinkAnimation::GetLayer(int index){
	return targetLayer[index];
}

int LinkAnimation::GetIndex(Layer *layer, LINK** link){
	for(int i=0; i<linkCount; i++){
		if(((*link)[i]).own == layer)
			return i;
	}

#ifdef _DEBUG
	MessageBox(NULL, TEXT("LINK構造体の引数が誤っています。"), TEXT("エラー　LinkAnimation"), MB_OK);
#endif

	return linkIndex;
}

void LinkAnimation::JudgeLayer(int x, int y){
	for(int i=0; i<linkCount; i++){
		int lx = targetLayer[i]->GetX();
		int ly = targetLayer[i]->GetY();
		int lx1 = lx + targetLayer[i]->GetWidth();
		int ly1 = ly + targetLayer[i]->GetHeight();

		if(x >= lx && x <= lx1 && y >= ly && y <= ly1){
			targetLayer[linkIndex]->SetLinkEnable(false);
			linkIndex = i;
			return;
		}
	}
}