#include <windows.h>
#include <setjmp.h>
#include <math.h>
#pragma comment(lib,"msimg32")
#pragma comment(lib,"winmm")

#define GAME_W 840
#define GAME_H 660
#define INCLUDE_INTERFACE_H

#define _OK_ MessageBox(NULL, L"OK", L"OK", MB_OK);
#define MAX_ANIM 32
#define MAX_LINK 32
#define MAX_MSG  32
#define MAX_MAPLINK 32
#define FPS 60.0

#define _GAMEMAIN int WINAPI WinMain(HINSTANCE hCurInst,HINSTANCE hPrevInst,LPSTR lpsCmdline,int nCmdShow){return _MainW(hCurInst,nCmdShow);}
#define GAMEMAIN _GAMEMAIN
#define WM_IDLE   0xffff
#define NOTHING -1

#define MSPOINT(x,y) ((WPARAM)(((WORD)(x))+((WPARAM)((WORD)(y)<<16))))
#define MOUSE_LEFT_DOWN			0x00001
#define MOUSE_RIGHT_DOWN		0x00002
#define MOUSE_MIDDLE_DOWN		0x00004
#define MOUSE_XBUTTON1_DOWN		0x00008
#define MOUSE_XBUTTON2_DOWN		0x00010
#define MOUSE_LEFT_DBLCLK		0x00020
#define MOUSE_RIGHT_DBLCLK		0x00040
#define MOUSE_MIDDLE_DBLCLK		0x00080
#define MOUSE_LEFT_UP			0x00100
#define MOUSE_RIGHT_UP			0x00200
#define MOUSE_MIDDLE_UP			0x00400
#define MOUSE_XBUTTON1_UP		0x00800
#define MOUSE_XBUTTON2_UP		0x01000
#define MOUSE_LEFT_JUSTDOWN		0x02000
#define MOUSE_RIGHT_JUSTDOWN	0x04000
#define MOUSE_MIDDLE_JUSTDOWN	0x08000
#define MOUSE_XBUTTON1_JUSTDOWN 0x10000
#define MOUSE_XBUTTON2_JUSTDOWN 0x20000

typedef int (*FANCTION)(void*);
typedef int(*MESSAGE)(int, void*);

class Layer;
class Map;

typedef struct tagLINK{
	Layer* own;
	Layer* up;
	Layer* down;
	Layer* right;
	Layer* left;
}LINK, *ILINK;

typedef struct tagMAPLINK{
	int ox;
	int oy;
	int nx;
	int ny;
	Map *map;
}MAPLINK;

//キー入力
BYTE* Update( void );		//更新する
BYTE* GetKeys( void );		//キー配列を取得
void  WaitKey( int );		//指定のキーが押されるまで待つ	
void  WaitKeyDown( int );	//指定のキーが押される瞬間まで待つ
bool  GetKey( int );		//指定のキーが押されていればtrueを返す
bool  GetKeyDown( int );	//指定のキーが押されている瞬間であればtrueを返す

//マウス入力
int MessageMouse(int, void*);
int UpdateMouse();
int GetMouseInputs(void);
bool GetMouseInput(int input);
void WaitMouseInput(int input);
void GetMousePoint(int *x, int *y);

class Chara;

class Layer{
	//参照カウンター
	int dwref;
	POINT location;
	SIZE  size;
	bool  visible, maskEnable, transEnable, linkEnable;
	BYTE  opaque;
	COLORREF transColor, linkColor;
	HBITMAP **hBitmap, hMaskBitmap;
	//ビットマップ配列・番号
	int xnum, ynum;
	int xbit, ybit;

public:
	//初期化処理
	Layer(int x,int y,int width,int height);

	//参照カウンター
	Layer* Retain(void);
	virtual bool  Release(void);

	//座標
	void SetLocation(int x,int y){location.x=x; location.y=y;}
	int  GetX(void){return location.x;}
	int  GetY(void){return location.y;}

	//表示サイズ
	void SetSize(int w,int h){size.cx=w; size.cy=h;}
	int  GetWidth(void){return size.cx;}
	int  GetHeight(void){return size.cy;}

	//画像サイズ
	int GetBmpW(void);
	int GetBmpH(void);

	//可視性
	void SetVisible(bool visibleI){visible=visibleI;}
	bool GetVisible(void){return visible;}

	//不透明度
	void SetOpaque(BYTE value){opaque=value;}
	BYTE GetOpaque(void){return opaque;}

	//描画、このレイヤを指定したデバイステキストに転送する
	virtual bool DrawLayer(HDC hdc,int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);

	//画像読み込み
	void LoadBMP(PCTSTR name);

	//画像分割読み込み
	void LoadDivBMP(PCTSTR name, int xNum, int yNum);

	//ビットマップ数
	void SetBMPNum(int xNum, int yNum);
	int GetBMPXNum();
	int GetBMPYNum();

	//ビットマップ番号指定
	void SetBMP(int x, int y);

	//レイヤに描画を行うためのデバイスコンテキスト
	HDC  GetDC(void);
	void DeleteDC(HDC hdc);

	//マスクデータとなる２色ビットマップのデバイスコンテキスト
	HDC  GetMaskDC(void);
	void DeleteMaskDC(HDC hdc);

	//マスクデータを使って透過処理を有効にするかどうか
	void SetMaskEnable(bool enable){maskEnable=enable;transEnable=false;}
	bool GetMaskEnable(void){return maskEnable;}

	//抜き色設定
	void SetTransColor(COLORREF color){transColor = color;}
	COLORREF GetTransColor(void){return transColor;}

	//抜き色を使って透過処理をするかどうか
	void SetTransEnable(bool enable){transEnable=enable;maskEnable=false;}
	bool GetTransEnable(void){return transEnable;}

	//リンク設定
	void SetLinkEnable(bool enable);
	bool GetLinkEnable(void);

	//リンクの枠の色
	void SetLinkColor(COLORREF color);
	COLORREF GetLinkColor(void);

	//破棄処理
	virtual ~Layer(void);
};

class LayerManager{
	//レイヤの配列
	Layer** contents;

	//レイヤの数
	int dwLayerCount;

	//counter配列の最大インデックス
	int dwMax;

public:
	//レイヤを作成する
	LayerManager();

	//レイヤを追加する
	int Add(Layer *);
	//レイヤを指定したインデックスに挿入する
	bool Insert(Layer *, int);
	//指定したレイヤを削除する
	bool Remove(Layer *);
	//指定したインデックスのレイヤを削除する
	void RemoveAt(int);
	//すべてのレイヤを削除する
	void RemoveAll();
	//指定したインデックスのレイヤを取得する
	Layer* GetLayer(int);
	//レイヤ数を返す
	int GetCount();

	//レイヤマネージャを破棄する
	~LayerManager();
};

class MessageLayer : public Layer {
	PTSTR text;
	COLORREF textColor , shadowColor;
	LOGFONT font;
	int marginX , marginY;
public:
	//初期化処理
	MessageLayer(int x, int y, int width, int height);

	//描画処理
	bool DrawLayer(HDC hdc,int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);

	//描画するためのテキスト
	void  SetText(PTSTR settext);
	PTSTR GetText(void);

	//テキストを削除する
	void Clear(void);

	//テキストの色
	void SetTextColor(COLORREF color);
	COLORREF GetTextColor(void);

	//テキストの影の色
	void SetShadowColor(COLORREF color);
	COLORREF GetShadowColor(void);

	//テキストのフォント
	void SetFont(LOGFONT logfont);
	LOGFONT GetFont(void);

	//テキストの座標
	void SetMargin(int x, int y);
	int  GetMarginX(void);
	int  GetMarginY(void);

	//破棄処理
	~MessageLayer();
};

class Animation{
	//参照カウンター
	int ref;
	//ミリ秒単位のアニメーション時間
	int timeMill;
	//達成率
	int counter,max;
	//省略してよいか
	bool omit;
public:
	Animation(int time);
	virtual ~Animation();

	//参照カウンター
	Animation* Retain();
	virtual bool  Release();

	//このアニメーションが１周するまでに必要な時間をミリ秒単位で返す
	int  GetTime();
	//このアニメーションの達成率を設定
	void SetAttRate(float ps);
	//このアニメーションの達成率を返す
	float GetAttRate();
	//このアニメーションを省略してよいか設定する
	void SetOmit(bool om);
	//このアニメーションを省略してよいか返す
	bool GetOnit();
	//このアニメーションが終わるまで待つ
	void WaitEnd();
	//このアニメーションを次の状態に進める
	virtual bool Next();
};

class AnimationManager{
	//アニメーションの配列
	Animation* animObject[MAX_ANIM];

	//アニメーションの数
	int animCount;
public:
	AnimationManager();
	~AnimationManager();
	//アニメーションを追加する
	int AddAnimation(Animation *animation);
	//指定したインデックスのアニメーションを削除する
	bool RemoveAtAnimation(int index);
	//アニメーションの数を返す
	int  GetAnimationLength();
	//指定したインデックスのアニメーションを返す
	Animation* GetAnimation(int index);
};

class FeedAnimation : public Animation{
	//フェードイン・フェードアウトを行うレイヤ
	Layer* targetLayer;
	//不透明度
	BYTE targetValue, defaultValue;
public:
	FeedAnimation(Layer* layer,int time, BYTE value);
	~FeedAnimation();
	bool Next();
};

class TextAnimation : public Animation{
	MessageLayer* targetLayer;
	PTSTR text;
public:
	TextAnimation(MessageLayer* layer, PCTSTR str, int time);
	~TextAnimation();
	bool Next();
};

class PosAnimation : public Animation{
	Layer* targetLayer;
	//開始座標
	int startX, startY;
	//目標座標
	int endX, endY;
public:
	PosAnimation(Layer* layer, int time, int x, int y);
	~PosAnimation();
	bool Next();
};

class ZoomAnimation : public Animation{
	Layer* targetLayer;
	//開始サイズ
	int startW, startH;
	//目標サイズ
	int endW, endH;
public:
	ZoomAnimation(Layer* layer, int time, int w, int h);
	~ZoomAnimation();
	bool Next();
};

class LinkAnimation : public Animation{
	Layer** targetLayer;			//選択肢のレイヤ
	RECT* rect;						//レイヤの関連付け
	int linkCount;					//レイヤの数
	int linkIndex;					//指定しているレイヤのインデックス
	bool decide;					//決定しているか
public:
	LinkAnimation(LINK **link, int count);
	~LinkAnimation();
	bool Next();

	Layer* GetDecideLayer();		//決定されたレイヤを返す
	int GetDecideIndex();			//決定されたインデックスを返す
	int GetLinkLength();			//レイヤの数を返す
	Layer* GetLayer(int index);		//指定したインデックスのレイヤを返す

private:
	int GetIndex(Layer *layer, LINK** link);		//指定したレイヤのインデックスを返す
	void JudgeLayer(int x, int y);					//指定した座標のレイヤのインデックスを代入
};

class DivAnimation : public Animation{
	Layer* targetLayer;
	int count;
	int maxCount;
	int x, y;
public:
	DivAnimation(Layer *layer, int time, int during);
	~DivAnimation();
	bool Next();
};


class Map {
   Layer* mapimage;
   HBITMAP image;
   int mapx;
   int mapy;
   int width;
   int height;
   char **mapdat;
   int count_chara;
   int max_chara;
   Chara **chara;
   int dwref;
   MAPLINK maplink[MAX_MAPLINK];
   int link_count;
   bool visible;
public:
	Map(Layer *layer,PCTSTR filename,PCTSTR bmpname);
	~Map();
	//マップ表示
	void DrawMap();
	//通行判定
	bool Judge(int x,int y);
	//マップ間移動設定・判断
	void SetMapLink(int x, int y, Map *map, int lx, int ly);
	MAPLINK* JudgeLink(int x, int y);
	//座標
	void SetLocation(int x,int y);
	int  GetX(void);
	int  GetY(void);
	//サイズ
	int GetWidth();
	int GetHeight();

	//キャラクター
	void SetChara(Chara *c);
	Chara* GetChara(int x, int y);
	void RemoveAtChara(Chara *c);

	Map* Retain();
	bool Release();

	//可視性
	void SetVisible(bool visibleI);
	bool GetVisible();
private:
	void LoadMap(PCTSTR name);
	void LoadBMP(PCTSTR name);
	char Change(char str,bool i);
	void ChangeXY(Chara *c, int *x, int *y);
};	

class Chara : public Animation{
	int x, y;
	BYTE muki;
	bool walk;
	Layer *chara;
	Map *map;
	int targetmove;
	bool main;
	bool visible;
public:
	Chara(PCTSTR name, int xPos, int yPos, BYTE way, Map *tagetmap);
	~Chara();
	bool Next();
	virtual void next();

	//キャラクターの座標
	void SetLocation(int xPos,int yPos);
	int GetX();
	int GetY();

	//キャラクターの向き
	void SetWay(BYTE way);
	BYTE GetWay();

	//マップ
	Map* GetMap();

	//歩行
	void SetWalk(bool Walk);

	//キャラクター移動
	void MoveChara(BYTE way, int move);

	//主人公か設定・取得
	void SetMain(bool m);
	bool GetMain(); 

	//可視性
	void SetVisible(bool visibleI);
	bool GetVisible();
};

class MainChara : public Chara{
	bool automatic;
public:
	MainChara(PCTSTR name, int xPos, int yPos, BYTE way, Map *tagetmap);
	~MainChara();
	void next();

	//キャラクター自動・操縦
	void SetAuto(bool Auto);
};

class Windows {
	CREATESTRUCT cs;
	WNDCLASSEX wc;
	HINSTANCE hCurInst;
	HWND hMainWindow;
	int nCmdShow;
	MSG message;
	HDC hScreenDC;
	HBITMAP hScreenImage;
	HACCEL hAccelTable;
	LayerManager *managerL;
	AnimationManager *managerA;
	MESSAGE msgFanc[MAX_MSG];
	void* msgParam[MAX_MSG];
	int msgCount;
	LOGFONT font;

	DWORD frames;		/*フレームカウント*/
	DWORD beforeTime;	/*以前の時間*/
public:
	jmp_buf env;	//setjmp用のバッファ
public:
	Windows(HINSTANCE hCur, int nCmd);
	~Windows();
	CREATESTRUCT* GetWindows(void);
	WNDCLASSEX* GetClass(void);
	bool CreateWindows(void);
	HWND GetHwnd(void);
	MSG GetMsg(void);
	int AddMsgFanc(MESSAGE msg, void* param);
	bool RemoveMsgFanc(MESSAGE msg);
	bool Run(void);		//メッセージがあったらtrue
	//レイヤマネージャーを得る。
	LayerManager* GetLayerManager(void);
	//アニメーションマネージャーを得る
	AnimationManager* GetAnimationManager();
	//デフォルトのフォントを得る
	LOGFONT* GetDefaultFont();
private:
	LRESULT WndProc (HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK DefWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	void IdleProc();
	void DrawLayer(HDC);
	void DoAnimation();
	int Message(int msg);
};
Windows* GetWindows();
void SetWindows(Windows *win);
int _MainW(HINSTANCE hCurInst,int nCmdShow);

int GameMain(Windows* win);

//役割:２次元配列を確保する。
//引数:1.X座標の長さ 2.Y座標の長さ 3.確保する変数の大きさ
// 例 :int **p = malloc2(5, 10, sizeof(int)); これで int p[5][10]; と同じことができる。
void** malloc2(int w, int h, int size);
void** realloc2(void** p, int w, int h, int size);
void free2(void** p);
