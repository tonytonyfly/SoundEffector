/**		@author 堀部　悠平
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


//新しいエフェクト作ったら種類の数を増やしてください
//////////////////////////////////////////
//エフェクトの種類
const int	N_EFF	= 3;
//////////////////////////////////////////

//コントロール類のメッセージ
const int	IDC_BUTTON	= WM_APP;
const int	IDC_SLIDER	= WM_APP + 16;
const int	IDC_LIST1	= IDC_SLIDER + 16;


//バイトデータと浮動小数点データ変換用構造体
struct WORD_DATA{
	unsigned char	Hi;
	unsigned char	Lo;
};

union DATA_CONV{
	struct WORD_DATA	Word;
	short				Integer;
};


//スライダーに必要な情報
struct PARAM_INFO{
	//値の上限、加減、初期値
	int		Minimum;
	int		Maximum;
	int		Value;

	//パラメータの名前（必ず半角16文字以内で）
	char	Name[16];
};



//信号処理クラス
class SOUND_EFFECTOR{
private:
	int	BuffSize;		//バッファサイズ



	//エフェクトの名前との対応
	char	EffName[N_EFF][64];

	//エフェクトごとにパラメータがいくつあるかを管理
	int		NumParam[N_EFF];

	//エフェクト用パラメータの初期設定
	struct PARAM_INFO	ParamInfo[N_EFF][16];

	//パラメータを格納
	int		Param[16];



	int	Thru(float* InBuff, float* OutBuff);		//エフェクトなし
	int	MovAvg(float* InBuff, float* OutBuff);		//移動平均フィルタリング
	int	Distortion(float *InBuff, float *OutBuff);	//ディストーション

	//エフェクトへのポインタ（エフェクトを切り替えるために必要）
	int	(SOUND_EFFECTOR::*pEffect[N_EFF])(float *, float *);

public:
		SOUND_EFFECTOR();										//コンストラクタ
	int	SetBuffSize(int BuffSize);

	int	Effect(float *InBuff, float *OutBuff, int SelEff);


	friend class SOUND_CONTROLER;
};

//音声処理用クラス
class SOUND_CONTROLER{
private:
	//音声信号情報
	int				SamplingRate;		//サンプリング周波数
	int				BitDepth;			//量子化ビット数
	int				BuffSize;			//バッファサイズ（入出力）




	//エフェクト処理
	SOUND_EFFECTOR	Effector;			//信号処理インタフェース
	float			*WaveInBuffer;		//信号処理用浮動小数点バッファ
	float			*WaveOutBuffer;		//加工後の信号波形用バッファ
	union DATA_CONV	DataConv;			//データ変換インタフェース




	//状態制御
	bool			WaveInRunning;		//動作中のフラグ
	bool			WaveOutRunning;		//
	bool			isInitialized;		//初期化済みのフラグ（メモリ関連）
	//bool			EffectEnabled;		//エフェクト処理中か否か
	int				PtrInBuff;			//使用中の入力バッファをポイント
	int				PtrOutBuff;			//使用中の出力バッファをポイント
	bool			BufferFilled;		//入力バッファが一杯になったかどうか
	bool			Waiting;			//入力待ち


	int				Eff;				//エフェクトの選択
	HFONT			hFontDefault;		//デフォルトのフォント
	HFONT			hFontName;			//エフェクト名のフォント
	HFONT			hFontParamname;		//パラメータ名のフォント
	HFONT			hFontParam;			//パラメータの数値のフォント



	//デバイス操作関連パラメータ
	//入出力ハンドル
	HWAVEIN			hWaveIn;			//Wave入力ハンドル
	HWAVEOUT		hWaveOut;			//Wave出力用ハンドル

	WAVEFORMATEX	waveformat;			//入出力信号のフォーマット

	//Waveヘッダ
	PWAVEHDR		pWaveInHeader[3];	//信号入力用Waveヘッダ
	PWAVEHDR		pWaveOutHeader[2];	//信号出力用Waveヘッダ

	//バッファ
	LPSTR			pInputBuffer[3];	//信号入力用バッファ
	LPSTR			pOutputBuffer[2];	//信号出力用バッファ



	//ウインドウ関連
	static LRESULT CALLBACK	ChildProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);	//ウインドウプロシージャ
	static LRESULT CALLBACK	EffProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);		//エフェクタパラメータ設定用ウインドウプロシージャ

	//ボタンコントロール
	HWND	hInputCtrlButton;	//ストップボタン
	HWND	hOutputCtrlButton;	//スタートボタン

	HWND	hEffEnblButton;		//エフェクトON/OFFボタン

	//コンボボックス
	HWND	hWndList;



	//エフェクトパラメータ制御用スライダーコントロール
	HWND	hSlider[16];		//上限16個

	//ウインドウサイズ
	int	Width;
	int	Height;






	//バイトデータを浮動小数点データに変換
	int		ByteToFloat(LPSTR InData, float* OutData);

	//浮動小数点データをバイトデータに変換
	int		FloatToByte(float* InData, LPSTR OutData);

	//メモリ開放
	int		ReleaseMem();

	//コンストラクタ
	SOUND_CONTROLER();					//シングルトン化





	//サウンドコントロール関係のメソッド
	int		InitAudio(int SamplingRate, int bitDepth, int BuffSize);	//初期化

	int		InputStart();												//信号入力処理開始
	int		OutputStart();												//信号出力処理開始

	int		WaveInStop();												//入力停止
	int		WaveOutStop();												//出力停止

	int		WaveInOpened();												//Wave入力ハンドルがオープンされた時の処理
	int		WaveOutOpened();											//Wave出力ハンドルがオープンされた時の処理

	int		WaveInClosed();												//Wave入力ハンドルがクローズされた時の処理
	int		WaveOutClosed();											//Wave出力ハンドルがクローズされた時の処理

	int		InputData();												//信号入力処理
	int		OutputData();												//信号出力処理

//	int		EffectOn(){EffectEnabled = TRUE;};							//エフェクト処理開始
//	int		EffectOff(){EffectEnabled = FALSE;};						//エフェクト処理停止

	int		Reset();													//パラメータを変更せず処理再開
	int		Reset(int SamplingRate, int bitDeapth, int BuffSize);		//パラメータを変更して処理再開



public:
	static SOUND_CONTROLER&	InitInstance();								//インスタンス化関数
	int						InitApp(HWND hWnd);							//ウインドウ作成
	int						InitEffApp(HWND hWnd);						//エフェクタパラメータ引渡し用ウインドウ作成
	~SOUND_CONTROLER();													//デストラクタ

	HWND			hWndChild;											//ウインドウハンドル（子ウインドウ）
	HWND			hWndEff;											//エフェクタパラメータ設定用ウインドウ

	int	GetWidth(){return Width;};
	int	GetHeight(){return Height;};
};

#endif


