#pragma once

#include "main.h"
#include "model.h"
#include "collision01.h"


class FieldObject
{
private:
	BOOL _inUse;		// 使用中フラグ
	BOOL _isLoadModel;	// モデル読み込み済みフラグ
	BOOL _inCollidable; // 当たり判定を取るかのフラグ

	XMFLOAT3 _pos;		// ワールド座標
	XMFLOAT3 _rot;		// ワールド回転
	XMFLOAT3 _scl;		// ワールド拡縮

	XMFLOAT3 _oldPos; // 前フレームの座標,当たり判定を動かす時に使う
	XMFLOAT3 _oldScl; // 前フレームの大きさ
	XMFLOAT3 _oldRot; // 前フレームの回転
	
	DX11_MODEL _model;	// モデルデータ

	Shape *_shape; // 当たり判定の形状のポインタ

public:
	void SetInUse(BOOL inUse) { _inUse = inUse; };
	BOOL GetInUse(void) { return _inUse; };
	XMFLOAT3 GetPos(void) { return _pos; };
	XMFLOAT3 GetRot(void) { return _rot; };
	XMFLOAT3 GetScl(void) { return _scl; };

	void SetSRT(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scl);


	FieldObject();
	~FieldObject();		// デストラクタ

	bool GetInCollidable(void) { return _inCollidable; };
	void SetInCollidable(bool b) { _inCollidable = b; };

	Shape *GetShape(void) { return _shape; };
	void SetShape(Shape *shape) { *_shape = *shape; };

	XMFLOAT3 GetOldPos(void) { return _oldPos; };
	void SetOldPos(XMFLOAT3 pos) { _oldPos = pos; };

	XMFLOAT3 GetOldScl(void) { return _oldScl; };
	void SetOldScl(XMFLOAT3 scl) { _oldScl = scl; };

	XMFLOAT3 GetOldRot(void) { return _oldRot; };
	void SetOldRot(XMFLOAT3 rot) { _oldRot = rot; };

	void SetModel(DX11_MODEL model) { _model = model; };

	void NewShape(int i);
	void DeleteShape(void)
	{
		delete _shape;
		_shape = NULL;
	};
};
