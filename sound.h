//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : ���� ��
//
//=============================================================================
#pragma once

#include <windows.h>
#include "main.h"
#include "xaudio2.h"						// �T�E���h�����ŕK�v
//#include <AK/SoundEngine/Common/AkSoundEngine.h>
//#include <AK/IBytes.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define _USE_MATH_DEFINES

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_title000,		// �^�C�g��BGM
	SOUND_LABEL_BGM_tutorial000,	// �`���[�g���A��BGM
	SOUND_LABEL_BGM_game000,		// �Q�[����BG�l
	SOUND_LABEL_BGM_result000,		// ���U���gBGM
	SOUND_LABEL_SE_ground000,		// ���nSE
	SOUND_LABEL_SE_jump000,			// �W�����vSE
	SOUND_LABEL_SE_jumpkill000,		// ���݂�SE
	SOUND_LABEL_SE_nextscene000,	// �V�[���J��SE
	SOUND_LABEL_SE_nextscene001,	// �V�[���J��SE
	SOUND_LABEL_SE_playerDestroy000,// �v���C���[�L��SE

	SOUND_LABEL_BGM_MONO_title000,	// �e�X�g�p�^�C�g��BGM�i���m�����j

	SOUND_LABEL_MAX,
};

enum
{
	SOURCEVOICE_CREATE_1,
	SOURCEVOICE_CREATE_2,

	SOURCEVOICE_MAX
};

enum
{
	VOICE_NUM_1 = 1,
	VOICE_NUM_2,

	VOICE_NUM_MAX
};

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class SOUND_PARAM
{
public:
	char	*fileName;		// �t�@�C����
	int		loopCnt;		// ���[�v�J�E���g
	float	defaultVolume;	// �f�t�H���g�{�����[��
	int		type;			// ���̎��
};


class SOUND_VOICE
{
public:
	IXAudio2SourceVoice	**pSourceVoice;
};

class SOUND_FADE
{
public:
	int		state;

	float	sVolume;
	float	eVolume;

	float	preVolume;

	float	sTime;
	float	eTime;
};

class SOUND_PANNING
{
public:
	BOOL		state;

	float	cameraDiff;

	XMFLOAT3 pos;
};

class SOUND_LPF
{
public:

};

class SOUND_HPF
{
public:

};

class SOUND_LPF_DYNAMIC
{
public:

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

//	 CreateSourceVoice(�T�E���h�̃��x��);

void CreateSourceVoices(SOUND_VOICE *voice, int label);

