#include "interface.h"

#define POPNAME_THIS TEXT("Proc")
#define IDR_ACCELERATOR1 102

Windows *_win;

Windows* GetWindows(){
	return _win;
}

void SetWindows(Windows* win){
	_win = win;
}

int _MainW(HINSTANCE hCurInst,int nCmdShow){
	Windows *win = new Windows(hCurInst,nCmdShow);
	int i=0;

	SetWindows(win);
	if(setjmp(win->env)!=0)
		goto end;
	i=GameMain(win);

end: 
	delete win;
	return i;
}

Windows::Windows(HINSTANCE hCur, int nCmd){
	hCurInst = hCur;
	nCmdShow = nCmd;
	frames = 0;
	beforeTime = 0;
	msgCount = 0;
	ZeroMemory(msgFanc, sizeof(MESSAGE) * MAX_MSG);
	AddMsgFanc(MessageMouse, NULL);
	managerL = new LayerManager();
	managerA = new AnimationManager();
	
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)DefWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hCurInst;
    wc.hIcon = (HICON)LoadImage(NULL,
        MAKEINTRESOURCE(IDI_APPLICATION),
        IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED);
    wc.hCursor = (HCURSOR)LoadImage(NULL,
        MAKEINTRESOURCE(IDC_ARROW),
        IMAGE_CURSOR,0,0,
        LR_DEFAULTSIZE | LR_SHARED);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = TEXT("Windows");
    wc.hIconSm = (HICON)LoadImage(NULL,
        MAKEINTRESOURCE(IDI_APPLICATION),
        IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED);

	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.lpszClass = TEXT("Windows");
	cs.lpszName = TEXT("Windows");
	cs.style = WS_OVERLAPPEDWINDOW;
	cs.x = CW_USEDEFAULT;
	cs.y = CW_USEDEFAULT;
	cs.cx = CW_USEDEFAULT;
	cs.cy = CW_USEDEFAULT;
	cs.hwndParent = NULL;
	cs.hMenu = NULL;
	cs.hInstance = NULL;
	cs.lpCreateParams = 0;

	font.lfHeight		  =0;
	font.lfWidth		  =0;
	font.lfEscapement	  =0;
	font.lfOrientation	  =0;
	font.lfWeight		  =FW_BOLD;
	font.lfItalic		  =FALSE;
	font.lfUnderline	  =FALSE;
	font.lfStrikeOut	  =FALSE;
	font.lfCharSet		  =SHIFTJIS_CHARSET;
	font.lfOutPrecision	  =OUT_DEFAULT_PRECIS;
	font.lfClipPrecision  =CLIP_DEFAULT_PRECIS;
	font.lfQuality		  =PROOF_QUALITY;
	font.lfPitchAndFamily =FIXED_PITCH | FF_MODERN;
	lstrcpy(font.lfFaceName, TEXT("MS　ゴシック"));
}

Windows::~Windows(){
	delete managerL;
	delete managerA;
}

CREATESTRUCT* Windows::GetWindows(void){
	return &cs;
}

WNDCLASSEX* Windows::GetClass(void){
	return &wc;
}

MSG Windows::GetMsg(void){
	return message;
}

bool Windows::CreateWindows(){
	HDC hdc;

	if(!RegisterClassEx(&wc))
		return false;

	HWND hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
								cs.x, cs.y, cs.cx, cs.cy,
								cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);
	if(!hWnd)
       return false;
    ShowWindow(hWnd,nCmdShow);
    UpdateWindow(hWnd);

	hAccelTable = LoadAccelerators(hCurInst, (LPCTSTR)IDR_ACCELERATOR1);

	// this を生成したウィンドウに関連付ける
	SetProp( hWnd, POPNAME_THIS, (HANDLE)this );
	hMainWindow = hWnd;

	hdc = GetDC(hWnd);
	hScreenImage = CreateCompatibleBitmap(hdc, GAME_W, GAME_H);
	hScreenDC = CreateCompatibleDC(hdc);
	SelectObject(hScreenDC, hScreenImage);
	ReleaseDC(hWnd, hdc);

    return true;
}

HWND Windows::GetHwnd(){
	return hMainWindow;
}

LRESULT Windows::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	/*static MessageLayer *tx = new MessageLayer(0, 50, 150, 20);
	int x,y;*/
	
	int r = Message(msg);

	switch(msg){
		case WM_PAINT:
			GetClientRect(hWnd, &rect);
			hdc = BeginPaint(hWnd, &ps);
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, hScreenDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			break;
		/*case WM_MOUSEMOVE:
			TCHAR str[32];
			x = LOWORD(lp);
			y = HIWORD(lp);
			wsprintf(str, TEXT("(%d,%d)"), x, y);
			tx->Clear();
			tx->SetText(str);
			break;*/
		case WM_DESTROY:
			DeleteDC(hScreenDC);
			DeleteObject(hScreenImage);
			PostQuitMessage (0);       
            break;
		default: 
			if(r == NOTHING)
				return DefWindowProc (hWnd, msg, wp, lp);
			else
				return r;
	}

	return 0;
}

LRESULT Windows::DefWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	// static メンバ変数のため、hWnd に関連づいている this を取得
	// hWnd に this を入れるのは、ウインドウ生成直後の InitInstance メンバ関数内
	Windows *win = (Windows*)GetProp(hWnd,POPNAME_THIS);

	if( win )
		return win->WndProc( hWnd, message, wParam, lParam );

	return DefWindowProc( hWnd, message, wParam, lParam );
}

bool Windows::Run(){
	if (PeekMessage(&message , NULL , 0 , 0 , PM_REMOVE)) {
		if (message.message == WM_QUIT) longjmp(env, 1);
		if (!TranslateAccelerator(message.hwnd, hAccelTable, &message)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		return true;
	}
	else {
		IdleProc();
		return false;
	}
}

/* ウィンドウメッセージが送られていない間に繰り返し呼び出される */
void Windows::IdleProc() {
	HBITMAP hBitmap;
	HDC hdc;
	DWORD nowTime , progress;
#ifdef _DEBUG
	static TCHAR str[255] = TEXT("計測中です");
#endif

	if (beforeTime == 0) beforeTime = timeGetTime();
	nowTime = timeGetTime(); /* 現在の時間 */
	progress = nowTime - beforeTime;

	hBitmap = CreateCompatibleBitmap(hScreenDC , GAME_W , GAME_H);
	hdc = CreateCompatibleDC(hScreenDC);
	SelectObject(hdc , GetStockObject(BLACK_PEN));
	SelectObject(hdc , GetStockObject(WHITE_BRUSH));
	SelectObject(hdc , GetStockObject(SYSTEM_FONT));
	SetTextColor(hdc , RGB(0 , 0 , 0));
	SetBkMode(hdc , TRANSPARENT);
	SelectObject(hdc , hBitmap);

	//ScreenClear(hdc , windowSize.cx , windowSize.cy);

	DoAnimation();
	Update();
	UpdateMouse();
	DrawLayer(hdc);
	Message(WM_IDLE);
	BitBlt(hScreenDC , 0 , 0 , GAME_W , GAME_H , hdc , 0 , 0 , SRCCOPY);

	InvalidateRect(hMainWindow , NULL , FALSE);

#ifdef _DEBUG
	TextOut(hScreenDC , 10 , 10 , str , lstrlen(str));	/*FPSの表示*/
	InvalidateRect(hMainWindow , NULL , FALSE);
#endif

	/*FPSの監視・指定したFPS以上の場合は一時停止する*/
	if ((frames * (1000.0 / FPS)) > progress) {
		Sleep((DWORD)((frames * (1000.0 / FPS)) - progress));
	}

	if(progress >= 1000) {	/*1秒以上経過した*/
#ifdef _DEBUG
		wsprintf(str , TEXT("FPS=%03d") , frames);	/*FPSの文字列表現の保存*/
#endif
		beforeTime = nowTime;	/*時間の更新*/
		frames = 0;
	}
	frames++;

	DeleteDC(hdc);
	DeleteObject(hBitmap);
}

void Windows::DrawLayer(HDC hdc){
	LayerManager *manager;
	int max;

	//管理しているレイヤマネージャーを取得する
	manager = GetLayerManager();
	//レイヤの数を取得する
	max = manager->GetCount();

	//レイヤを処理するループ
	for(int i=0; i<max; i++){
		Layer* layer;
		int x,y,w1,h1,w2,h2;

		layer = manager->GetLayer(i);

		//レイヤは非表示なので表示しない
		if(!layer->GetVisible()) continue;

		//レイヤの座標とサイズを取得
		x = layer->GetX();
		y = layer->GetY();
		w1 = layer->GetWidth();
		h1 = layer->GetHeight();
		w2 = layer->GetBmpW();
		h2 = layer->GetBmpH();

		layer->DrawLayer(hdc, x, y, w1, h1, 0, 0, w2, h2);
	}
}

void Windows::DoAnimation(){
	AnimationManager *manager;
	int max;

	//レイヤの座標とサイズを取得
	manager = GetAnimationManager();
	//レイヤの座標とサイズを取得
	max = manager->GetAnimationLength();

	//アニメーションを処理するループ
	for(int i=0; i<max; i++){
		Animation* animation;

		animation = manager->GetAnimation(i);

		if(!animation->Next()){
			manager->RemoveAtAnimation(i);
			i-=1;
			max-=1;
		}
	}
}

int Windows::Message(int msg){
	int r = NOTHING;

	for(int i=0; i<msgCount; i++){
		int n = msgFanc[i](msg, msgParam[i]);
		if(n != NOTHING)
			r = n;
	}

	return r;
}

int Windows::AddMsgFanc(MESSAGE msg, void* param){
	if(msgCount == MAX_MSG){
		//エラー処理
#ifdef _DEBUG
		MessageBox(NULL, TEXT("最大値を超えて関数を追加しています。"), TEXT("エラー　Windows::AddMsgFanc"), MB_OK);
#endif
		return -1;
	}

	msgFanc[msgCount] = msg;
	msgParam[msgCount] = param;
	msgCount++;
	return msgCount; 
}

bool Windows::RemoveMsgFanc(MESSAGE msg){
	int count = NOTHING;

	for(int i=0; i<msgCount; i++){
		if(msgFanc[i] == msg){
			count = i;
			break;
		}
	}

	if(count == NOTHING){
		//エラー処理
		return true;
	}

	msgCount--;
	for(int i=count; i<msgCount; i++){
		msgFanc[i] = msgFanc[i+1];
		msgParam[i] = msgParam[i+1];
	}

	return false;
}

LayerManager* Windows::GetLayerManager(){
	return managerL;
}

AnimationManager* Windows::GetAnimationManager(){
	return managerA;
}

LOGFONT* Windows::GetDefaultFont(){
	return &font;
}