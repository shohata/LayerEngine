#include "interface.h"

#define CHARA 30

Map::Map(Layer *layer,PCTSTR filename, PCTSTR bmpname){
	LoadMap(filename);
	LoadBMP(bmpname);
	mapx = 0;
	mapy = 0;
	dwref = 1;
	mapimage = layer->Retain();
	count_chara = 0;
	max_chara = CHARA;
	link_count = 0;
	chara = (Chara**)malloc(CHARA * sizeof(Chara*));
	visible = false;
}

void Map::DrawMap(){
	int cx = mapx / 32;
	int shx = mapx %32;
	int cy = mapy / 32;
	int shy = mapy %32;
	int MapW = (mapimage->GetWidth() + 31) / 32;
	int MapH = (mapimage->GetHeight() + 32) / 32;
	RECT rc;
	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	HDC hdc = mapimage->GetDC();
	HDC bmp = CreateCompatibleDC(NULL);
	SelectObject(bmp , image);
	
	for(int y=0; y<MapH; y++){
		for(int x=0; x<MapW; x++){	
			if((cx+x >= width) || (cx+x < 0) || (cy+y >= height) || (cy+y < 0))	
				goto box;

			char c = mapdat[cx+x][cy+y];
			int i = Change(c,true);
			if(i!=0){
				BitBlt(hdc, x*32-shx, y*32-shy, 32, 32, bmp, (i-1)*32, 0, SRCCOPY);
				continue;
			}
box:
			rc.top = y*32-shy;
			rc.left = x*32-shx;
			rc.bottom = rc.top + 32;
			rc.right = rc.left + 32;
			FillRect(hdc, &rc, hBrush);
		}
	}
	mapimage->DeleteDC(hdc);
	DeleteDC(bmp);
	DeleteObject(hBrush);
}

bool Map::Judge(int x, int y){
	int i = Change(mapdat[x][y],true);

	if(i==3 || i==4 || GetChara(x, y))//通れるかどうか判定する。trueなら通れる。falseなら通れない。
		return false;
	else
		return true;
}

void Map::SetMapLink(int x, int y, Map *map, int lx, int ly){
	if(link_count == MAX_MAPLINK)
		return;

	maplink[link_count].ox = x;
	maplink[link_count].oy = y;
	maplink[link_count].nx = lx;
	maplink[link_count].ny = ly;
	maplink[link_count].map = map;
	
	link_count++;
}

MAPLINK* Map::JudgeLink(int x, int y){
	for(int i = 0; i<link_count; i++){
		if(maplink[i].ox == x){
			if(maplink[i].oy == y)
				return &(maplink[i]);
		}
	}
	return NULL;
}

void Map::SetLocation(int x, int y){
	mapx = x;
	mapy = y;
}

int Map::GetX(){
	return mapx;
}

int Map::GetY(){
	return mapy;
}

int Map::GetWidth(){
	return width;
}

int Map::GetHeight(){
	return height;
}

Map::~Map(){
	if(image){
		DeleteObject(image);
	}
	mapimage->Release();
	free2((void**)mapdat);
}

void Map::LoadMap(PCTSTR name){
	 HANDLE fh;
	 BOOL resurt;
	 int size[2];
	 DWORD byte;

     fh=CreateFile(name,GENERIC_READ,0,NULL,
		 OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
     if(fh==INVALID_HANDLE_VALUE){
		 MessageBox(NULL,TEXT("ファイルを開けませんでした。"),TEXT("エラー"),MB_OK);
		 return;
	 }
     resurt = ReadFile(fh,size,sizeof(int)*2,&byte,0);
	 if(!resurt){
	 	CloseHandle(fh);
	 	MessageBox(NULL,TEXT("ファイルの読み込みに失敗しました。"),TEXT("エラー"),MB_OK);
	 	return;
	 }	 
	 
	 width = size[0];
	 height = size[1];
	 mapdat = (char**)malloc2(width, height, sizeof(char));
	 if(mapdat == NULL)
		 return;
     for(int x=0; x<width; x++){
	 	resurt = ReadFile(fh,mapdat[x],height,&byte,0);
	 	if(!resurt){
	 	 	CloseHandle(fh);
		 	MessageBox(NULL,TEXT("ファイルの読み込みに失敗しました。"),TEXT("エラー"),MB_OK);
		 	return;
	 	}   
     }      
     CloseHandle(fh);
     return ;
}

void Map::LoadBMP(PCTSTR name){
   BITMAP bmp_info;
   HBITMAP hbmp2;
   HDC hdc,im,hdc2;
   int bmph,bmpw;

   hbmp2 = (HBITMAP)LoadImage(NULL,name,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
   GetObject(hbmp2,(int)sizeof(bmp_info),&bmp_info);
   bmpw = bmp_info.bmWidth;
   bmph = bmp_info.bmHeight;
   hdc = CreateCompatibleDC(NULL);
   SelectObject(hdc,hbmp2);

   hdc2 = GetDC(GetWindows()->GetHwnd());
   im = CreateCompatibleDC(NULL);
   image = CreateCompatibleBitmap(hdc2,bmph/32*bmpw,32);
   SelectObject(im,image);
   ReleaseDC(GetWindows()->GetHwnd(), hdc2);
                                     
   for(int i=bmph/32; i>=0; i--){
	BitBlt(im,i*bmpw,0,bmpw,32,hdc,0,i*32,SRCCOPY);
   }
  
   if(bmpw % 32 != 0 && bmph % 32 != 0)
      MessageBox(NULL,TEXT("高さ・幅が合いません。"),TEXT("警告"),MB_OK);

   /*TCHAR str[20];
   wsprintf(str,TEXT("幅%d 高さ%d"),bmpw,bmph);
   MessageBox(NULL,str,TEXT("確認"),MB_OK);*/
   
   bmpw *= bmph/32;
     
   DeleteObject(hbmp2);
   DeleteDC(hdc);
   DeleteDC(im);
}

char Map::Change(char str,bool i){
  switch(i){
	case true://文字から数字
		if(str >= '!'  && str <= '~')
			return str - '!';
	else if(str >= '｡' && str <= 223)
			return str - 68;
	else
		return 0; 
		break;

	case false://数字から文字
		if(str >= 0  && str <= 94)
			return str + 33;
		else if(str >= 95 && str <= 157)
			return str + 68;
		else
			return 0;
		break;
  }
	return 0;
}

void Map::SetChara(Chara *c){
	if(count_chara >= max_chara){
		max_chara += CHARA;
		chara = (Chara**)realloc(*chara, max_chara);
	}
	chara[count_chara] = c;
	count_chara ++;
}

Chara* Map::GetChara(int x, int y){
	int cx, cy;

	for(int i=0; i<count_chara; i++){
		ChangeXY(chara[i], &cx, &cy);

		if(cx == x && cy == y)
			return chara[i];
	}
	return NULL;
}

void Map::ChangeXY(Chara *c, int *x, int *y){
	*x = c->GetX() / 32;
	*y = c->GetY() / 32;

	if(c->GetX() % 32 >= 16)
		*x+=1;
		
	if(c->GetY() % 32 >= 16)
		*y+=1;	
}

void Map::RemoveAtChara(Chara *c){
	int index;

	for(int i=0; i<count_chara; i++){
		if(chara[i] == c)
			index = i;
	}

	count_chara --;

	for(int i=index; chara[i]; i++){
		if(i == max_chara - 1){
			chara[i] = NULL;
			break;
		}
		chara[i] = chara[i + 1];
	}
}

Map* Map::Retain(){
	dwref+=1;

	return this;
}

bool Map::Release(){
	dwref-=1;

	if(dwref == 0){
		delete this;
		return true;
	}
	return false;
}

void Map::SetVisible(bool visibleI){
	mapimage->SetVisible(visibleI);
	visible = visibleI;
}

bool Map::GetVisible(){
	return visible;
}