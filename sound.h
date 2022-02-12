//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 湯川 蒼
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define _USE_MATH_DEFINES

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_title000,		// タイトルBGM
	SOUND_LABEL_BGM_tutorial000,	// チュートリアルBGM
	SOUND_LABEL_BGM_game000,		// ゲーム内BGＭ
	SOUND_LABEL_BGM_result000,		// リザルトBGM
	SOUND_LABEL_SE_ground000,		// 着地SE
	SOUND_LABEL_SE_jump000,			// ジャンプSE
	SOUND_LABEL_SE_jumpkill000,		// 踏みつけSE
	SOUND_LABEL_SE_nextscene000,	// シーン遷移SE
	SOUND_LABEL_SE_nextscene001,	// シーン遷移SE
	SOUND_LABEL_SE_playerDestroy000,// プレイヤーキルSE

	SOUND_LABEL_BGM_MONO_title000,	// テスト用タイトルBGM（モノラル）

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void FadeSound(float targetVolume, float targetTime);
void UpdateFadeSound(float deltaTime, float targetVolume, float targetTime);

