#include "interface.h"

Layer::Layer(int x, int y, int width, int height)
{
	//フィールドの初期化
	dwref = 1;
	location.x = x;
	location.y = y;
	size.cx = width;
	size.cy = height;
	xnum = 1;
	ynum = 1;
	xbit = 0;
	ybit = 0;
	opaque = 0xFF;
	transColor = 0;
	linkColor = RGB(0, 0, 255);
	visible = true;
	maskEnable = false;
	transEnable = false;
	linkEnable = false;

	hBitmap = (HBITMAP**)malloc2(2,2,sizeof(HBITMAP));
	//hBitmap = Malloc2<HBITMAP>(1,1);
	hBitmap[0][0] = NULL;
	hMaskBitmap = NULL;

	GetWindows()->GetLayerManager()->Add(this);
}

Layer::~Layer()
{
	for(int x=0; x<xnum; x++){
		for(int y=0; y<ynum; y++){
			if(hBitmap[0][0]){
				DeleteObject(hBitmap[x][y]);
			}
		}
	}
	//free2((void**)hBitmap);
	if(hMaskBitmap){
		DeleteObject(hMaskBitmap);
	}
	GetWindows()->GetLayerManager()->Remove(this);

	if(dwref != 0)
		MessageBox(NULL,TEXT("delete ではなく Relese で破棄して下さい。"),TEXT("エラー Layer"),MB_OK);
}

bool Layer::DrawLayer(HDC hdc, int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	HDC hLayerDC;

	hLayerDC = GetDC();
	SetStretchBltMode(hdc , COLORONCOLOR);
	
	if (opaque == 0xFF) {
		if (maskEnable) {	/*マスクを使う*/
			HDC hMaskDC;

			hMaskDC = GetMaskDC();
			StretchBlt(hdc , x1 , y1 , w1 , h1 , hMaskDC , x2 , y2 , w2 , h2 , SRCPAINT);
			DeleteMaskDC(hMaskDC);

			StretchBlt(hdc , x1 , y1 , w1 , h1 , hLayerDC , x2 , y2 , w2 , h2 , SRCAND);
		}
		else if(transEnable) {  /*抜き色を使う*/
			TransparentBlt(hdc, x1, y1, w1, h1, hLayerDC, x2, y2, w2, h2, transColor);
		}
		else {	/*マスク・抜き色を使わない*/
			StretchBlt(hdc , x1 , y1 , w1 , h1 , hLayerDC , x2 , y2 , w2 , h2 , SRCCOPY);
		}
	}
	else {
		BLENDFUNCTION blendFunc = { 0 };

		blendFunc.SourceConstantAlpha = opaque;

		if (maskEnable) {	/*マスクを使う*/
			HBITMAP hTempBitmap;
			HDC hTempDC , hMaskDC;

			hMaskDC = GetMaskDC();
			hTempBitmap = CreateCompatibleBitmap(hLayerDC , w2 , h2);
			hTempDC = CreateCompatibleDC(hLayerDC);
			SelectObject(hTempDC , hTempBitmap);

			StretchBlt(hTempDC , 0 , 0 , w1 , h1 , hdc , x1 , y1 , w2 , h2 , SRCCOPY);
			StretchBlt(hTempDC , 0 , 0 , w1 , h1 , hMaskDC , x2 , y2 , w2 , h2 , SRCPAINT);
			StretchBlt(hTempDC , 0 , 0 , w1 , h1 , hLayerDC , x2 , y2 , w2 , h2 , SRCAND);
			AlphaBlend(hdc , x1 , y1 , w1 , h1 , hTempDC , 0 , 0 , w2 , h2 , blendFunc);

			DeleteMaskDC(hMaskDC);

			::DeleteDC(hTempDC);
			DeleteObject(hTempBitmap);
		}
		else if(transEnable){  /*抜き色を使う*/
			HBITMAP hTempBitmap;
			HDC hTempDC;

			hTempBitmap = CreateCompatibleBitmap(hLayerDC , w2 , h2);
			hTempDC = CreateCompatibleDC(hLayerDC);
			SelectObject(hTempDC , hTempBitmap);

			StretchBlt(hTempDC , 0 , 0 , w1 , h1 , hdc , x1 , y1 , w2 , h2 , SRCCOPY);
			TransparentBlt(hTempDC, 0, 0, w1, h1, hLayerDC, x2, y2, w2, h2, transColor);
			AlphaBlend(hdc , x1 , y1 , w1 , h1 , hTempDC , 0 , 0 , w2 , h2 , blendFunc);

			::DeleteDC(hTempDC);
			DeleteObject(hTempBitmap);
		}
		else {	/*マスク・抜き色を使わない*/
			AlphaBlend(hdc , x1 , y1 , w1 , h1 , hLayerDC , x2 , y2 , w2 , h2 , blendFunc);
		}
	}
 
	//リンクされている時は、四角形を表示する
	if (linkEnable == true) {
		HPEN hPen = CreatePen(PS_INSIDEFRAME, 1, linkColor);
		HBRUSH hBrush = (HBRUSH) GetStockObject(NULL_BRUSH);
		SelectObject(hdc, hPen);
		SelectObject(hdc, hBrush);
		Rectangle(hdc, x1, y1, x1 + w2, y1 + h2);
		DeleteObject(hPen);
	}

	DeleteDC(hLayerDC);
	return true;
}

void Layer::LoadBMP(PCTSTR name){
	BITMAP bmp_info;

	if(hBitmap[xbit][ybit] != NULL)
		DeleteObject(hBitmap[xbit][ybit]);
	hBitmap[xbit][ybit] = (HBITMAP)LoadImage(NULL,name,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	GetObject(hBitmap[xbit][ybit],(int)sizeof(bmp_info),&bmp_info);
	size.cx = bmp_info.bmWidth;
	size.cy = bmp_info.bmHeight;
}

void Layer::LoadDivBMP(PCTSTR name, int xNum, int yNum){
	BITMAP bmp_info;
	HBITMAP mem;
	HDC hmem,image;
	int bmpw, bmph;

	SetBMPNum(xNum, yNum);

	mem = (HBITMAP)LoadImage(NULL,name,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(mem == NULL) return;
	GetObject(mem,(int)sizeof(bmp_info),&bmp_info);
	hmem = CreateCompatibleDC(NULL);
	SelectObject(hmem,mem);
	bmpw = bmp_info.bmWidth / xNum;
	bmph = bmp_info.bmHeight / yNum;
	SetSize(bmpw, bmph);

	for(int x=0; x<xnum; x++){
		for(int y=0; y<ynum; y++){
			SetBMP(x, y);
			image = GetDC();
			BitBlt(image, 0, 0, bmpw, bmph, hmem, x*bmpw, y*bmph, SRCCOPY);
			DeleteDC(image);
		}
	}

	DeleteDC(hmem);
	DeleteObject(mem);
	SetBMP(0, 0);
}

void Layer::SetBMPNum(int xNum, int yNum){
	xnum = xNum;
	ynum = yNum;

	hBitmap = (HBITMAP**)realloc2((void**)hBitmap, xnum, ynum, sizeof(HBITMAP));
}

int Layer::GetBMPXNum(){
	return xnum;
}

int Layer::GetBMPYNum(){
	return ynum;
}

void Layer::SetBMP(int x, int y){
	if(x<xnum && x>=0)
		xbit = x;
	if(y<ynum && y>=0)
		ybit = y;
}

Layer* Layer::Retain(){
	dwref+=1;

	return this;
}

bool Layer::Release(){
	dwref-=1;

	if(dwref == 0){
		delete this;
		return true;
	}
	return false;
}

HDC Layer::GetDC()
{
	HDC hResult;

	if (hBitmap[xbit][ybit] == NULL) {
		HDC hdc;

		hdc = CreateDC(TEXT("DISPLAY") , NULL , NULL , NULL);
		hBitmap[xbit][ybit] = CreateCompatibleBitmap(hdc , size.cx , size.cy);
		hResult = CreateCompatibleDC(hdc);
		SelectObject(hResult , hBitmap[xbit][ybit]);
		DeleteDC(hdc);
	}
	else {
		hResult = CreateCompatibleDC(NULL);
		SelectObject(hResult , hBitmap[xbit][ybit]);
	}

	return hResult;
}

void Layer::DeleteDC(HDC hdc)
{
	::DeleteDC(hdc);
}

HDC Layer::GetMaskDC()
{
	HDC hResult;

	if (hMaskBitmap == NULL) {
		HDC hdc;

		hdc = CreateDC(TEXT("DISPLAY") , NULL , NULL , NULL);
		hMaskBitmap = CreateCompatibleBitmap(hdc , size.cx , size.cy);
		hResult = CreateCompatibleDC(hdc);
		SelectObject(hResult , hMaskBitmap);
		::DeleteDC(hdc);
	}
	else {
		hResult = CreateCompatibleDC(NULL);
		SelectObject(hResult , hMaskBitmap);
	}

	return hResult;
}

void Layer::DeleteMaskDC(HDC hdc)
{
	::DeleteDC(hdc);
}

int Layer::GetBmpW()
{
	BITMAP bmp;

	GetObject(hBitmap[xbit][ybit], (int)sizeof(bmp), &bmp);
	return bmp.bmWidth;
}

int Layer::GetBmpH()
{
	BITMAP bmp;

	GetObject(hBitmap[xbit][ybit], (int)sizeof(bmp), &bmp);
	return bmp.bmHeight;
}

void Layer::SetLinkEnable(bool enable)
{
	linkEnable = enable;
}

bool Layer::GetLinkEnable()
{
	return linkEnable;
}

void Layer::SetLinkColor(COLORREF color)
{
	linkColor = color;
}

COLORREF Layer::GetLinkColor()
{
	return linkColor;
}
