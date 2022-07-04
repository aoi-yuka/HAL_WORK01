#pragma once

#include "main.h"
#include "model.h"
#include "collision01.h"


class FieldObject
{
private:
	BOOL _inUse;		// �g�p���t���O
	BOOL _isLoadModel;	// ���f���ǂݍ��ݍς݃t���O
	BOOL _inCollidable; // �����蔻�����邩�̃t���O

	XMFLOAT3 _pos;		// ���[���h���W
	XMFLOAT3 _rot;		// ���[���h��]
	XMFLOAT3 _scl;		// ���[���h�g�k

	XMFLOAT3 _oldPos; // �O�t���[���̍��W,�����蔻��𓮂������Ɏg��
	XMFLOAT3 _oldScl; // �O�t���[���̑傫��
	XMFLOAT3 _oldRot; // �O�t���[���̉�]
	
	DX11_MODEL _model;	// ���f���f�[�^

	Shape *_shape; // �����蔻��̌`��̃|�C���^

public:
	void SetInUse(BOOL inUse) { _inUse = inUse; };
	BOOL GetInUse(void) { return _inUse; };
	XMFLOAT3 GetPos(void) { return _pos; };
	XMFLOAT3 GetRot(void) { return _rot; };
	XMFLOAT3 GetScl(void) { return _scl; };

	void SetSRT(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scl);


	FieldObject();
	~FieldObject();		// �f�X�g���N�^

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
