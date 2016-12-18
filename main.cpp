/**	@author	�x���@�I��
	main.cpp
*/

#include"SoundEffector.h"


//���W���[���C���X�^���X
HINSTANCE	g_hInst;

//�E�C���h�E�n���h��
HWND		g_hWnd;

//�N���X�l�[��
char		szClassName[]	= "SoundEffector";

//�E�C���h�E�v���V�[�W���[
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SOUND_CONTROLER&	Sc	= SOUND_CONTROLER::InitInstance();
	RECT				rc;

	switch(uMsg)
	{
	case WM_CREATE:
		if(Sc.InitApp(hWnd) == -1)
		{
			MessageBox(NULL, "�q�E�C���h�E�쐬���s", "Error", MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}

		if(Sc.InitEffApp(hWnd) == -1)
		{
			MessageBox(NULL, "�p�����[�^�ݒ�p�E�C���h�E�쐬���s", "Error", MB_OK | MB_ICONEXCLAMATION);
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

//�G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;	//���b�Z�[�W�\����
	WNDCLASSEX	wc;		//�E�C���h�E�N���X
	HWND		hWnd;	//�E�C���h�E�n���h��
	BOOL		bRet;

	//���W���[���C���X�^���X���O���[�o���ϐ��ɑ��
	g_hInst		= hCurInst;

	//���������N���A
	ZeroMemory(&wc,	sizeof(wc));

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.lpfnWndProc		= WndProc;				//�E�C���h�E�v���V�[�W���[�o�^
	wc.hInstance		= hCurInst;				//���W���[���C���X�^���X��o�^
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

	//�E�C���h�E�N���X�o�^
	if(RegisterClassEx(&wc) == 0)
		return FALSE;

	//�E�C���h�E�쐬
	hWnd	= CreateWindowEx(0, 
							szClassName, 
							"Sound Effector Ver.��", 
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

	//���b�Z�[�W���[�v
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