//=============================================================================
//
// 影処理 [shadow.h]
// Author : 湯川 蒼
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitShadow(void);
void UninitShadow(void);
void UpdateShadow(void);
void DrawShadow(void);

int CreateShadow(XMFLOAT3 pos, float fSizeX, float fSizeZ);
void ReleaseShadow(int nIdxShadow);
void SetPositionShadow(int nIdxShadow, XMFLOAT3 pos);
void SetColorShadow(int nIdxShadow, XMFLOAT4 col);
void SetShadowScale(int nIdxShadow, XMFLOAT3 scl);
