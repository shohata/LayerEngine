#include "interface.h"

#define REPEAT_START	30		// 自動リピートの開始溜め時間(0～127)
#define REPEAT_CYCLE	6		// 自動リピート速度(0～127)

static BYTE g_trg[256];
static int _mouse;

// 現在の入力を更新する
BYTE* Update( void )
{
	BYTE keys[256];

	// キー情報を取得
#ifdef _CONSOLE
	ZeroMemory( keys, sizeof(keys) );
	if( _kbhit() )
	{
		int key;
		key = _getch();
		if( key>='a' && key<='z' )
			keys[ key & ~0x20 ] = 0x80;
		else if( key>='A' && key<='Z' )
			keys[ key ] = 0x80;
		else if( key>='0' && key<='9' )
			keys[ key ] = 0x80;
	}
	memcpy( g_trg, keys, sizeof(g_trg) );

#else
	int i;
	BYTE *trg;

	// キー情報を取得
	if( GetKeyboardState( keys ) == 0 )
		return g_trg;

	// テンキーの数字は、フルキーと同じにしてしまう
	keys['0'] |= keys[ VK_NUMPAD0 ];
	keys['1'] |= keys[ VK_NUMPAD1 ];
	keys['2'] |= keys[ VK_NUMPAD2 ];
	keys['3'] |= keys[ VK_NUMPAD3 ];
	keys['4'] |= keys[ VK_NUMPAD4 ];
	keys['5'] |= keys[ VK_NUMPAD5 ];
	keys['6'] |= keys[ VK_NUMPAD6 ];
	keys['7'] |= keys[ VK_NUMPAD7 ];
	keys['8'] |= keys[ VK_NUMPAD8 ];
	keys['9'] |= keys[ VK_NUMPAD9 ];

	// トリガ押した瞬間のみ判定するように変更
	for( i=0; i<256; i++ )
	{
		trg = &g_trg[ i ];

		// ボタンを押している？
		if( keys[ i ] & 0x80 )
		{
			// 最上位ビットを倒す
			*trg &= ~0x80;

			// キーリピート中か？
			if( *trg )
			{
				// リピート開始時間を減らす
				(*trg)--;

				// リピート発生なら再び最上位ビットとリピート再開時間をセットする
				if( *trg == 0 )
					*trg = 0x80 + REPEAT_CYCLE;
			}
			else
			{
				// 初めて押したので最上位ビットとリピート開始時間をセットする
				*trg = 0x80 + REPEAT_START;
			}
		}
		else
		{
			// 離しているので情報をリセット
			*trg = 0;
		}
	}
#endif

	return g_trg;
}

// 現在のキー入力を取得
BYTE* GetKeys( void )
{
	return g_trg;
}

bool GetKeyDown(int key)
{
	if((g_trg[key] & (~0x80)) == REPEAT_START)
		return true;

	return false;
}

bool GetKey(int key)
{
	if(g_trg[key] != 0)
		return true;

	return false;
}

void WaitKeyDown(int key)
{
	while( (g_trg[key] & (~0x80)) != REPEAT_START )
		GetWindows()->Run();

	Update();
}

void WaitKey(int key)
{
	while( g_trg[key] == 0 )
		GetWindows()->Run();

	Update();
}

int MessageMouse(int m, void* p){
	MSG msg = GetWindows()->GetMsg();
	
	switch(m){
		case WM_LBUTTONDOWN:
			if(!(_mouse & MOUSE_LEFT_DOWN))
				_mouse |= MOUSE_LEFT_JUSTDOWN;
			break;
		case WM_LBUTTONUP:
			_mouse |= MOUSE_LEFT_UP;
			break;
		case WM_LBUTTONDBLCLK:
			_mouse |= MOUSE_LEFT_DBLCLK;
			break;
		case WM_RBUTTONDOWN:
			if(!(_mouse & MOUSE_RIGHT_DOWN))
				_mouse |= MOUSE_RIGHT_JUSTDOWN;
			break;
		case WM_RBUTTONUP:
			_mouse |= MOUSE_RIGHT_UP;
			break;
		case WM_RBUTTONDBLCLK:
			_mouse |= MOUSE_RIGHT_DBLCLK;
			break;
		case WM_MBUTTONDOWN:
			if(!(_mouse & MOUSE_MIDDLE_DOWN))
				_mouse |= MOUSE_MIDDLE_JUSTDOWN;
			break;
		case WM_MBUTTONUP:
			_mouse |= MOUSE_MIDDLE_UP;
			break;
		case WM_MBUTTONDBLCLK:
			_mouse |= MOUSE_MIDDLE_DBLCLK;
			break;
		default:
			return NOTHING;
	}
	return 0;
}

int UpdateMouse(){
	//解除する
	if(_mouse & MOUSE_LEFT_UP){
		_mouse ^= MOUSE_LEFT_UP;	
		_mouse = MOUSE_LEFT_DOWN & _mouse ^ _mouse;
		_mouse = MOUSE_LEFT_DBLCLK & _mouse ^ _mouse;
	}
	if(_mouse & MOUSE_RIGHT_UP){
		_mouse ^= MOUSE_RIGHT_UP;
		_mouse = MOUSE_RIGHT_DOWN & _mouse ^ _mouse;
		_mouse = MOUSE_RIGHT_DBLCLK & _mouse ^ _mouse;
	}
	if(_mouse & MOUSE_MIDDLE_UP){
		_mouse ^= MOUSE_MIDDLE_UP;
		_mouse = MOUSE_MIDDLE_DOWN & _mouse ^ _mouse;
		_mouse = MOUSE_MIDDLE_DBLCLK & _mouse ^ _mouse;
	}

	//切り替え
	if(_mouse & MOUSE_LEFT_JUSTDOWN){
		_mouse ^= MOUSE_LEFT_JUSTDOWN;
		_mouse |= MOUSE_LEFT_DOWN;
	}
	if(_mouse & MOUSE_LEFT_JUSTDOWN){
		_mouse ^= MOUSE_LEFT_JUSTDOWN;
		_mouse |= MOUSE_LEFT_DOWN;
	}
	if(_mouse & MOUSE_LEFT_JUSTDOWN){
		_mouse ^= MOUSE_LEFT_JUSTDOWN;
		_mouse |= MOUSE_LEFT_DOWN;
	}

	return _mouse;
}

int GetMouseInputs(void){
	return _mouse;
}

bool GetMouseInput(int input){
	if(_mouse & input)
		return true;
	
	return false;
}

void WaitMouseInput(int input){
	while(!(_mouse & input))
		GetWindows()->Run();
}

void GetMousePoint(int *x, int *y){
	POINT p;

	GetCursorPos(&p);
	ScreenToClient(GetWindows()->GetHwnd(), &p);

	*x = p.x;
	*y = p.y;
}

/*int GetMouseInput( void );
  定義値				マウスボタン  
  MOUSE_INPUT_LEFT		マウス左ボタン  
  MOUSE_INPUT_RIGHT		マウス右ボタン  
  MOUSE_INPUT_MIDDLE	マウス中央ボタン

  GetMousePoint( &posX, &posY );*/
