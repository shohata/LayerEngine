#include "interface.h"

MainChara::MainChara(PCTSTR name, int xPos, int yPos, BYTE way, Map *targetmap) 
											: Chara(name, xPos, yPos, way, targetmap) {
	automatic = false;
	SetMain(true);
}

MainChara::~MainChara(){
	Chara::~Chara();
}

void MainChara::next(){
	if(automatic == false){
		int x = GetX();
		int y = GetY();
		BYTE muki = 0;

		if((x % 32) == 0 && (y % 32) == 0){
			if(GetKey(VK_UP))
				muki = 0;
			else if(GetKey(VK_RIGHT))
				muki = 1;
			else if(GetKey(VK_DOWN))
				muki = 2;
			else if(GetKey(VK_LEFT))
				muki = 3;
			else
				return;

			MoveChara(muki, 32);
		}
	}
}

void MainChara::SetAuto(bool Auto){
	automatic = Auto;
}