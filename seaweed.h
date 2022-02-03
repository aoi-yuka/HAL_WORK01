//=============================================================================
//
// 海藻の処理 [seaweed.h]
// Author : 湯川 蒼
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSeaweed(void);
void UninitSeaweed(void);
void UpdateSeaweed(void);
void DrawSeaweed(void);

int SetSeaweed(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


