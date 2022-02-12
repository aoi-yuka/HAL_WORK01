//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : ���� ��
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

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

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void FadeSound(float targetVolume, float targetTime);
void UpdateFadeSound(float deltaTime, float targetVolume, float targetTime);

