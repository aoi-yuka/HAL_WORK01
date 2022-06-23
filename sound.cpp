//=============================================================================
//
// サウンド処理 [sound.cpp]
// Author : 湯川 蒼
//
//=============================================================================
#include "sound.h"
#include "main.h"

#include <math.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum
{
	LOOP = -1,
	NONLOOP = 0,
};

enum
{
	TYPE_BGM,
	TYPE_SE,
};

//*****************************************************************************
// クラス定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
IXAudio2 *xAudio2 = NULL;									// XAudio2オブジェクトへのインターフェイス
IXAudio2MasteringVoice *masteringVoice = NULL;			// マスターボイス
//IXAudio2SourceVoice *sourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
BYTE *audioData[SOUND_LABEL_MAX] = {};					// オーディオデータ
DWORD audioSize[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

// 各音素材のパラメータ 
SOUND_PARAM soundParam[SOUND_LABEL_MAX] =
{// サウンドファイル名, ループ（-1：する / 0：しない）
	{ (char*)"data/BGM/bgm_title000.wav", LOOP },		// タイトルBGM
	{ (char*)"data/BGM/bgm_tutorial000.wav", LOOP },	// チュートリアルBGM
	{ (char*)"data/BGM/bgm_game000.wav", LOOP },		// ゲーム内BGM
	{ (char*)"data/BGM/bgm_result000.wav", LOOP },	// リザルトBGM
	{ (char*)"data/SE/ground000.wav", NONLOOP },			// 着地SE
	{ (char*)"data/SE/jump000.wav", NONLOOP },			// ジャンプSE
	{ (char*)"data/SE/jumpkill000.wav", NONLOOP },		// 踏みつけSE
	{ (char*)"data/SE/nextscene000.wav", NONLOOP },		// シーン遷移SE
	{ (char*)"data/SE/nextscene001.wav", NONLOOP },		// シーン遷移SE
	{ (char*)"data/SE/playerDestroy000.wav", NONLOOP },	// プレイヤーキルSE
	{ (char*)"data/BGM/bgm_mono_title000.wav", LOOP },	// テスト用タイトルBGM（モノラル）

};

HWND globalHWnd;


//=============================================================================
// 初期化処理
//=============================================================================
BOOL InitSound(HWND hWnd)
{
	HRESULT hr;
	globalHWnd = hWnd;

	// COMライブラリの初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&xAudio2, 0);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}
	
	// マスターボイスの生成
	hr = xAudio2->CreateMasteringVoice(&masteringVoice);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if(xAudio2)
		{
			// XAudio2オブジェクトの開放
			xAudio2->Release();
			xAudio2 = NULL;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}

	// サウンドデータの初期化
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(soundParam[nCntSound].fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		if(dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &audioSize[nCntSound], &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		audioData[nCntSound] = (BYTE*)malloc(audioSize[nCntSound]);
		hr = ReadChunkData(hFile, audioData[nCntSound], audioSize[nCntSound], dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// ソースボイスの生成
		//hr = xAudio2->CreateSourceVoice(&sourceVoice[nCntSound], &(wfx.Format));
		//if(FAILED(hr))
		//{
		//	MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
		//	return FALSE;
		//}

		// バッファの値設定
		//memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		//buffer.AudioBytes = audioSize[nCntSound];
		//buffer.pAudioData = audioData[nCntSound];
		//buffer.Flags      = XAUDIO2_END_OF_STREAM;
		//buffer.LoopCount  = soundParam[nCntSound].loopCnt;

		// オーディオバッファの登録
		//sourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}
	  
	return TRUE;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSound(void)
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(sourceVoice[nCntSound])
		{
			// 一時停止
			sourceVoice[nCntSound]->Stop(0);
	
			// ソースボイスの破棄
			sourceVoice[nCntSound]->DestroyVoice();
			sourceVoice[nCntSound] = NULL;
	
			// オーディオデータの開放
			free(audioData[nCntSound]);
			audioData[nCntSound] = NULL;
		}
	}
	
	// マスターボイスの破棄
	masteringVoice->DestroyVoice();
	masteringVoice = NULL;
	
	if(xAudio2)
	{
		// XAudio2オブジェクトの開放
		xAudio2->Release();
		xAudio2 = NULL;
	}
	
	// COMライブラリの終了処理
	CoUninitialize();
}

//=============================================================================
// セグメント再生(再生中なら停止)
//=============================================================================
void PlaySound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = audioSize[label];
	buffer.pAudioData = audioData[label];
	buffer.Flags      = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount  = soundParam[label].loopCnt;

	// 状態取得
	sourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		sourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		sourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	sourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	sourceVoice[label]->Start(0);
}

//=============================================================================
// セグメント停止(ラベル指定)
//=============================================================================
void StopSound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	sourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		sourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		sourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// セグメント停止(全て)
//=============================================================================
void StopSound(void)
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(sourceVoice[nCntSound])
		{
			// 一時停止
			sourceVoice[nCntSound]->Stop(0);
		}
	}
}

//=============================================================================
// チャンクのチェック
//=============================================================================
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;
	
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	while(hr == S_OK)
	{
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch(dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if(dwChunkType == format)
		{
			*pChunkSize         = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if(dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}
	
	return S_OK;
}

//=============================================================================
// チャンクデータの読み込み
//=============================================================================
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;
	
	if(SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	return S_OK;
}

//=============================================================================
// SOUND_PANの生成
//=============================================================================
void CreatePanning(SOUND_PANNING *pan, int label)
{
	HRESULT hr;
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;

	// ソースボイスの生成
	hr = xAudio2->CreateSourceVoice(&sourceVoice[label], &(wfx.Format));
	if (FAILED(hr))
	{
		MessageBox(globalHWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
		return;
	}

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = audioSize[label];
	buffer.pAudioData = audioData[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = soundParam[label].loopCnt;

	// オーディオバッファの登録
	sourceVoice[label]->SubmitSourceBuffer(&buffer);


	pan->panning = TRUE;
}

//=============================================================================
// パンニングの更新処理(ラベル指定)
//=============================================================================
//void UpdatePanning()
//{
//	for (int i = 0; i < SOUND_LABEL_MAX; i++)
//	{
//		XAUDIO2_VOICE_STATE xa2state;
//		sourceVoice[i]->GetState(&xa2state);
//		if (xa2state.BuffersQueued > 0)	// 再生中のみ
//		{
//			if (filter[i].panning == TRUE)	// パンニング中のみ
//			{
//				if (filter[i].cameraDiff > MOVE_STEP)
//				{
//
//					CAMERA *camera = GetCamera();
//					PLAYER *player = GetPlayer();
//
//					float volumeL = 0.0f;
//					float volumeR = 0.0f;
//					int inChannels = 0;
//					int outChannels = 0;
//					float volumes[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//
//					// 距離の計算
//					XMVECTOR calc = XMVector3Length(XMLoadFloat3(&player->pos) - XMLoadFloat3(&camera->pos));
//					float length = 0.0f;
//					XMStoreFloat(&length, calc);
//
//
//					if (length <= AUDIBLE_RANGE)
//					{// 範囲内
//
//						PLAYER *player = GetPlayer();
//
//						XMFLOAT3 vecA = { 0.0f, 0.0f, 0.0f };
//						XMFLOAT3 vecB = { 0.0f, 0.0f, 0.0f };
//						XAUDIO2_VOICE_DETAILS details;
//						sourceVoice[i]->GetVoiceDetails(&details);	// ソースボイスの情報を取得
//						inChannels = details.InputChannels;
//						masteringVoice->GetVoiceDetails(&details);		// マスターボイスの情報を取得
//						outChannels = details.InputChannels;
//
//						// 2つのベクトルの算出
//						XMVECTOR calcVecA = XMLoadFloat3(&player->pos) - XMLoadFloat3(&camera->pos);
//						XMVECTOR calcVecB = XMLoadFloat3(&soundParams[i].pos) - XMLoadFloat3(&camera->pos);
//						XMStoreFloat3(&vecA, calcVecA);
//						XMStoreFloat3(&vecB, calcVecB);
//
//						// 角の計算
//						float ang = atan2f(-vecA.x + vecB.x, -vecA.z + vecB.z) - camera->rot.y;
//
//						// 270度以上を変換
//						if (ang >= XM_PI * 1.5f)
//						{
//							ang -= 2.0f * XM_PI;
//						}
//						// 90より大きい且つ270未満を変換
//						if ((ang > XM_PI * 0.5f) && (ang < XM_PI * 1.5f))
//						{
//							ang = XM_PI - ang;
//						}
//
//						ang = (ang + XM_PI * 0.5f) *0.5f;
//
//						// 変換
//						float t = length / AUDIBLE_RANGE;
//						volumeL = cosf(ang) * (1.0f - t);
//						volumeR = sinf(ang) * (1.0f - t);
//
//						// いい感じに調整(人間の耳では0.5fにしても半分に聞こえない)
//						volumeL = volumeL * volumeL;
//						volumeR = volumeR * volumeR;
//
//#ifdef _DEBUG
//						PrintDebugProc("angle: %f\n", ang);
//						PrintDebugProc("volumeL: %f\n", volumeL);
//						PrintDebugProc("volumeR: %f\n", volumeR);
//#endif
//
//						// 左右のボリュームへ代入
//						volumes[0] = { volumeL };
//						volumes[3] = { volumeR };
//
//						sourceVoice[i]->SetOutputMatrix(NULL, inChannels, outChannels, volumes);
//					}
//					else
//					{
//						volumeL = volumeR = 0.0f;
//						sourceVoice[i]->SetOutputMatrix(NULL, inChannels, outChannels, volumes);
//					}
//				}
//			}
//		}
//		else
//		{
//			filter[i].panning = FALSE;
//		}
//	}
//
//
//}
