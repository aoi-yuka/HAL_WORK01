//=============================================================================
//
// �T�E���h���� [sound.cpp]
// Author : ���� ��
//
//=============================================================================
#include "sound.h"
#include "main.h"

#include <math.h>

//*****************************************************************************
// �}�N����`
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
// �N���X��`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
IXAudio2 *xAudio2 = NULL;									// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
IXAudio2MasteringVoice *masteringVoice = NULL;			// �}�X�^�[�{�C�X
//IXAudio2SourceVoice *sourceVoice[SOUND_LABEL_MAX] = {};	// �\�[�X�{�C�X
BYTE *audioData[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^
DWORD audioSize[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^�T�C�Y

// �e���f�ނ̃p�����[�^ 
SOUND_PARAM soundParam[SOUND_LABEL_MAX] =
{// �T�E���h�t�@�C����, ���[�v�i-1�F���� / 0�F���Ȃ��j
	{ (char*)"data/BGM/bgm_title000.wav", LOOP },		// �^�C�g��BGM
	{ (char*)"data/BGM/bgm_tutorial000.wav", LOOP },	// �`���[�g���A��BGM
	{ (char*)"data/BGM/bgm_game000.wav", LOOP },		// �Q�[����BGM
	{ (char*)"data/BGM/bgm_result000.wav", LOOP },	// ���U���gBGM
	{ (char*)"data/SE/ground000.wav", NONLOOP },			// ���nSE
	{ (char*)"data/SE/jump000.wav", NONLOOP },			// �W�����vSE
	{ (char*)"data/SE/jumpkill000.wav", NONLOOP },		// ���݂�SE
	{ (char*)"data/SE/nextscene000.wav", NONLOOP },		// �V�[���J��SE
	{ (char*)"data/SE/nextscene001.wav", NONLOOP },		// �V�[���J��SE
	{ (char*)"data/SE/playerDestroy000.wav", NONLOOP },	// �v���C���[�L��SE
	{ (char*)"data/BGM/bgm_mono_title000.wav", LOOP },	// �e�X�g�p�^�C�g��BGM�i���m�����j

};

HWND globalHWnd;


//=============================================================================
// ����������
//=============================================================================
BOOL InitSound(HWND hWnd)
{
	HRESULT hr;
	globalHWnd = hWnd;

	// COM���C�u�����̏�����
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2�I�u�W�F�N�g�̍쐬
	hr = XAudio2Create(&xAudio2, 0);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2�I�u�W�F�N�g�̍쐬�Ɏ��s�I", "�x���I", MB_ICONWARNING);

		// COM���C�u�����̏I������
		CoUninitialize();

		return FALSE;
	}
	
	// �}�X�^�[�{�C�X�̐���
	hr = xAudio2->CreateMasteringVoice(&masteringVoice);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�}�X�^�[�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);

		if(xAudio2)
		{
			// XAudio2�I�u�W�F�N�g�̊J��
			xAudio2->Release();
			xAudio2 = NULL;
		}

		// COM���C�u�����̏I������
		CoUninitialize();

		return FALSE;
	}

	// �T�E���h�f�[�^�̏�����
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// �o�b�t�@�̃N���A
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// �T�E���h�f�[�^�t�@�C���̐���
		hFile = CreateFile(soundParam[nCntSound].fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
		if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// �t�@�C���|�C���^��擪�Ɉړ�
			MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
	
		// WAVE�t�@�C���̃`�F�b�N
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
		if(dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(3)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
	
		// �t�H�[�}�b�g�`�F�b�N
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}

		// �I�[�f�B�I�f�[�^�ǂݍ���
		hr = CheckChunk(hFile, 'atad', &audioSize[nCntSound], &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
		audioData[nCntSound] = (BYTE*)malloc(audioSize[nCntSound]);
		hr = ReadChunkData(hFile, audioData[nCntSound], audioSize[nCntSound], dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return FALSE;
		}
	
		// �\�[�X�{�C�X�̐���
		//hr = xAudio2->CreateSourceVoice(&sourceVoice[nCntSound], &(wfx.Format));
		//if(FAILED(hr))
		//{
		//	MessageBox(hWnd, "�\�[�X�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
		//	return FALSE;
		//}

		// �o�b�t�@�̒l�ݒ�
		//memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		//buffer.AudioBytes = audioSize[nCntSound];
		//buffer.pAudioData = audioData[nCntSound];
		//buffer.Flags      = XAUDIO2_END_OF_STREAM;
		//buffer.LoopCount  = soundParam[nCntSound].loopCnt;

		// �I�[�f�B�I�o�b�t�@�̓o�^
		//sourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}
	  
	return TRUE;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSound(void)
{
	// �ꎞ��~
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(sourceVoice[nCntSound])
		{
			// �ꎞ��~
			sourceVoice[nCntSound]->Stop(0);
	
			// �\�[�X�{�C�X�̔j��
			sourceVoice[nCntSound]->DestroyVoice();
			sourceVoice[nCntSound] = NULL;
	
			// �I�[�f�B�I�f�[�^�̊J��
			free(audioData[nCntSound]);
			audioData[nCntSound] = NULL;
		}
	}
	
	// �}�X�^�[�{�C�X�̔j��
	masteringVoice->DestroyVoice();
	masteringVoice = NULL;
	
	if(xAudio2)
	{
		// XAudio2�I�u�W�F�N�g�̊J��
		xAudio2->Release();
		xAudio2 = NULL;
	}
	
	// COM���C�u�����̏I������
	CoUninitialize();
}

//=============================================================================
// �Z�O�����g�Đ�(�Đ����Ȃ��~)
//=============================================================================
void PlaySound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = audioSize[label];
	buffer.pAudioData = audioData[label];
	buffer.Flags      = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount  = soundParam[label].loopCnt;

	// ��Ԏ擾
	sourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		sourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		sourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	sourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	sourceVoice[label]->Start(0);
}

//=============================================================================
// �Z�O�����g��~(���x���w��)
//=============================================================================
void StopSound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	sourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		sourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		sourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// �Z�O�����g��~(�S��)
//=============================================================================
void StopSound(void)
{
	// �ꎞ��~
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(sourceVoice[nCntSound])
		{
			// �ꎞ��~
			sourceVoice[nCntSound]->Stop(0);
		}
	}
}

//=============================================================================
// �`�����N�̃`�F�b�N
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
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	while(hr == S_OK)
	{
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch(dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
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
// �`�����N�f�[�^�̓ǂݍ���
//=============================================================================
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;
	
	if(SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	return S_OK;
}

//=============================================================================
// SOUND_PAN�̐���
//=============================================================================
void CreatePanning(SOUND_PANNING *pan, int label)
{
	HRESULT hr;
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;

	// �\�[�X�{�C�X�̐���
	hr = xAudio2->CreateSourceVoice(&sourceVoice[label], &(wfx.Format));
	if (FAILED(hr))
	{
		MessageBox(globalHWnd, "�\�[�X�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
		return;
	}

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = audioSize[label];
	buffer.pAudioData = audioData[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = soundParam[label].loopCnt;

	// �I�[�f�B�I�o�b�t�@�̓o�^
	sourceVoice[label]->SubmitSourceBuffer(&buffer);


	pan->panning = TRUE;
}

//=============================================================================
// �p���j���O�̍X�V����(���x���w��)
//=============================================================================
//void UpdatePanning()
//{
//	for (int i = 0; i < SOUND_LABEL_MAX; i++)
//	{
//		XAUDIO2_VOICE_STATE xa2state;
//		sourceVoice[i]->GetState(&xa2state);
//		if (xa2state.BuffersQueued > 0)	// �Đ����̂�
//		{
//			if (filter[i].panning == TRUE)	// �p���j���O���̂�
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
//					// �����̌v�Z
//					XMVECTOR calc = XMVector3Length(XMLoadFloat3(&player->pos) - XMLoadFloat3(&camera->pos));
//					float length = 0.0f;
//					XMStoreFloat(&length, calc);
//
//
//					if (length <= AUDIBLE_RANGE)
//					{// �͈͓�
//
//						PLAYER *player = GetPlayer();
//
//						XMFLOAT3 vecA = { 0.0f, 0.0f, 0.0f };
//						XMFLOAT3 vecB = { 0.0f, 0.0f, 0.0f };
//						XAUDIO2_VOICE_DETAILS details;
//						sourceVoice[i]->GetVoiceDetails(&details);	// �\�[�X�{�C�X�̏����擾
//						inChannels = details.InputChannels;
//						masteringVoice->GetVoiceDetails(&details);		// �}�X�^�[�{�C�X�̏����擾
//						outChannels = details.InputChannels;
//
//						// 2�̃x�N�g���̎Z�o
//						XMVECTOR calcVecA = XMLoadFloat3(&player->pos) - XMLoadFloat3(&camera->pos);
//						XMVECTOR calcVecB = XMLoadFloat3(&soundParams[i].pos) - XMLoadFloat3(&camera->pos);
//						XMStoreFloat3(&vecA, calcVecA);
//						XMStoreFloat3(&vecB, calcVecB);
//
//						// �p�̌v�Z
//						float ang = atan2f(-vecA.x + vecB.x, -vecA.z + vecB.z) - camera->rot.y;
//
//						// 270�x�ȏ��ϊ�
//						if (ang >= XM_PI * 1.5f)
//						{
//							ang -= 2.0f * XM_PI;
//						}
//						// 90���傫������270������ϊ�
//						if ((ang > XM_PI * 0.5f) && (ang < XM_PI * 1.5f))
//						{
//							ang = XM_PI - ang;
//						}
//
//						ang = (ang + XM_PI * 0.5f) *0.5f;
//
//						// �ϊ�
//						float t = length / AUDIBLE_RANGE;
//						volumeL = cosf(ang) * (1.0f - t);
//						volumeR = sinf(ang) * (1.0f - t);
//
//						// ���������ɒ���(�l�Ԃ̎��ł�0.5f�ɂ��Ă������ɕ������Ȃ�)
//						volumeL = volumeL * volumeL;
//						volumeR = volumeR * volumeR;
//
//#ifdef _DEBUG
//						PrintDebugProc("angle: %f\n", ang);
//						PrintDebugProc("volumeL: %f\n", volumeL);
//						PrintDebugProc("volumeR: %f\n", volumeR);
//#endif
//
//						// ���E�̃{�����[���֑��
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
