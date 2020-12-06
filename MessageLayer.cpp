#include "interface.h"

MessageLayer::MessageLayer(int x, int y, int width, int height) :Layer(x,y,width,height){
	text = NULL;
	textColor = 0xFFFFFF;
	shadowColor = 0;
	memcpy(&font , GetWindows()->GetDefaultFont() , sizeof(LOGFONT));
	marginX = 0;
	marginY = 0;
}

bool MessageLayer::DrawLayer(HDC hdc, int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
	Layer::DrawLayer(hdc, x1, y1, w1, h1, x2, y2, w2, h2);

	/*テキストの描画*/
	if (text) {
		RECT rect;
		HFONT hFont , hOld;

		rect.left = x1 + marginX + 2;
		rect.top = y1 + marginY + 2;
		rect.right = x1 + w1 - marginX;
		rect.bottom = y1 + h1 - marginY;

		hFont = CreateFontIndirect(&font);
		hOld = (HFONT)SelectObject(hdc , hFont);
		SetBkMode(hdc, TRANSPARENT);

		::SetTextColor(hdc , shadowColor);
		DrawText(hdc , text , -1 , &rect , DT_WORDBREAK | DT_EXPANDTABS);

		rect.left -= 2;
		rect.top -= 2;
		rect.right -= 2;
		rect.bottom -= 2;

		::SetTextColor(hdc , textColor);
		DrawText(hdc , text , -1 , &rect , DT_WORDBREAK | DT_EXPANDTABS);
		SelectObject(hdc , hOld);

		DeleteObject(hFont);
	}

	return true;
}

void MessageLayer::SetText(PTSTR settext){
	DWORD length;

	length = lstrlen(settext);
	if (text) {
		DWORD length2;

		length2 = lstrlen(text);
		text = (PTSTR)realloc(text , sizeof(TCHAR) * (length + length2 + 1));
	}
	else {
		text = (PTSTR)malloc(sizeof(TCHAR) * (length + 1));
		text[0] = 0;
	}

	lstrcatW(text , settext);
}

PTSTR MessageLayer::GetText(){
	return text;
}

void MessageLayer::Clear(){
	if (text) {
		free(text);
		text = NULL;
	}
}

void MessageLayer::SetTextColor(COLORREF color){
	textColor = color;
}

COLORREF MessageLayer::GetTextColor(){
	return textColor;
}

void MessageLayer::SetShadowColor(COLORREF color){
	shadowColor = color;
}

COLORREF MessageLayer::GetShadowColor(){
	return shadowColor;
}

void MessageLayer::SetFont(LOGFONT logfont){
	memcpy(&font, &logfont, sizeof(LOGFONT));
}

LOGFONT MessageLayer::GetFont(){
	return font;
}

void MessageLayer::SetMargin(int x, int y){
	marginX = x;
	marginY = y;
}

int MessageLayer::GetMarginX(){
	return marginX;
}

int MessageLayer::GetMarginY(){
	return marginY;
}

MessageLayer::~MessageLayer(){
	Layer::~Layer();
	if(text)free(text);
}

