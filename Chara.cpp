#include "interface.h"

Chara::Chara(PCTSTR name, int xPos, int yPos, BYTE way, Map *targetmap) : Animation(1000){
	chara = new Layer(0, 0, 24, 32);
	chara->LoadDivBMP(name, 3, 4);
	chara->SetTransColor(RGB(0, 0, 0));
	chara->SetTransEnable(true);
	chara->SetBMP(2, way);
	chara->SetVisible(true);
	x = xPos;
	y = yPos;
	muki = way;
	walk = false;
	targetmove = 0;
	main = false;
	visible = false;
	map = targetmap->Retain();
	map->SetChara(this);
}

Chara::~Chara(){
	chara->Release();
	map->RemoveAtChara(this);
	map->Release();
}

bool Chara::Next(){
	int i, n;
	MAPLINK *maplink;

	next();
	if(map->GetVisible() == false)
		SetVisible(false);
	else
		SetVisible(true);

	if((x % 32) == 0 && (y % 32) == 0){
		if(targetmove > 0){
			int xmap = x / 32;
			int ymap = y / 32;
			switch(muki){
				case 0:
					ymap--;
					break;
				case 1:
					xmap++;
					break;
				case 2:
					ymap++;
					break;
				case 3:
					xmap--;
					break;
			}
			if(map->Judge(xmap, ymap)){
				walk = true;
				targetmove-= 32;
			}else{
				targetmove=0;
				walk = false;
			}
			maplink = map->JudgeLink(xmap, ymap);
			if(maplink != NULL){
				SetVisible(false);
				map->RemoveAtChara(this);
				if(GetMain() == true)
					map->SetVisible(false);
				map->Release();
				map = maplink->map->Retain();
				if(GetMain() == true)
					map->SetVisible(true);
				map->SetChara(this);
				x = maplink->nx * 32;
				y = maplink->ny * 32;
				walk = false;
				targetmove = 0;
				if(main == true)
					map->DrawMap();
			}
		}else{
			walk = false;
		}
	}

	if(walk == true){
		switch(muki){
			case 0:
				y--;
				break;
			case 1:
				x++;
				break;
			case 2:
				y++;
				break;
			case 3:
				x--;
				break;
		}
	}

	i = (x + y) % 32;
	if(i >=  0 && i <  8)
		n = 1;
	else if(i >=  8 && i < 16)
		n = 0;
	else if(i >= 16 && i < 24)
		n = 1;
	else if(i >= 24 && i < 32)
		n = 2;
	else
		n = 1;

	chara->SetBMP(n, muki);
	chara->SetLocation(x - map->GetX() + 4, y - map->GetY() - 14);

	return true;
}

void Chara::next(){

}	

void Chara::MoveChara(BYTE way, int move){
	muki = way;
	targetmove = move;

	switch(muki){
		case 0:
			if((y - move) < 0)
				targetmove = y;
			break;
		case 1:
			if((x + move) > (map->GetWidth() * 32 - 32))
				targetmove = map->GetWidth() * 32 - 32 - x;
			break;
		case 2:
			if((y + move) > (map->GetHeight() * 32 - 32))
				targetmove = map->GetHeight() * 32 - 32 - y;
			break;
		case 3:
			if((x - move) < 0)
				targetmove = x;
			break;
	}
}

void Chara::SetLocation(int xPos, int yPos){
	x = xPos;
	y = yPos;
}

int Chara::GetX(){
	return x;
}

int Chara::GetY(){
	return y;
}

void Chara::SetWay(BYTE way){
	muki = way;
}

BYTE Chara::GetWay(){
	return muki;
}

void Chara::SetWalk(bool Walk){
	walk = Walk;
}

Map* Chara::GetMap(){
	return map;
}

void Chara::SetMain(bool m){
	main = m;
}

bool Chara::GetMain(){
	return main;
}

void Chara::SetVisible(bool visibleI){
	visible = visibleI;
	chara->SetVisible(visible);
}

bool Chara::GetVisible(){
	return visible;
}