#include "interface.h"

#define MAX_POINT 32

void** point[MAX_POINT];
int width[MAX_POINT];
int height[MAX_POINT];
int count=0;

int Count(void** p){
	for(int i=0; i<count; i++){
		if(point[i] == p){
			return i;
		}
	}
	return NOTHING;
}

void** malloc2(int w, int h, int size){
	void** p;

	p = (void**)malloc( w*sizeof(void*) );

	if(p == NULL) {
		MessageBox(NULL, TEXT("メモリの確保に失敗しました"), TEXT("エラー"), MB_OK);
		return NULL;
	}

	for(int i = 0; i < w; ++i) {
		p[i] = (void*)malloc(size * h);
		if(p[i] == NULL) {
			MessageBox(NULL, TEXT("メモリの確保に失敗しました"), TEXT("エラー"), MB_OK);
			return NULL;
		}
	}

	for(int x=0; x<w; x++)
		ZeroMemory(p[x], h * size);

	point[count] = p;
	width[count] = w;
	height[count] = h;
	count++;

	return p;
}

void** realloc2(void** p, int w, int h, int size){
	void** pointer = malloc2(w, h, size);
	int i = Count(p);
	int W = width[i];
	int H = height[i];

	for(int x=0; x<W; x++)
		memcpy(pointer[x], p[x], H * size);

	free2(p);

	return pointer;
}

void free2(void** p){
	int w,i;
	
	i = Count(p);
	w = width[i];
	for(int n = 0; n < w; ++n) {
		free(p[n]);
	}
	free(p);

	count--;
	for(int n=i; n<count; ++n){
		point[n] = point[n+1];
		width[n] = width[n+1];
		height[n] = height[n+1];
	}
}
