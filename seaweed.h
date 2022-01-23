//=============================================================================
//
// 木処理 [tree.h]
// Author : 
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


