/**		@author �x���@�I��
		SoundEffector.cpp
*/

#include"SoundEffector.h"

//���W���[���C���X�^���X
extern HINSTANCE	g_hInst;

//�e�E�C���h�E�n���h��
extern HWND			g_hWnd;



//�C���X�^���X�擾
SOUND_CONTROLER&	SOUND_CONTROLER::InitInstance()
{
	static SOUND_CONTROLER	Instance;

	return Instance;
}

//�R���X�g���N�^
SOUND_CONTROLER::SOUND_CONTROLER()
{
	//��Ԑ���ϐ�
	WaveInRunning	= FALSE;
	WaveOutRunning	= FALSE;
	isInitialized	= FALSE;
	//EffEnabled		= 0;
	BufferFilled	= FALSE;
	Waiting			= FALSE;

	Eff				= 0;	//Effecgt Thru���f�t�H���g�ɐݒ�

	//�o�b�t�@�̃|�C���^
	PtrInBuff	= 0;
	PtrOutBuff	= 0;
}

//�f�X�g���N�^
SOUND_CONTROLER::~SOUND_CONTROLER()
{
	//���o�͒�~
	WaveInStop();
	WaveOutStop();
	ReleaseMem();

	return ;
}

//�������J��
int SOUND_CONTROLER::ReleaseMem()
{
	int	i;

	for(i = 0; i < 3; i++)
	{
		if(pWaveInHeader[i] != NULL)
			delete		pWaveInHeader[i];

		if(pInputBuffer[i] != NULL)
			delete[]	pInputBuffer[i];
	}

	for(i = 0; i < 2; i++)
	{
		if(pWaveOutHeader[i] != NULL)
			delete		pWaveOutHeader[i];

		if(pOutputBuffer[i] != NULL)
			delete[]	pOutputBuffer[i];
	}

	return 0;
}

//������
int	SOUND_CONTROLER::InitAudio(int SamplingRate, int bitDepth, int BuffSize)
{
	int		i;	//���[�v����

	//�G�t�F�N�^�̃o�b�t�@�T�C�Y��������
	Effector.SetBuffSize(BuffSize);

	//���쒆�Ȃ瑦���^�[��
	if((WaveInRunning == TRUE) || (WaveOutRunning == TRUE))
		return -1;

	isInitialized	= FALSE;

	//����p�����[�^������
	this->SamplingRate	= SamplingRate;
	this->BitDepth		= bitDepth;
	this->BuffSize		= BuffSize;

	//���͗pWave�������m��
	for(i = 0; i < 3; i++)
	{
		pWaveInHeader[i]	= new WAVEHDR;							//Wave�w�b�_
		pInputBuffer[i]		= new char[(bitDepth / 8) * BuffSize];	//�o�b�t�@�m��

		if((pWaveInHeader[i] == NULL) | (pInputBuffer[i] == NULL))
		{
			ReleaseMem();
			return -1;
		}
	}

	//�o�͗pWave�������m��
	for(i = 0; i < 2; i++)
	{
		pWaveOutHeader[i]	= new WAVEHDR;								//Wave�w�b�_
		pOutputBuffer[i]	= new char[(bitDepth / 8) * BuffSize];		//�o�b�t�@�m��

		if((pWaveOutHeader[i] == NULL) | (pOutputBuffer[i] == NULL))
		{
			ReleaseMem();
			return -1;
		}
	}

	//�M�������p���������_�o�b�t�@�m��
	WaveInBuffer	= new float[BuffSize * 2];

	if(WaveInBuffer == NULL)
	{
		ReleaseMem();
		return -1;
	}

	for(i = 0; i < (BuffSize * 2); i++)
		WaveInBuffer[i]	= 0;

	WaveOutBuffer	= new float[BuffSize];

	if(WaveOutBuffer == NULL)
	{
		ReleaseMem();
		return -1;
	}

	for(i = 0; i < BuffSize; i++)
		WaveOutBuffer[i]	= 0;

	//���o�͐M���̃t�H�[�}�b�g��ݒ�
	waveformat.wFormatTag		= WAVE_FORMAT_PCM;	//PCM���j�A�E�F�[�u
	waveformat.nChannels		= 1;				//�`�����l����
	waveformat.nSamplesPerSec	= SamplingRate;		//�T���v�����O���g��
	waveformat.nAvgBytesPerSec	= (bitDepth / 8) * SamplingRate;	//��b������̃f�[�^��
	waveformat.nBlockAlign		= (bitDepth / 8) * SamplingRate;	//���E�`�����l�����l��������b������̃f�[�^��

	waveformat.wBitsPerSample	= bitDepth;			//�P�T���v��������̃r�b�g��
	waveformat.cbSize			= 0;

	//Wave�w�b�_������
	for(i = 0; i < 3; i++)
	{
		pWaveInHeader[i]->lpData			= pInputBuffer[i];
		pWaveInHeader[i]->dwBufferLength	= BuffSize * 2;
		pWaveInHeader[i]->dwBytesRecorded	= 0;
		pWaveInHeader[i]->dwUser			= 0;
		pWaveInHeader[i]->dwFlags			= 0;
		pWaveInHeader[i]->dwLoops			= 1;
		pWaveInHeader[i]->lpNext			= NULL;
		pWaveInHeader[i]->reserved			= 0;
	}

	for(i = 0; i < 2; i++)
	{
		pWaveOutHeader[i]->lpData			= pOutputBuffer[i];
		pWaveOutHeader[i]->dwBufferLength	= BuffSize * 2;
		pWaveOutHeader[i]->dwBytesRecorded	= 0;
		pWaveOutHeader[i]->dwUser			= 0;
		pWaveOutHeader[i]->dwFlags			= 0;
		pWaveOutHeader[i]->dwLoops			= 1;
		pWaveOutHeader[i]->lpNext			= NULL;
		pWaveOutHeader[i]->reserved			= 0;
	}

	isInitialized	= TRUE;

	return 0;
}


//�o�C�g�f�[�^�𕂓������_�f�[�^�ɕϊ�
int	SOUND_CONTROLER::ByteToFloat(LPSTR InData, float* OutData)
{
	int	i;
	int	temp;

	for(i = 0; i < BuffSize; i ++)
		OutData[i]	= OutData[BuffSize + i];

	for(i = 0; i < BuffSize; i ++)
	{
		DataConv.Word.Hi	= InData[2 * i];
		DataConv.Word.Lo	= InData[(2 * i) + 1];

		temp					= (int)DataConv.Integer;
		OutData[BuffSize + i]	= (float)temp;
	}

	return 0;
}

//���������_�f�[�^���o�C�g�f�[�^�ɕϊ�
int	SOUND_CONTROLER::FloatToByte(float* InData, LPSTR OutData)
{
	int	temp;

	for(int i = 0; i < BuffSize; i ++)
	{
		temp				= (int)InData[i];
		DataConv.Integer	= (short)temp;

		OutData[2 * i]			= DataConv.Word.Hi;
		OutData[(2 * i) + 1]	= DataConv.Word.Lo;
	}

	return 0;
}
		


//�M�������J�n
int	SOUND_CONTROLER::InputStart()
{
	int	i;

	//����������ĂȂ���΃��^�[��
	if(isInitialized == FALSE)
		return -1;

	if(WaveInRunning == TRUE)
		return 0;

/*///////////////////
for(i = 0; i < BuffSize; i++)
	WaveOutBuffer[i]	= 100 * sin((i * 1760 * 3.14159265) / 44100);
*////////////////////

	if(WaveInRunning == TRUE)
		return 0;

	if(waveInOpen(&hWaveIn, WAVE_MAPPER, &waveformat, (DWORD)hWndChild, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR)
		return -1;
	
	for(i = 0; i < 3; i++)
		waveInPrepareHeader(hWaveIn, pWaveInHeader[i], sizeof(WAVEHDR));

	WaveInRunning		= TRUE;

	return 0;
}

//�M���o�͏����J�n
int	SOUND_CONTROLER::OutputStart()
{
	int	i;

	if(WaveOutRunning == TRUE)
		return 0;

	//Wave�o�̓n���h�����I�[�v��
	if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveformat, (DWORD)hWndChild, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR)
	{
		waveInClose(hWaveIn);
		return -1;
	}

	//���������X���b�v�A�E�g����Ȃ��悤�ɂ���
	for(i = 0; i < 2; i++)
		waveOutPrepareHeader(hWaveOut, pWaveOutHeader[i], sizeof(WAVEHDR));

///////////////////
//for(i = BuffSize; i < (2 * BuffSize); i++)
//	WaveInBuffer[i]	= (i % 60) - 30;
///////////////////

	WaveOutRunning	= TRUE;

	return 0;
}


//�M�����͒�~
int	SOUND_CONTROLER::WaveInStop()
{
	//���͂����Z�b�g
	if(WaveInRunning == TRUE)
	{
		waveInReset(hWaveIn);
		WaveInRunning	= FALSE;
	}

	return 0;
}

//�M���o�͒�~
int	SOUND_CONTROLER::WaveOutStop()
{
	//�o�͂����Z�b�g
	if(WaveOutRunning == TRUE)
	{
		waveOutReset(hWaveOut);
		WaveOutRunning	= FALSE;
	}

	return 0;
}


//Wave���͂��I�[�v�����ꂽ��
int	SOUND_CONTROLER::WaveInOpened()
{
	HRESULT	Res;

	//�o�b�t�@���X���b�v�A�E�g����Ȃ��悤�ɐݒ�
	for(int i = 0; i < 3; i++)
		waveInPrepareHeader(hWaveIn, pWaveInHeader[i], sizeof(WAVEHDR));

	//���͗p�o�b�t�@��ǉ�
	Res	= waveInAddBuffer(hWaveIn, pWaveInHeader[(PtrInBuff + 1) % 3], sizeof(WAVEHDR));

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave In �G���[", "waveInAddBuffer1()", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "�����ȃn���h�����w�肳��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "�h���C�o��������܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "���������蓖�Ď��s�A�������̓��b�N�ł��Ă��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "���̑��̃G���[", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		return -1;
	}

	Res	= waveInAddBuffer(hWaveIn, pWaveInHeader[(PtrInBuff + 2) % 3], sizeof(WAVEHDR));

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave In �G���[", "waveInAddBuffer2()", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "�����ȃn���h�����w�肳��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "�h���C�o��������܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "���������蓖�Ď��s�A�������̓��b�N�ł��Ă��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "���̑��̃G���[", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		return -1;
	}

	//���͊J�n
	Res = waveInStart(hWaveIn);

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave In �G���[", "waveInStart()", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "�����ȃn���h�����w�肳��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "�h���C�o��������܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "���������蓖�Ď��s�A�������̓��b�N�ł��Ă��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "���̑��̃G���[", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		return -1;
	}

	return 0;
}

//Wave���̓n���h���N���[�Y���̏���
int	SOUND_CONTROLER::WaveInClosed()
{
	//�o�b�t�@���J��
	for(int i = 0; i < 3; i++)
		waveInUnprepareHeader(hWaveIn, pWaveInHeader[i], sizeof(WAVEHDR));

	//�o�b�t�@��0�ɃN���A
	for(int i = 0; i < (BuffSize * 2); i++)
		WaveInBuffer[i]		= 0.0;

	for(int i = 0; i < BuffSize; i++)
		WaveOutBuffer[i]	= 0.0;

	return 0;
}


//�M�����͏���
int	SOUND_CONTROLER::InputData()
{
	BufferFilled	= TRUE;

	//��~����
	if(WaveInRunning == FALSE)
	{
		//�E�F�[�u���̓n���h�����N���[�Y
		waveInClose(hWaveIn);

		return 0;
	}

	//���̓o�b�t�@��ǉ�
	waveInAddBuffer(hWaveIn, pWaveInHeader[PtrInBuff], sizeof(WAVEHDR));
	
	//���������_�o�b�t�@�Ƀf�[�^��ǉ�
	ByteToFloat(pInputBuffer[(PtrInBuff + 1) % 3], WaveInBuffer);
	PtrInBuff	= (PtrInBuff + 1) % 3;

	//�G�t�F�N�g����
	Effector.Effect(WaveInBuffer, WaveOutBuffer, Eff);

/*
	if(Waiting == TRUE)
	{
		OutputData();
		Waiting	= FALSE;
	}
*/
	return 0;
}

//wave�o�͂��I�[�v�����ꂽ��
int	SOUND_CONTROLER::WaveOutOpened()
{
	HRESULT	Res;

	//�o�b�t�@���X���b�v�A�E�g����Ȃ��悤�ɐݒ�
	for(int i = 0; i < 2; i++)
		waveOutPrepareHeader(hWaveOut, pWaveOutHeader[i], sizeof(WAVEHDR));

	//�������o��
	Res = waveOutWrite(hWaveOut, pWaveOutHeader[0], sizeof(WAVEHDR));
	Res	= waveOutWrite(hWaveOut, pWaveOutHeader[1], sizeof(WAVEHDR));

	BufferFilled	= FALSE;

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave Out �G���[", "Wave Out Error", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "�����ȃn���h�����w�肳��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "�h���C�o��������܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "���������蓖�Ď��s�A�������̓��b�N�ł��Ă��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case WAVERR_UNPREPARED:
			MessageBox(hWndChild, "Wave�o�b�t�@�����b�N�ł��Ă��܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "���̑��̃G���[", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
	}

	PtrOutBuff		= 0;	//�o�̓o�b�t�@��ύX

	return 0;
}

//Wave�o�̓n���h���N���[�Y���̏���
int	SOUND_CONTROLER::WaveOutClosed()
{
	//�o�b�t�@�J��
	for(int i = 0; i < 2; i++)
		waveOutUnprepareHeader(hWaveOut, pWaveOutHeader[i], sizeof(WAVEHDR));

	return 0;
}

//�M���o�͏���
int	SOUND_CONTROLER::OutputData()
{
	HRESULT	Result;

	//��~����
	if(WaveOutRunning	== FALSE)
	{
		waveOutClose(hWaveOut);

		return 0;
	}
/*
	if((BufferFilled == FALSE) && (WaveInRunning == TRUE))
	{
		Waiting	= TRUE;
		return 1;
	}
	BufferFilled	= FALSE;
*/
	//�M�����o��
	Result	= waveOutWrite(hWaveOut, pWaveOutHeader[PtrOutBuff], sizeof(WAVEHDR));

	if(Result != MMSYSERR_NOERROR)
	{
		switch(Result)
		{
		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "�h���C�o��������܂���", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "���������蓖�Ď��s�B�������̓��b�N�ł��Ă��܂���B", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;
		}
	}

	//�������ꂽ�M�����o�b�t�@�����O
	FloatToByte(WaveOutBuffer, pOutputBuffer[PtrOutBuff]);
	PtrOutBuff	= PtrOutBuff ^ 1;	//�o�̓o�b�t�@��ύX

	//Effector.MovAvg(WaveInBuffer, WaveOutBuffer);

	return 0;
}
