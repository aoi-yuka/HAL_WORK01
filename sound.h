//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 湯川 蒼
//
//=============================================================================
#pragma once

#include <windows.h>
#include "main.h"
#include "xaudio2.h"						// サウンド処理で必要
//#include <AK/SoundEngine/Common/AkSoundEngine.h>
//#include <AK/IBytes.h>

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
// クラス定義
//*****************************************************************************
class SOUND_PARAM
{
public:
	char	*fileName;		// ファイル名
	int		loopCnt;		// ループカウント
	float	defaultVolume;	// デフォルトボリューム
	int		type;			// 音の種類
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
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

//	 CreateSourceVoice(サウンドのラベル);

void CreateSourceVoices(SOUND_VOICE *voice, int label);

