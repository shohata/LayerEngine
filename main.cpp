#include "interface.h"

#define MOJI		100
#define LINK_COUNT	2

void CreateRect(Layer *layer,COLORREF color,BYTE value);
void Text(MessageLayer *layer,PCTSTR text);
void Wait(DWORD time);
void LightningBall(int x, int y, int way);
int Link(MessageLayer *layer, PTSTR **text, int count);
void Game1();//ラブレター
void Game2();//瞬間視
void Game3();//速読

GAMEMAIN int GameMain(Windows *win){
	CREATESTRUCT *cs;
	
	cs = win->GetWindows();
	cs->cx = 834;
	cs->cy = 678;
	if(!win->CreateWindows())
		return FALSE;

	Game1();

	while(TRUE)
		win->Run();

	return 0;
}

void CreateRect(Layer *layer,COLORREF color,BYTE value)
{
	RECT rect;
	HBRUSH hBrush;
	HDC hdc;

	layer->SetOpaque(value);

	rect.top = 0;
	rect.left = 0;
	rect.right = layer->GetWidth();
	rect.bottom = layer->GetHeight();

	hBrush = CreateSolidBrush(color);

	hdc = layer->GetDC();
	FillRect(hdc, &rect, hBrush);
	layer->DeleteDC(hdc);

	DeleteObject(hBrush);
}

void Text(MessageLayer *layer, PCTSTR text){
	TextAnimation* anim;

	layer->SetVisible(true);
	anim = new TextAnimation(layer, text, lstrlen(text) * MOJI);
	anim->WaitEnd();
	while(TRUE){
		if(GetKeyDown(VK_RETURN))
			break;
		if(GetMouseInput(MOUSE_LEFT_JUSTDOWN))
			break;
			
		GetWindows()->Run();
	}
	anim->Release();
	Update();
	layer->SetVisible(false);
}

void Wait(DWORD time){
	DWORD beginTime = timeGetTime();

	while((timeGetTime()-beginTime) < time)
		GetWindows()->Run();
}

void LightningBall(int x, int y, int way){
	Layer *light;
	Animation *anim, *anim2;
	int time;

	light = new Layer(x, y, 0, 0);
	light->SetTransColor(RGB(0,0,0));
	light->SetTransEnable(true);
	light->LoadDivBMP(TEXT("Img\\lightning_ball.bmp"), 6, 1);
	
	switch(way){
		case 0:
			time = y * 3;
			anim2 = new PosAnimation(light, time, x, 0);
			break;
		case 1:
			time = (640-x) * 3;
			anim2 = new PosAnimation(light, time,640, y);
			break;
		case 2:
			time = (480-y) * 3;
			anim2 = new PosAnimation(light, time, x, 480);
			break;
		case 3:
			time = x * 3;
			anim2 = new PosAnimation(light, time, 0, y);
			break;
	}
	anim2->SetOmit(false);
	anim = new DivAnimation(light, time, 50);
	anim->SetOmit(false);

	anim->WaitEnd();
	light->Release();
	anim->Release();
	anim2->Release();
}

int Link(MessageLayer *layer, PTSTR **text, int count){
	int x, y, w, h;
	int r;
	LinkAnimation *anim;
	LINK *link;
	MessageLayer **list;

	x = layer->GetX() + layer->GetMarginX();
	y = layer->GetY() + layer->GetMarginY();
	w = layer->GetWidth() - 2 * layer->GetMarginX();
	h = (layer->GetHeight() - 2 * layer->GetMarginY()) / 3;

	link = (LINK*)malloc(sizeof(LINK) * count);
	ZeroMemory(link, sizeof(LINK) * count);
	list = (MessageLayer**)malloc(sizeof(MessageLayer*) * count);

	for(int i=0; i<count; i++){
		list[i] = new MessageLayer(x, y, w, h);
		CreateRect(list[i], RGB(0, 0, 0), 255);
		(*list[i]).SetTransEnable(true);
		(*list[i]).SetTransColor(RGB(0, 0, 0));
		(*list[i]).SetText((*text)[i]);
		y += h;
	}

	for(int i=0; i<count; i++){
		link[i].own = list[i];
		if((i+1) < count)
			link[i].down = list[i+1];
		if((i-1) >= 0)
			link[i].up = list[i-1];
	}

	anim = new LinkAnimation(&link, count);
	anim->WaitEnd();
	r = anim->GetDecideIndex();
	anim->Release();

	for(int i=0; i<count; i++)
		(*list[i]).Release();

	free(list);
	free(link);

	return r;
}

void Game1(){
	MessageLayer* text;
	Layer *layer_map;
	Map *map, *map2;
	MainChara *chara1;
	Chara *chara2;
	
	//初期化
	layer_map = new Layer(0, 0, 640, 480);
	map = new Map(layer_map, TEXT("Img\\mapdat.txt"), TEXT("Img\\map3.bmp"));
	map2 = new Map(layer_map, TEXT("Img\\mapdat2.txt"), TEXT("Img\\map3.bmp"));
	map->SetMapLink(18, 0, map2, 18, 14);
	map2->SetMapLink(18, 14, map, 18, 0);
	map->SetVisible(true);
	map->DrawMap();

	chara1 = new MainChara(TEXT("Img\\char.bmp"), 64, 64, 3, map);
	chara2 = new Chara(TEXT("Img\\char2.bmp"), 160, 160, 3, map);

	text = new MessageLayer(64, 380, 600, 100);
	text->LoadBMP(TEXT("Img\\TextFrame.bmp"));
	text->SetMargin(20, 13);
	text->SetTransColor(RGB(255, 255, 255));
	text->SetTransEnable(true);
	text->SetVisible(false);

	//本編
	while(!(chara1->GetX() == 128 && chara1->GetY() == 160))
		GetWindows()->Run();

	chara1->SetAuto(true);
	chara1->SetWay(1);
	Text(text, TEXT("あの・・・・・\nこのラブレター受け取って下さい。\nお願いします。"));
	Text(text, TEXT("誰が受け取るかよっ"));
	chara2->MoveChara(1, 544);
	Text(text, TEXT("ああっ・・・・・     \n僕の青春は、終わりました・・・"));
	chara1->SetAuto(false);

	while(!(chara1->GetX() == 544 && chara1->GetY() == 160))
		GetWindows()->Run();

	chara1->SetAuto(true);
	chara1->SetWay(1);
	Text(text, TEXT("しつこいんだよっ"));
	chara2->SetWay(3);
	Text(text, TEXT("死ねーーーーー\nライトニングボール！！"));
	LightningBall(530, 130, 3);
	Text(text, TEXT("\n\tライトニングボールを覚えた！！"));
	chara1->SetAuto(false);
	Text(text, TEXT("まあ、いいだろう。ちょうどいいハンデだ。"));
	chara2->MoveChara(0, 160);

	while(!(chara2->GetX() == 18*32 && chara2->GetY() == 14*32))
		GetWindows()->Run();
	chara2->SetLocation(17*32, 9*32);

	while(TRUE){
		WaitKeyDown(VK_RETURN);//WaitMouseInput(MOUSE_LEFT_JUSTDOWN);
		LightningBall(chara1->GetX()-16, chara1->GetY()-32, chara1->GetWay());
		GetWindows()->Run();
	}

	text->Release();
	layer_map->Release();
}

void Game2(){
	MessageLayer* layer1, *layer2, *list, *text;
	LinkAnimation *anim;
	LINK *link;
	PTSTR *str;
	int index;

	layer1 = new MessageLayer(100, 200, 440, 80);
	CreateRect(layer1, RGB(128, 0, 0), 255);
	layer1->SetText(TEXT("\tデジタルノート"));
	
	layer2 = new MessageLayer(100, 300, 440, 80);
	CreateRect(layer2, RGB(0, 128, 0), 255);
	layer2->SetText(TEXT("\tＣ言語"));

	text = new MessageLayer(64, 500, 600, 100);
	text->LoadBMP(TEXT("Img\\TextFrame.bmp"));
	text->SetMargin(20, 13);
	text->SetTransColor(RGB(255, 255, 255));
	text->SetTransEnable(true);
	text->SetVisible(false);

	link = (LINK*)malloc(sizeof(LINK) * LINK_COUNT);
	ZeroMemory(link, sizeof(LINK) * LINK_COUNT);
	link[0].own = link[1].up   = layer1;
	link[1].own = link[0].down = layer2;

	anim = new LinkAnimation(&link, LINK_COUNT);
	anim->WaitEnd();
	list = (MessageLayer*)anim->GetDecideLayer()->Retain();
	anim->Release();
	free(link);

	TCHAR text1[] = TEXT("後悔");
	TCHAR text2[] = TEXT("無常");
	str = (PTSTR*)malloc(sizeof(PTSTR) * 2);
	str[0] = text1;
	str[1] = text2;
	index = Link(list, &str, 2);
	free(str);

	TCHAR tx[32];
	TCHAR _tx[] = TEXT("あなたは、%sが好きなのですね！！");
	if(index == 0)
		wsprintf(tx, _tx, TEXT("後悔"));
	else
		wsprintf(tx, _tx, TEXT("無常"));

	text->SetVisible(true);
	Text(text, tx);

	while(TRUE)
		GetWindows()->Run();

	layer1->Release();
	layer2->Release();
	list->Release();
}

void Game3(){
	Layer *main, *crs;
	MessageLayer *text;
	PosAnimation *anim;
	float speed[85];
	float kyori = 0.0;
	int time,line = 30;
	TCHAR str[32];

	SIZE size[86] ={{800,0},{800,146},{782,9},{782,130},{766,0},{766,136},{750,0},{750,306},{732,9},{732,370},
					{714,9},{714,602},{698,0},{698,602},{680,0},{680,602},{665,0},{665,602},{647,0},{647,385},
					{629,9},{629,598},{613,0},{613,602},{597,0},{597,602},{578,0},{578,55},{561,0},{561,196},
					{544,9},{544,302},{527,9},{527,70},{511,0},{511,194},{492,9},{492,185},{476,9},{476,427},
					{460,0},{460,602},{443,0},{443,405},{425,0},{425,95},{407,9},{407,139},{391,0},{391,602},
					{374,0},{374,105},{340,9},{340,482},{323,9},{323,587},{306,9},{306,592},{274,9},{274,597},
					{257,0},{257,119},{239,0},{239,242},{221,9},{221,179},{191,9},{191,602},{170,0},{170,90},
					{153,9},{153,512},{137,9},{137,294},{120,0},{120,422},{104,9},{104,207},{85,0},{85,602},
					{68,0},{68,33},{51,9},{51,350},{35,0},{35,275}};

	for(int i=0; i<85; i++){
		int x = size[i+1].cx - size[i].cx;
		int y = size[i+1].cy - size[i].cy;
		speed[i] = sqrt((float)(x*x + y*y));
		kyori += speed[i];
	}

	main = new Layer(0, 0, 832, 640);
	main->LoadBMP(TEXT("Img\\都会のトム＆ソーヤ.bmp"));
	crs = new Layer(size[0].cx, size[0].cy, 16, 16);
	CreateRect(crs, RGB(0, 255, 0), 128);
	text = new MessageLayer(0, 620, 832, 20);
	CreateRect(text, RGB(255, 255, 255), 255);
	text->SetTextColor(RGB(0, 0, 0));
	text->SetShadowColor(RGB(128, 128, 128));
	text->SetText(TEXT("読書スピード　10秒に30行"));

	while(TRUE){
		while(!GetKeyDown(VK_RETURN)){
			if(GetKeyDown(VK_UP))
				line++;
			if(GetKeyDown(VK_DOWN))
				line--;
			wsprintf(str, TEXT("読書スピード　10秒に%d行"), line);
			text->Clear();
			text->SetText(str);
			GetWindows()->Run();
			Update();
		}

		if(line <= 0)
			line = 1;

		crs->SetLocation(size[0].cx, size[0].cy);
		for(int i=0; i<85; i++){
			time = (int)((43 * 10) / (float)line * speed[i] / (float)kyori * 1000);

			anim = new PosAnimation(crs, time, size[i+1].cx, size[i+1].cy);
			anim->SetOmit(false);
			anim->WaitEnd();
			anim->Release();
		}
	}
}