/**		@author �x���@�I��
		EffWindow.cpp
*/

#include"SoundEffector.h"

//���W���[���C���X�^���X
extern HINSTANCE	g_hInst;

//�e�E�C���h�E�n���h��
extern HWND			g_hWnd;





//�E�C���h�E�֘A
//�E�C���h�E�쐬
int	SOUND_CONTROLER::InitApp(HWND hWnd)
{
	WNDCLASSEX	wc;	//�E�C���h�E�N���X
	RECT		rc;	//�e�E�C���h�E�̃N���C�A���g�̈���擾

	//���������N���A
	ZeroMemory(&wc,	sizeof(wc));

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.lpfnWndProc		= SOUND_CONTROLER::ChildProc;		//�E�C���h�E�v���V�[�W���[�o�^
	wc.hInstance		= g_hInst;				//���W���[���C���X�^���X��o�^
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
	wc.lpszClassName	= "SOUNDCONTROLCLASS";

	if(hWnd == NULL)
		MessageBox(NULL, "�E�C���h�E�n���h�����擾�ł��Ă��܂���B", "�G���[", MB_ICONEXCLAMATION | MB_OK);

	//�E�C���h�E�N���X�o�^
	if(RegisterClassEx(&wc) == 0)
		return -1;

	//�e�E�C���h�E�̃N���C�A���g�̈���擾
	GetClientRect(hWnd, &rc);

	//�E�C���h�E�̏c���̕����擾
	Width	= rc.right - rc.left;
	Height	= rc.bottom - rc.top - 32;

	//�q�E�C���h�E���쐬
	hWndChild	= CreateWindowEx(WS_EX_CLIENTEDGE, 
							"SOUNDCONTROLCLASS", 
							NULL, 
							WS_CHILD | WS_VISIBLE, 
							0, 0, 
							Width, Height, 
							hWnd, 
							NULL, 
							g_hInst, 
							NULL);

	if(hWndChild == NULL)
		return -1;
/*
	//�G�t�F�N�g�I��p�R���{�{�b�N�X���쐬
	hWndCombo	= CreateWindowEx(WS_EX_CLIENTEDGE,
								"COMBOBOX",
								NULL,
								CBS_DROPDOWNLIST | CBS_HASSTRINGS |	WS_TABSTOP | WS_VISIBLE | WS_CHILD,
								150, 42, 150, N_EFF * 24, hWndChild, (HMENU)IDC_COMBO1,
								g_hInst, NULL);
*/

	//�G�t�F�N�g�I��p���X�g�{�b�N�X���쐬
	hWndList	= CreateWindowEx(WS_EX_CLIENTEDGE,
								"LISTBOX",
								NULL,
								LBS_NOTIFY | WS_VSCROLL | LBS_SORT | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
								150, 42, 150, 21 * 2, hWndChild, (HMENU)IDC_LIST1,
								g_hInst, NULL);


	if(hWndList == NULL)
		return -1;

	//SendMessage(hWndCombo, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

	//�R���{�{�b�N�X�ɃA�C�e����ǉ�
	for(int i = 0; i < N_EFF; i++)
		SendMessage(hWndList, LB_INSERTSTRING, -1, (LPARAM)Effector.EffName[i]);

	SendMessage(hWndList, LB_SETCURSEL, 0, 0);


	//�{�^���R���g���[���쐬
	hInputCtrlButton	= CreateWindowEx(0, 
							"BUTTON", 
							"Start", 
							BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD, 
							rc.left + 5, rc.top + 5, 
							85, 27, 
							hWndChild, 
							(HMENU)IDC_BUTTON, 
							g_hInst, 
							NULL);
/*
	hOutputCtrlButton	= CreateWindowEx(0, 
							"BUTTON", 
							"Output Start", 
							BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD, 
							rc.left + 95, rc.top + 5, 
							90, 27, 
							hWndChild, 
							(HMENU)(IDC_BUTTON + 1), 
							g_hInst, 
							NULL);

	hEffEnblButton		= CreateWindowEx(0, 
							"BUTTON", 
							"Effect Off", 
							BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD, 
							rc.left + 5, rc.top + 38, 
							85, 27, 
							hWndChild, 
							(HMENU)(IDC_BUTTON + 2), 
							g_hInst, 
							NULL);
*/
	return 0;
}


//�E�C���h�E�v���V�[�W��
LRESULT CALLBACK	SOUND_CONTROLER::ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SOUND_CONTROLER&	Sc	= SOUND_CONTROLER::InitInstance();

	switch(uMsg)
	{
	//�I�[�f�B�I�f�o�C�X����������
	case WM_CREATE:
		if(Sc.InitAudio(44100, 16, 2048) == -1)
			MessageBox(NULL, "Audio Initialization Error", "Error", MB_ICONEXCLAMATION | MB_OK);
		break;

	//�I�[�f�B�I���o�͏���
	case MM_WIM_OPEN:
		if(Sc.WaveInOpened() == -1)
			MessageBox(hWnd, "Wave In Initialization Error", "Error", MB_OK | MB_ICONEXCLAMATION);
		break;

	case MM_WOM_OPEN:
		if(Sc.WaveOutOpened() == -1)
			MessageBox(hWnd, "Wave Out Initialization Error", "Error", MB_OK | MB_ICONEXCLAMATION);
		break;

	case MM_WOM_CLOSE:
		Sc.WaveOutClosed();
		break;

	case MM_WIM_CLOSE:
		Sc.WaveInClosed();
		break;

	case MM_WIM_DATA:
		Sc.InputData();
		break;

	case MM_WOM_DONE:
		Sc.OutputData();
		break;

	case WM_COMMAND:
		//�G�t�F�N�g�I�����̏���
		if(LOWORD(wParam) == IDC_LIST1)
		{
			RECT	rc;
			Sc.Eff	= (int)SendMessage(Sc.hWndList, LB_GETCURSEL, 0, 0);

			//�G�t�F�N�g�̃p�����[�^��ݒ�
			for(int i = 0; i < Sc.Effector.NumParam[Sc.Eff]; i++)
			{
				//�X���C�_�[�̏���l�Ɖ����l��ݒ�
				SendMessage(Sc.hSlider[i], TBM_SETRANGEMAX, TRUE, Sc.Effector.ParamInfo[Sc.Eff][i].Maximum);
				SendMessage(Sc.hSlider[i], TBM_SETRANGEMIN, TRUE, Sc.Effector.ParamInfo[Sc.Eff][i].Minimum);

				//���ۂ̒l��ݒ�
				SendMessage(Sc.hSlider[i], TBM_SETPOS, TRUE, Sc.Effector.ParamInfo[Sc.Eff][i].Value);
				Sc.Effector.Param[i]	= Sc.Effector.ParamInfo[Sc.Eff][i].Value;
			}

			//�G�t�F�N�g�ɉ����ăE�C���h�E�̑傫����ύX
			GetWindowRect(g_hWnd, &rc);
			MoveWindow(g_hWnd, rc.left, rc.top, rc.right - rc.left, 162 + 32 * Sc.Effector.NumParam[Sc.Eff], TRUE);
			MoveWindow(Sc.hWndEff, 0, Sc.Height, Sc.Width, 32 + 32 * Sc.Effector.NumParam[Sc.Eff], TRUE);
		}

		switch(wParam)
		{
		//���͊J�n�{�^��
		case IDC_BUTTON | (BN_CLICKED << 16):
			if(Sc.WaveInRunning == FALSE)
			{
				if(Sc.InputStart() == -1)
					MessageBox(NULL, "�T�E���h�̓��͊J�n�Ɏ��s���܂���", "Error", MB_ICONEXCLAMATION | MB_OK);

				SetWindowText(Sc.hInputCtrlButton, "Stop");

				if(Sc.OutputStart() == -1)
					MessageBox(NULL, "�T�E���h�̏o�͊J�n�Ɏ��s���܂���", "Error", MB_ICONEXCLAMATION | MB_OK);
			}

			else
			{
				Sc.WaveInStop();
				Sc.WaveOutStop();
				SetWindowText(Sc.hInputCtrlButton, "Start");
			}
			break;
/*
		//�o�͊J�n�{�^��
		case (IDC_BUTTON + 1) | (BN_CLICKED << 16):
			if(Sc.WaveOutRunning == FALSE)
			{
				if(Sc.OutputStart() == -1)
					MessageBox(NULL, "�T�E���h�̏o�͊J�n�Ɏ��s���܂���", "Error", MB_ICONEXCLAMATION | MB_OK);

				SetWindowText(Sc.hOutputCtrlButton, "Output Stop");
			}

			else
			{
				Sc.WaveOutStop();
				SetWindowText(Sc.hOutputCtrlButton, "Output Start");
			}
			break;
*/
/*
		//�G�t�F�N�g�C�l�[�u���{�^��
		case (IDC_BUTTON + 2) | (BN_CLICKED << 16):
			if(Sc.Eff == 0)
				SetWindowText(Sc.hEffEnblButton, "Effect On");

			else
				SetWindowText(Sc.hEffEnblButton, "Effect Off");
			Sc.EffEnabled	= Sc.EffEnabled ^ 1;

			if(Sc.hWndEff == NULL)
				Sc.InitEffApp(g_hWnd);

			break;
*/
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}





//�G�t�F�N�^�̃p�����[�^�ݒ�p�E�C���h�E�쐬
int	SOUND_CONTROLER::InitEffApp(HWND hWnd)
{
	WNDCLASSEX		wc;	//�E�C���h�E�N���X

	//���������N���A
	ZeroMemory(&wc,	sizeof(wc));

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.lpfnWndProc		= SOUND_CONTROLER::EffProc;		//�E�C���h�E�v���V�[�W���[�o�^
	wc.hInstance		= g_hInst;						//���W���[���C���X�^���X��o�^
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
	wc.lpszClassName	= "SOUNDEFFECTORCLASS";

	if(hWnd == NULL)
		MessageBox(NULL, "�E�C���h�E�n���h�����擾�ł��Ă��܂���B", "�G���[", MB_ICONEXCLAMATION | MB_OK);

	//�E�C���h�E�N���X�o�^
	if(RegisterClassEx(&wc) == 0)
		return -1;

	hWndEff	= CreateWindowEx(WS_EX_CLIENTEDGE, 
							"SOUNDEFFECTORCLASS", 
							NULL, 
							WS_CHILD | WS_VISIBLE, 
							0, Height, 
							Width, 36, 
							hWnd, 
							NULL, 
							g_hInst, 
							NULL);

	if(hWndEff == NULL)
		return -1;

	//�R�����R���g���[����������
	InitCommonControls();

	//�X���C�_�[��16�쐬
	for(int i = 0; i < 16; i++)
	{
		hSlider[i]	= CreateWindowEx(0,
									TRACKBAR_CLASS,
									NULL,
									WS_CHILD | WS_TABSTOP | WS_VISIBLE,
									130, 32 + (i * 32), Width - 130, 32, hWndEff, (HMENU)(IDC_SLIDER + 1), g_hInst, NULL);

		if(hSlider[i] == NULL)
			return -1;
	}

	return 0;
}


//�G�t�F�N�^�p�����[�^�擾�E�C���h�E�̃E�C���h�E�v���V�[�W��
LRESULT CALLBACK	SOUND_CONTROLER::EffProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SOUND_CONTROLER&	Sc	= SOUND_CONTROLER::InitInstance();
	HDC					hdc;			//�`��p�f�o�C�X�R���e�L�X�g�n���h��
	PAINTSTRUCT			ps;				//�`����\����

	RECT				rc;
	RECT				rc1;

	char				Str[64];		//��������ꎞ�I�ɕێ�

	int					tmp;


	switch(uMsg)
	{
	//�I�[�f�B�I�f�o�C�X����������
	case WM_CREATE:
		//�t�H���g�쐬
		Sc.hFontDefault	= CreateFont(28, 0, 0, FW_LIGHT, 
									FW_EXTRALIGHT,
									FALSE, FALSE, FALSE,
									SHIFTJIS_CHARSET,
									OUT_DEFAULT_PRECIS,
									CLIP_DEFAULT_PRECIS,
									DEFAULT_QUALITY,
									DEFAULT_PITCH | FF_DONTCARE, "Garamond");

		Sc.hFontName	= CreateFont(28, 0, 0, 0, 
									FW_EXTRALIGHT,
									TRUE, FALSE, FALSE,
									SHIFTJIS_CHARSET,
									OUT_DEFAULT_PRECIS,
									CLIP_DEFAULT_PRECIS,
									DEFAULT_QUALITY,
									DEFAULT_PITCH | FF_DONTCARE, "Garamond");

		Sc.hFontParam	= CreateFont(12, 0, 0, 0, 
									FW_DONTCARE,
									FALSE, FALSE, FALSE,
									SHIFTJIS_CHARSET,
									OUT_DEFAULT_PRECIS,
									CLIP_DEFAULT_PRECIS,
									DEFAULT_QUALITY,
									DEFAULT_PITCH | FF_DONTCARE, "MS ����");

		//�^�C�}�[��ݒ�
		SetTimer(hWnd, 500, 10, NULL);
		SetTimer(hWnd, 501, 100, NULL);
		break;

	//�G�t�F�N�g���\��
	case WM_PAINT:
		hdc	= BeginPaint(hWnd, &ps);
		
		SelectObject(hdc, Sc.hFontDefault);
		TextOut(hdc, 3, 0, "Effect:", 7);

		SelectObject(hdc, Sc.hFontName);
		TextOut(hdc, 90, 0, Sc.Effector.EffName[Sc.Eff], strlen(Sc.Effector.EffName[Sc.Eff]));

		SelectObject(hdc, Sc.hFontParam);

		//�`���`��ݒ�
		rc.top		= 0;
		rc.bottom	= 32;
		rc.left		= 20;
		rc.right	= 100;

		rc1.top		= 0;
		rc1.bottom	= 32;
		rc1.left	= 100;
		rc1.right	= 130;

		//�p�����[�^�̒l��\��
		for(int i = 0; i < Sc.Effector.NumParam[Sc.Eff]; i++)
		{
			rc.top		= 37 + 32 * i;
			rc.bottom	= rc.top + 32;
			rc1.top		= rc.top;
			rc1.bottom	= rc.bottom;


			sprintf(Str, "%s:", Sc.Effector.ParamInfo[Sc.Eff][i].Name);
			DrawText(hdc, Str, strlen(Str), &rc, NULL);

			sprintf(Str, "%d", Sc.Effector.Param[i]);
			DrawText(hdc, Str, strlen(Str), &rc1, DT_RIGHT);
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_TIMER:
		//�X���C�_�[�̒l���擾
		if(wParam == 500)
		{
			for(int i = 0; i < Sc.Effector.NumParam[Sc.Eff]; i++)
			{
				Sc.Effector.ParamInfo[Sc.Eff][i].Value	= SendMessage(Sc.hSlider[i], TBM_GETPOS, 0, 0);
				Sc.Effector.Param[i]					= Sc.Effector.ParamInfo[Sc.Eff][i].Value;
			}
		}

		//�p�����[�^�l����������
		else if(wParam == 501)
		{
			rc.top		= 32;
			rc.bottom	= 32 + 32 * Sc.Effector.NumParam[Sc.Eff];
			rc.left		= 100;
			rc.right	= 130;

			InvalidateRect(hWnd, &rc, TRUE);
		}

		else
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;

	case WM_DESTROY:
		KillTimer(hWnd, 500);
		KillTimer(hWnd, 501);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

