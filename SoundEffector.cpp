/**		@author 堀部　悠平
		SoundEffector.cpp
*/

#include"SoundEffector.h"

//モジュールインスタンス
extern HINSTANCE	g_hInst;

//親ウインドウハンドル
extern HWND			g_hWnd;



//インスタンス取得
SOUND_CONTROLER&	SOUND_CONTROLER::InitInstance()
{
	static SOUND_CONTROLER	Instance;

	return Instance;
}

//コンストラクタ
SOUND_CONTROLER::SOUND_CONTROLER()
{
	//状態制御変数
	WaveInRunning	= FALSE;
	WaveOutRunning	= FALSE;
	isInitialized	= FALSE;
	//EffEnabled		= 0;
	BufferFilled	= FALSE;
	Waiting			= FALSE;

	Eff				= 0;	//Effecgt Thruをデフォルトに設定

	//バッファのポインタ
	PtrInBuff	= 0;
	PtrOutBuff	= 0;
}

//デストラクタ
SOUND_CONTROLER::~SOUND_CONTROLER()
{
	//入出力停止
	WaveInStop();
	WaveOutStop();
	ReleaseMem();

	return ;
}

//メモリ開放
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

//初期化
int	SOUND_CONTROLER::InitAudio(int SamplingRate, int bitDepth, int BuffSize)
{
	int		i;	//ループ制御

	//エフェクタのバッファサイズを初期化
	Effector.SetBuffSize(BuffSize);

	//動作中なら即リターン
	if((WaveInRunning == TRUE) || (WaveOutRunning == TRUE))
		return -1;

	isInitialized	= FALSE;

	//動作パラメータ初期化
	this->SamplingRate	= SamplingRate;
	this->BitDepth		= bitDepth;
	this->BuffSize		= BuffSize;

	//入力用Waveメモリ確保
	for(i = 0; i < 3; i++)
	{
		pWaveInHeader[i]	= new WAVEHDR;							//Waveヘッダ
		pInputBuffer[i]		= new char[(bitDepth / 8) * BuffSize];	//バッファ確保

		if((pWaveInHeader[i] == NULL) | (pInputBuffer[i] == NULL))
		{
			ReleaseMem();
			return -1;
		}
	}

	//出力用Waveメモリ確保
	for(i = 0; i < 2; i++)
	{
		pWaveOutHeader[i]	= new WAVEHDR;								//Waveヘッダ
		pOutputBuffer[i]	= new char[(bitDepth / 8) * BuffSize];		//バッファ確保

		if((pWaveOutHeader[i] == NULL) | (pOutputBuffer[i] == NULL))
		{
			ReleaseMem();
			return -1;
		}
	}

	//信号処理用浮動小数点バッファ確保
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

	//入出力信号のフォーマットを設定
	waveformat.wFormatTag		= WAVE_FORMAT_PCM;	//PCMリニアウェーブ
	waveformat.nChannels		= 1;				//チャンネル数
	waveformat.nSamplesPerSec	= SamplingRate;		//サンプリング周波数
	waveformat.nAvgBytesPerSec	= (bitDepth / 8) * SamplingRate;	//一秒当たりのデータ量
	waveformat.nBlockAlign		= (bitDepth / 8) * SamplingRate;	//左右チャンネルも考慮した一秒当たりのデータ量

	waveformat.wBitsPerSample	= bitDepth;			//１サンプル当たりのビット数
	waveformat.cbSize			= 0;

	//Waveヘッダを準備
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


//バイトデータを浮動小数点データに変換
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

//浮動小数点データをバイトデータに変換
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
		


//信号処理開始
int	SOUND_CONTROLER::InputStart()
{
	int	i;

	//初期化されてなければリターン
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

//信号出力処理開始
int	SOUND_CONTROLER::OutputStart()
{
	int	i;

	if(WaveOutRunning == TRUE)
		return 0;

	//Wave出力ハンドルをオープン
	if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveformat, (DWORD)hWndChild, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR)
	{
		waveInClose(hWaveIn);
		return -1;
	}

	//メモリをスワップアウトされないようにする
	for(i = 0; i < 2; i++)
		waveOutPrepareHeader(hWaveOut, pWaveOutHeader[i], sizeof(WAVEHDR));

///////////////////
//for(i = BuffSize; i < (2 * BuffSize); i++)
//	WaveInBuffer[i]	= (i % 60) - 30;
///////////////////

	WaveOutRunning	= TRUE;

	return 0;
}


//信号入力停止
int	SOUND_CONTROLER::WaveInStop()
{
	//入力をリセット
	if(WaveInRunning == TRUE)
	{
		waveInReset(hWaveIn);
		WaveInRunning	= FALSE;
	}

	return 0;
}

//信号出力停止
int	SOUND_CONTROLER::WaveOutStop()
{
	//出力をリセット
	if(WaveOutRunning == TRUE)
	{
		waveOutReset(hWaveOut);
		WaveOutRunning	= FALSE;
	}

	return 0;
}


//Wave入力がオープンされた時
int	SOUND_CONTROLER::WaveInOpened()
{
	HRESULT	Res;

	//バッファをスワップアウトされないように設定
	for(int i = 0; i < 3; i++)
		waveInPrepareHeader(hWaveIn, pWaveInHeader[i], sizeof(WAVEHDR));

	//入力用バッファを追加
	Res	= waveInAddBuffer(hWaveIn, pWaveInHeader[(PtrInBuff + 1) % 3], sizeof(WAVEHDR));

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave In エラー", "waveInAddBuffer1()", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "無効なハンドルが指定されました", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "ドライバが見つかりません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "メモリ割り当て失敗、もしくはロックできていません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "その他のエラー", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		return -1;
	}

	Res	= waveInAddBuffer(hWaveIn, pWaveInHeader[(PtrInBuff + 2) % 3], sizeof(WAVEHDR));

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave In エラー", "waveInAddBuffer2()", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "無効なハンドルが指定されました", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "ドライバが見つかりません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "メモリ割り当て失敗、もしくはロックできていません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "その他のエラー", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		return -1;
	}

	//入力開始
	Res = waveInStart(hWaveIn);

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave In エラー", "waveInStart()", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "無効なハンドルが指定されました", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "ドライバが見つかりません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "メモリ割り当て失敗、もしくはロックできていません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "その他のエラー", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		return -1;
	}

	return 0;
}

//Wave入力ハンドルクローズ時の処理
int	SOUND_CONTROLER::WaveInClosed()
{
	//バッファを開放
	for(int i = 0; i < 3; i++)
		waveInUnprepareHeader(hWaveIn, pWaveInHeader[i], sizeof(WAVEHDR));

	//バッファを0にクリア
	for(int i = 0; i < (BuffSize * 2); i++)
		WaveInBuffer[i]		= 0.0;

	for(int i = 0; i < BuffSize; i++)
		WaveOutBuffer[i]	= 0.0;

	return 0;
}


//信号入力処理
int	SOUND_CONTROLER::InputData()
{
	BufferFilled	= TRUE;

	//停止処理
	if(WaveInRunning == FALSE)
	{
		//ウェーブ入力ハンドルをクローズ
		waveInClose(hWaveIn);

		return 0;
	}

	//入力バッファを追加
	waveInAddBuffer(hWaveIn, pWaveInHeader[PtrInBuff], sizeof(WAVEHDR));
	
	//浮動小数点バッファにデータを追加
	ByteToFloat(pInputBuffer[(PtrInBuff + 1) % 3], WaveInBuffer);
	PtrInBuff	= (PtrInBuff + 1) % 3;

	//エフェクト処理
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

//wave出力がオープンされた時
int	SOUND_CONTROLER::WaveOutOpened()
{
	HRESULT	Res;

	//バッファをスワップアウトされないように設定
	for(int i = 0; i < 2; i++)
		waveOutPrepareHeader(hWaveOut, pWaveOutHeader[i], sizeof(WAVEHDR));

	//音声を出力
	Res = waveOutWrite(hWaveOut, pWaveOutHeader[0], sizeof(WAVEHDR));
	Res	= waveOutWrite(hWaveOut, pWaveOutHeader[1], sizeof(WAVEHDR));

	BufferFilled	= FALSE;

	if(Res != MMSYSERR_NOERROR)
	{
		MessageBox(hWndChild, "Wave Out エラー", "Wave Out Error", MB_OK | MB_ICONEXCLAMATION);
		switch(Res)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(hWndChild, "無効なハンドルが指定されました", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "ドライバが見つかりません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "メモリ割り当て失敗、もしくはロックできていません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case WAVERR_UNPREPARED:
			MessageBox(hWndChild, "Waveバッファがロックできていません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		default:
			MessageBox(hWndChild, "その他のエラー", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
	}

	PtrOutBuff		= 0;	//出力バッファを変更

	return 0;
}

//Wave出力ハンドルクローズ時の処理
int	SOUND_CONTROLER::WaveOutClosed()
{
	//バッファ開放
	for(int i = 0; i < 2; i++)
		waveOutUnprepareHeader(hWaveOut, pWaveOutHeader[i], sizeof(WAVEHDR));

	return 0;
}

//信号出力処理
int	SOUND_CONTROLER::OutputData()
{
	HRESULT	Result;

	//停止処理
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
	//信号を出力
	Result	= waveOutWrite(hWaveOut, pWaveOutHeader[PtrOutBuff], sizeof(WAVEHDR));

	if(Result != MMSYSERR_NOERROR)
	{
		switch(Result)
		{
		case MMSYSERR_NODRIVER:
			MessageBox(hWndChild, "ドライバが見つかりません", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;

		case MMSYSERR_NOMEM:
			MessageBox(hWndChild, "メモリ割り当て失敗。もしくはロックできていません。", "Error", MB_OK | MB_ICONEXCLAMATION);
			break;
		}
	}

	//生成された信号をバッファリング
	FloatToByte(WaveOutBuffer, pOutputBuffer[PtrOutBuff]);
	PtrOutBuff	= PtrOutBuff ^ 1;	//出力バッファを変更

	//Effector.MovAvg(WaveInBuffer, WaveOutBuffer);

	return 0;
}
