#include "interface.h"

//役割:2次元配列を確保する。
//引数:1.X座標の長さ 2.Y座標の長さ
// 例 :int **p = Malloc2<int>(5, 10); これで　int p[5][10]; と同じ
//特記:テンプレートを使用。malloc2関数より確実になるように調整。
//注意:テンプレートをソースファイルに書き、ヘッダーファイルにプロトタイプ宣言をすると　error LNK2019 になる。
//　　 よって、これは使えない・・・

#define ADD_MAX 32

typedef struct tagPOINTER {
	int w;
	int h;
	void **p;
}POINTER;

int DeleteFanc(int m, void*);

static POINTER* pointer = NULL;
static int count = 0;
static int max = 0;

void AddPointer(void **p, int w, int h){
	if(count >= max){
		max += ADD_MAX;
		if(count == 0){
			pointer = (POINTER*)malloc(sizeof(POINTER) * max);
			GetWindows()->AddMsgFanc(DeleteFanc, NULL);
		}else
			pointer = (POINTER*)malloc(sizeof(POINTER) * max);
	}

	pointer[count].p = p;
	pointer[count].w = w;
	pointer[count].h = h;
	count++;
}

int GetIndex(void **p){
	for(int i=0; i<count; i++){
		if(pointer[i].p == p)
			return i;
	}

	return NOTHING;
}

void RemoveAtPointer(int index){
	if(index >= count)
		return;

	count--;
	for(int i=index; i<count; i++)
		pointer[i] = pointer[i+1];
}

template <class P> P** Malloc2(int w, int h){
	P** point;

	point = (P**)malloc(sizeof(P*) * w);

	for(int i=0; i<w; i++){
		point[i] = (P*)malloc(sizeof(P) * h);
		ZeroMemory(point[i], sizeof(P) * h);
	}

	AddPointer(point, w, h);

	return point;
}

template <class P> void Free2(P** p){
	int index = GetIndex(p);

	if(index == NOTHING)
		return;

	for(int i=0; i<pointer[index].w; i++)
		free(pointer[index].p[i]);

	free(pointer[index].p);
	RemoveAtPointer(index);
}

template <class P> P** ReMalloc2(P** p, int w, int h){
	int index;
	P **point;

	point = Malloc2<P>(w, h);
	index = GetIndex(p);

	if(index == NOTHING) return;

	for(int i=0; i<pointer[index].w; i++)
		memcpy(point[i], pointer[index].p[i], sizeof(P) * pointer[index].h);

	Free2<P>(pointer[index].p);

	return point;
}

int DeleteFanc(int m, void* p){
	if(m != WM_DESTROY)
		return NOTHING;

	for( ; count ; )
		RemoveAtPointer(0);

	free(pointer);

	return 0;
}