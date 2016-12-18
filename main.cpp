/**	@author	堀部　悠平
	main.cpp
*/

#include"SoundEffector.h"


//モジュールインスタンス
HINSTANCE	g_hInst;

//ウインドウハンドル
HWND		g_hWnd;

//クラスネーム
char		szClassName[]	= "SoundEffector";

//ウインドウプロシージャー
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SOUND_CONTROLER&	Sc	= SOUND_CONTROLER::InitInstance();
	RECT				rc;

	switch(uMsg)
	{
	case WM_CREATE:
		if(Sc.InitApp(hWnd) == -1)
		{
			MessageBox(NULL, "子ウインドウ作成失敗", "Error", MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}

		if(Sc.InitEffApp(hWnd) == -1)
		{
			MessageBox(NULL, "パラメータ設定用ウインドウ作成失敗", "Error", MB_OK | MB_ICONEXCLAMATION);
			return -1;
		}
		break;

	case WM_SIZE:
		MoveWindow(Sc.hWndChild, 0, 0, Sc.GetWidth(), Sc.GetHeight(), TRUE);

		if(Sc.hWndEff != NULL)
		{
			GetClientRect(Sc.hWndEff, &rc);
			MoveWindow(Sc.hWndEff, 0, Sc.GetHeight(), Sc.GetWidth(), rc.bottom - rc.top, TRUE);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

//エントリーポイント
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;	//メッセージ構造体
	WNDCLASSEX	wc;		//ウインドウクラス
	HWND		hWnd;	//ウインドウハンドル
	BOOL		bRet;

	//モジュールインスタンスをグローバル変数に代入
	g_hInst		= hCurInst;

	//メモリをクリア
	ZeroMemory(&wc,	sizeof(wc));

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.lpfnWndProc		= WndProc;				//ウインドウプロシージャー登録
	wc.hInstance		= hCurInst;				//モジュールインスタンスを登録
	wc.hIcon			= (HICON)LoadImage(NULL, 
							MAKEINTRESOURCE(IDI_APPLICATION),
							IMAGE_ICON, 
							0, 
							0, 
							LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor			= (HCURSOR)LoadImage(NULL, 
								MAKEINTRESOURCE(IDC_ARROW), 
								IMAGE_CURSOR, 
								0, 
								0, 
								LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= (LPCSTR)szClassName;

	//ウインドウクラス登録
	if(RegisterClassEx(&wc) == 0)
		return FALSE;

	//ウインドウ作成
	hWnd	= CreateWindowEx(0, 
							szClassName, 
							"Sound Effector Ver.β", 
							WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 
							CW_USEDEFAULT, CW_USEDEFAULT, 
							320, 162, 
							NULL, 
							NULL, 
							hCurInst, 
							NULL);

	if(!hWnd)
		return FALSE;

	g_hWnd	= hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//メッセージループ
	while((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0)
	{
		if(bRet == -1)
			break;

		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}