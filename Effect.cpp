/**		@author 堀部　悠平
		Effector.cpp
*/

#include"SoundEffector.h"


//コンストラクタ
SOUND_EFFECTOR::SOUND_EFFECTOR()
{
//↓新しいエフェクト作ったらここに追加してください

//////////////////////////////////////////////////
	//エフェクトの名前
	strcpy(EffName[0], "Thru");				//Thru
	strcpy(EffName[1], "Moving Average");	//移動平均フィルタ
	strcpy(EffName[2], "Distortion");		//ディストーション
//////////////////////////////////////////////////

//////////////////////////////////////////////////
	//エフェクタへのポインタを初期化
	pEffect[0]	= &SOUND_EFFECTOR::Thru;
	pEffect[1]	= &SOUND_EFFECTOR::MovAvg;
	pEffect[2]	= &SOUND_EFFECTOR::Distortion;
//////////////////////////////////////////////////

//////////////////////////////////////////////////
	//エフェクトに与えるパラメータの情報
	NumParam[0]	= 0;	//Thruはパラメータ不要

	//移動平均フィルタのパラメータ情報
	NumParam[1]	= 1;
	//移動平均フィルタのパラメータの情報
	strcpy(ParamInfo[1][0].Name, "CutOffFreq");
	ParamInfo[1][0].Minimum			= 1;
	ParamInfo[1][0].Maximum			= 64;
	ParamInfo[1][0].Value			= 16;

	//ディストーションのパラメータ情報
	NumParam[2]	= 2;
	//ゲイン
	strcpy(ParamInfo[2][0].Name, "Gain");
	ParamInfo[2][0].Minimum			= 0;
	ParamInfo[2][0].Maximum			= 32;
	ParamInfo[2][0].Value			= 5;

	//スレッショルド
	strcpy(ParamInfo[2][1].Name, "Threshold");
	ParamInfo[2][1].Minimum			= 0;
	ParamInfo[2][1].Maximum			= 32767;
	ParamInfo[2][1].Value			= 27000;
//////////////////////////////////////////////////
}

//エフェクト呼び出し関数
int	SOUND_EFFECTOR::Effect(float *InBuff, float *OutBuff, int SelEff)
{
	//エフェクト処理
	(this->*pEffect[SelEff])(InBuff, OutBuff);

	return 0;
}

//エフェクトなし（通過）
int	SOUND_EFFECTOR::Thru(float* InBuff, float* OutBuff)
{
	//データをそのままコピ－
	for(int i = 0; i < BuffSize; i++)
		OutBuff[i]	= InBuff[BuffSize + i];// + 10000 * (float)sin(((double)i * 6.28) / 44);

	return 0;
}

//移動平均
int	SOUND_EFFECTOR::MovAvg(float *InBuff, float *OutBuff)
{
	float	temp;

	//移動平均を計算
	for(int i = 0; i < BuffSize; i++)
	{
		temp	= 0.0;
		for(int j = Param[0]; j >= 0; j--)
			temp	= temp + (InBuff[BuffSize + i - j] / Param[0]);

		OutBuff[i]	= temp;
	}

	return 0;
}

//オーバードライブ
int SOUND_EFFECTOR::Distortion(float *InBuff, float *OutBuff)
{
	float	temp;
	int		threshold;

	threshold	= Param[1];

	//オーバードライブ
	for(int i = 0; i < BuffSize; i++)
	{
		temp	= InBuff[BuffSize + i] * Param[0];

		if(temp > threshold)
			temp	= threshold;

		else if(temp < -threshold)
			temp	= -threshold;

		OutBuff[i]	= temp;
	}

	return 0;
}

//バッファサイズをセット
int	SOUND_EFFECTOR::SetBuffSize(int BuffSize)
{
	this->BuffSize	= BuffSize;

	return 0;
}