/**		@author �x���@�I��
		Effector.cpp
*/

#include"SoundEffector.h"


//�R���X�g���N�^
SOUND_EFFECTOR::SOUND_EFFECTOR()
{
//���V�����G�t�F�N�g������炱���ɒǉ����Ă�������

//////////////////////////////////////////////////
	//�G�t�F�N�g�̖��O
	strcpy(EffName[0], "Thru");				//Thru
	strcpy(EffName[1], "Moving Average");	//�ړ����σt�B���^
	strcpy(EffName[2], "Distortion");		//�f�B�X�g�[�V����
//////////////////////////////////////////////////

//////////////////////////////////////////////////
	//�G�t�F�N�^�ւ̃|�C���^��������
	pEffect[0]	= &SOUND_EFFECTOR::Thru;
	pEffect[1]	= &SOUND_EFFECTOR::MovAvg;
	pEffect[2]	= &SOUND_EFFECTOR::Distortion;
//////////////////////////////////////////////////

//////////////////////////////////////////////////
	//�G�t�F�N�g�ɗ^����p�����[�^�̏��
	NumParam[0]	= 0;	//Thru�̓p�����[�^�s�v

	//�ړ����σt�B���^�̃p�����[�^���
	NumParam[1]	= 1;
	//�ړ����σt�B���^�̃p�����[�^�̏��
	strcpy(ParamInfo[1][0].Name, "CutOffFreq");
	ParamInfo[1][0].Minimum			= 1;
	ParamInfo[1][0].Maximum			= 64;
	ParamInfo[1][0].Value			= 16;

	//�f�B�X�g�[�V�����̃p�����[�^���
	NumParam[2]	= 2;
	//�Q�C��
	strcpy(ParamInfo[2][0].Name, "Gain");
	ParamInfo[2][0].Minimum			= 0;
	ParamInfo[2][0].Maximum			= 32;
	ParamInfo[2][0].Value			= 5;

	//�X���b�V�����h
	strcpy(ParamInfo[2][1].Name, "Threshold");
	ParamInfo[2][1].Minimum			= 0;
	ParamInfo[2][1].Maximum			= 32767;
	ParamInfo[2][1].Value			= 27000;
//////////////////////////////////////////////////
}

//�G�t�F�N�g�Ăяo���֐�
int	SOUND_EFFECTOR::Effect(float *InBuff, float *OutBuff, int SelEff)
{
	//�G�t�F�N�g����
	(this->*pEffect[SelEff])(InBuff, OutBuff);

	return 0;
}

//�G�t�F�N�g�Ȃ��i�ʉ߁j
int	SOUND_EFFECTOR::Thru(float* InBuff, float* OutBuff)
{
	//�f�[�^�����̂܂܃R�s�|
	for(int i = 0; i < BuffSize; i++)
		OutBuff[i]	= InBuff[BuffSize + i];// + 10000 * (float)sin(((double)i * 6.28) / 44);

	return 0;
}

//�ړ�����
int	SOUND_EFFECTOR::MovAvg(float *InBuff, float *OutBuff)
{
	float	temp;

	//�ړ����ς��v�Z
	for(int i = 0; i < BuffSize; i++)
	{
		temp	= 0.0;
		for(int j = Param[0]; j >= 0; j--)
			temp	= temp + (InBuff[BuffSize + i - j] / Param[0]);

		OutBuff[i]	= temp;
	}

	return 0;
}

//�I�[�o�[�h���C�u
int SOUND_EFFECTOR::Distortion(float *InBuff, float *OutBuff)
{
	float	temp;
	int		threshold;

	threshold	= Param[1];

	//�I�[�o�[�h���C�u
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

//�o�b�t�@�T�C�Y���Z�b�g
int	SOUND_EFFECTOR::SetBuffSize(int BuffSize)
{
	this->BuffSize	= BuffSize;

	return 0;
}