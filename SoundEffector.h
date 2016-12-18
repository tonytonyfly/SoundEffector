/**		@author �x���@�I��
		SoundEffector.h
*/


//#define	SAFE_DELETE(p)	if(p){delete[] p; p = NULL;};


#ifndef __SOUNDEFFECTOR_H__
#define __SOUNDEFFECTOR_H__

#define DEBUG	1

#include<windows.h>
#include<mmsystem.h>
#include<commctrl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comctl32.lib")


//�V�����G�t�F�N�g��������ނ̐��𑝂₵�Ă�������
//////////////////////////////////////////
//�G�t�F�N�g�̎��
const int	N_EFF	= 3;
//////////////////////////////////////////

//�R���g���[���ނ̃��b�Z�[�W
const int	IDC_BUTTON	= WM_APP;
const int	IDC_SLIDER	= WM_APP + 16;
const int	IDC_LIST1	= IDC_SLIDER + 16;


//�o�C�g�f�[�^�ƕ��������_�f�[�^�ϊ��p�\����
struct WORD_DATA{
	unsigned char	Hi;
	unsigned char	Lo;
};

union DATA_CONV{
	struct WORD_DATA	Word;
	short				Integer;
};


//�X���C�_�[�ɕK�v�ȏ��
struct PARAM_INFO{
	//�l�̏���A�����A�����l
	int		Minimum;
	int		Maximum;
	int		Value;

	//�p�����[�^�̖��O�i�K�����p16�����ȓ��Łj
	char	Name[16];
};



//�M�������N���X
class SOUND_EFFECTOR{
private:
	int	BuffSize;		//�o�b�t�@�T�C�Y



	//�G�t�F�N�g�̖��O�Ƃ̑Ή�
	char	EffName[N_EFF][64];

	//�G�t�F�N�g���ƂɃp�����[�^���������邩���Ǘ�
	int		NumParam[N_EFF];

	//�G�t�F�N�g�p�p�����[�^�̏����ݒ�
	struct PARAM_INFO	ParamInfo[N_EFF][16];

	//�p�����[�^���i�[
	int		Param[16];



	int	Thru(float* InBuff, float* OutBuff);		//�G�t�F�N�g�Ȃ�
	int	MovAvg(float* InBuff, float* OutBuff);		//�ړ����σt�B���^�����O
	int	Distortion(float *InBuff, float *OutBuff);	//�f�B�X�g�[�V����

	//�G�t�F�N�g�ւ̃|�C���^�i�G�t�F�N�g��؂�ւ��邽�߂ɕK�v�j
	int	(SOUND_EFFECTOR::*pEffect[N_EFF])(float *, float *);

public:
		SOUND_EFFECTOR();										//�R���X�g���N�^
	int	SetBuffSize(int BuffSize);

	int	Effect(float *InBuff, float *OutBuff, int SelEff);


	friend class SOUND_CONTROLER;
};

//���������p�N���X
class SOUND_CONTROLER{
private:
	//�����M�����
	int				SamplingRate;		//�T���v�����O���g��
	int				BitDepth;			//�ʎq���r�b�g��
	int				BuffSize;			//�o�b�t�@�T�C�Y�i���o�́j




	//�G�t�F�N�g����
	SOUND_EFFECTOR	Effector;			//�M�������C���^�t�F�[�X
	float			*WaveInBuffer;		//�M�������p���������_�o�b�t�@
	float			*WaveOutBuffer;		//���H��̐M���g�`�p�o�b�t�@
	union DATA_CONV	DataConv;			//�f�[�^�ϊ��C���^�t�F�[�X




	//��Ԑ���
	bool			WaveInRunning;		//���쒆�̃t���O
	bool			WaveOutRunning;		//
	bool			isInitialized;		//�������ς݂̃t���O�i�������֘A�j
	//bool			EffectEnabled;		//�G�t�F�N�g���������ۂ�
	int				PtrInBuff;			//�g�p���̓��̓o�b�t�@���|�C���g
	int				PtrOutBuff;			//�g�p���̏o�̓o�b�t�@���|�C���g
	bool			BufferFilled;		//���̓o�b�t�@����t�ɂȂ������ǂ���
	bool			Waiting;			//���͑҂�


	int				Eff;				//�G�t�F�N�g�̑I��
	HFONT			hFontDefault;		//�f�t�H���g�̃t�H���g
	HFONT			hFontName;			//�G�t�F�N�g���̃t�H���g
	HFONT			hFontParamname;		//�p�����[�^���̃t�H���g
	HFONT			hFontParam;			//�p�����[�^�̐��l�̃t�H���g



	//�f�o�C�X����֘A�p�����[�^
	//���o�̓n���h��
	HWAVEIN			hWaveIn;			//Wave���̓n���h��
	HWAVEOUT		hWaveOut;			//Wave�o�͗p�n���h��

	WAVEFORMATEX	waveformat;			//���o�͐M���̃t�H�[�}�b�g

	//Wave�w�b�_
	PWAVEHDR		pWaveInHeader[3];	//�M�����͗pWave�w�b�_
	PWAVEHDR		pWaveOutHeader[2];	//�M���o�͗pWave�w�b�_

	//�o�b�t�@
	LPSTR			pInputBuffer[3];	//�M�����͗p�o�b�t�@
	LPSTR			pOutputBuffer[2];	//�M���o�͗p�o�b�t�@



	//�E�C���h�E�֘A
	static LRESULT CALLBACK	ChildProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);	//�E�C���h�E�v���V�[�W��
	static LRESULT CALLBACK	EffProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);		//�G�t�F�N�^�p�����[�^�ݒ�p�E�C���h�E�v���V�[�W��

	//�{�^���R���g���[��
	HWND	hInputCtrlButton;	//�X�g�b�v�{�^��
	HWND	hOutputCtrlButton;	//�X�^�[�g�{�^��

	HWND	hEffEnblButton;		//�G�t�F�N�gON/OFF�{�^��

	//�R���{�{�b�N�X
	HWND	hWndList;



	//�G�t�F�N�g�p�����[�^����p�X���C�_�[�R���g���[��
	HWND	hSlider[16];		//���16��

	//�E�C���h�E�T�C�Y
	int	Width;
	int	Height;






	//�o�C�g�f�[�^�𕂓������_�f�[�^�ɕϊ�
	int		ByteToFloat(LPSTR InData, float* OutData);

	//���������_�f�[�^���o�C�g�f�[�^�ɕϊ�
	int		FloatToByte(float* InData, LPSTR OutData);

	//�������J��
	int		ReleaseMem();

	//�R���X�g���N�^
	SOUND_CONTROLER();					//�V���O���g����





	//�T�E���h�R���g���[���֌W�̃��\�b�h
	int		InitAudio(int SamplingRate, int bitDepth, int BuffSize);	//������

	int		InputStart();												//�M�����͏����J�n
	int		OutputStart();												//�M���o�͏����J�n

	int		WaveInStop();												//���͒�~
	int		WaveOutStop();												//�o�͒�~

	int		WaveInOpened();												//Wave���̓n���h�����I�[�v�����ꂽ���̏���
	int		WaveOutOpened();											//Wave�o�̓n���h�����I�[�v�����ꂽ���̏���

	int		WaveInClosed();												//Wave���̓n���h�����N���[�Y���ꂽ���̏���
	int		WaveOutClosed();											//Wave�o�̓n���h�����N���[�Y���ꂽ���̏���

	int		InputData();												//�M�����͏���
	int		OutputData();												//�M���o�͏���

//	int		EffectOn(){EffectEnabled = TRUE;};							//�G�t�F�N�g�����J�n
//	int		EffectOff(){EffectEnabled = FALSE;};						//�G�t�F�N�g������~

	int		Reset();													//�p�����[�^��ύX���������ĊJ
	int		Reset(int SamplingRate, int bitDeapth, int BuffSize);		//�p�����[�^��ύX���ď����ĊJ



public:
	static SOUND_CONTROLER&	InitInstance();								//�C���X�^���X���֐�
	int						InitApp(HWND hWnd);							//�E�C���h�E�쐬
	int						InitEffApp(HWND hWnd);						//�G�t�F�N�^�p�����[�^���n���p�E�C���h�E�쐬
	~SOUND_CONTROLER();													//�f�X�g���N�^

	HWND			hWndChild;											//�E�C���h�E�n���h���i�q�E�C���h�E�j
	HWND			hWndEff;											//�G�t�F�N�^�p�����[�^�ݒ�p�E�C���h�E

	int	GetWidth(){return Width;};
	int	GetHeight(){return Height;};
};

#endif


