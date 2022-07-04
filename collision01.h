//=============================================================================
//
// �t�@�C��	: �����蔻�菈�� [collsion.h]
// �쐬��	: GP11A132 11 �����y��
//			  GP12B183 30 ���M�m
//=============================================================================
#pragma once

#include "main.h"
//#include "object.h"
#include "exfunc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define OFFSET_R (1.0f) // �����f����MAYA��ł̔��a
#define OFFSET_LEN (1.0f) // �����̃��f����MAYA��ł̕ӂ̒���


#define BOX_MODEL_NAME ("data/MODEL/collider_box.obj")	// �����̂̃��f����
#define SPHERE_MODEL_NAME ("data/MODEL/collider_sphere.obj") // ���̃��f����

//*****************************************************************************
// �`���ID
// ����ɂȂɂ��ǉ�����ꍇ�ACollisionTable�ɂ��Ή�����֐���ǉ�
// FieldObject�̃N���X(NewShape)�̕����Ȃǂ��m�F����
//*****************************************************************************
enum SHAPETYPE
{
	SHAPE_TYPE_BS,
	SHAPE_TYPE_AABB,
	SHAPE_TYPE_OBB,
	SHAPE_TYPE_MAX

};




//*****************************************************************************
// �N���X��`
//*****************************************************************************

// �`��̊��N���X
class Shape
{
private:
	XMFLOAT3 _pos;

	bool _isVisible;

//protected:
//	DX11_MODEL _model;


public:
	Shape() : _pos(XMFLOAT3(0.0f, 0.0f, 0.0f)), _isVisible(true){};
	Shape(XMFLOAT3 pos) : _pos(pos), _isVisible(true) {};

	virtual ~Shape() {};

	bool GetVisible(void) { return _isVisible; };
	void SetVisible(bool flg) { _isVisible = flg; };


	//DX11_MODEL *GetModel(void) { return &_model; };

	virtual SHAPETYPE GetType(void) { return SHAPE_TYPE_MAX; };
	
	virtual BOOL CheckHit(Shape * targetShape) { return FALSE; };



	virtual XMFLOAT3 GetPos() { return _pos; };
	virtual void SetPos(XMFLOAT3 pos) { _pos = pos; };

	virtual void Draw(void) {};

};

// �����蔻��̊֐��e�[�u�����擾����֐�(����2�s�͂�������Ȃ��ƃG���[���o��)
typedef BOOL(*COLLISION)(Shape *, Shape *);
COLLISION GetCollisionTable(int selfIdx, int targetIdx);


// �o�E���f�B���O�X�t�B�A
class BS : public Shape
{
private:
	float		_r;		// ���a

public:
	BS() :_r(0.0f)
	{
		//LoadModel(SPHERE_MODEL_NAME, &_model, LOAD_MODEL_DEFAULT);
	};
	BS(XMFLOAT3 pos, float r) : Shape(pos), _r(r)
	{
		//LoadModel(SPHERE_MODEL_NAME, &_model, LOAD_MODEL_DEFAULT);
	};
	~BS()
	{
		//UnloadModel(&_model);
	};

	void SetR(float r) { _r = r; };

	float GetR(void) { return _r; };



	SHAPETYPE GetType(void) override
	{
		return SHAPE_TYPE_BS;
	};

	BOOL CheckHit(Shape *targetShape) override
	{
		return GetCollisionTable(SHAPE_TYPE_BS, targetShape->GetType())(this, targetShape);
	};

	//void Draw(void) override
	//{
	//	// ���X�^���C�U�ݒ�
	//	SetCullingMode(CULL_MODE_BACK);

	//	float scl = _r / (OFFSET_R * SCALE_MODEL);
	//	// ���[���h�}�g���N�X����
	//	XMMATRIX mtxWorld = GetWorldMatrix(GetPos(), { 0.0f, 0.0f, 0.0f }, { scl, scl, scl });

	//	SetWorldMatrix(&mtxWorld);	// �V�F�[�_�[�ɑ���

	//	DrawModel(GetModel());	// �`��

	//};
};

// ���ɉ������o�E���f�B���O�{�b�N�X
//�iAxis Aligned Bounding Box�j
class AABB : public Shape
{
private:
	XMFLOAT3 _min;
	XMFLOAT3 _max;


public:
	AABB() : _min(XMFLOAT3(0.0f, 0.0f, 0.0f)), _max(XMFLOAT3(0.0f, 0.0f, 0.0f))
	{
		//LoadModel(BOX_MODEL_NAME, &_model, LOAD_MODEL_DEFAULT);

	};
	AABB(XMFLOAT3 min, XMFLOAT3 max) : _min(min), _max(max)
	{
		//LoadModel(BOX_MODEL_NAME, &_model, LOAD_MODEL_DEFAULT);

	};
	~AABB()
	{
		//UnloadModel(&_model);
	};

	void SetMin(XMFLOAT3 min) { _min = min; };
	void SetMax(XMFLOAT3 max) { _max = max; };

	XMFLOAT3 GetMin(void) { return _min; };
	XMFLOAT3 GetMax(void) { return _max; };

	void SetPos(XMFLOAT3 pos) override
	{
		float w, h, d;
		w = _max.x - _min.x;
		h = _max.y - _min.y;
		d = _max.z - _min.z;

		_min = { pos.x - w * 0.5f, pos.y - h * 0.5f, pos.z - d * 0.5f };
		_max = { pos.x + w * 0.5f, pos.y + h * 0.5f, pos.z + d * 0.5f };

	};

	XMFLOAT3 GetPos(void) override
	{
		return {(_min.x + _max.x) * 0.5f, (_min.y + _max.y) * 0.5f , (_min.z + _max.z) * 0.5f };
	};

	SHAPETYPE GetType(void) override { return SHAPE_TYPE_AABB; };

	BOOL CheckHit(Shape *targetShape) override
	{
		return GetCollisionTable(SHAPE_TYPE_AABB, targetShape->GetType())(this, targetShape);

	};


	//void Draw(void) override
	//{
	//	// ���X�^���C�U�ݒ�
	//	SetCullingMode(CULL_MODE_BACK);

	//	XMFLOAT3 scl = {
	//		(GetMax().x - GetMin().x) / (OFFSET_LEN * SCALE_MODEL),
	//		(GetMax().y - GetMin().y) / (OFFSET_LEN * SCALE_MODEL),
	//		(GetMax().z - GetMin().z) / (OFFSET_LEN * SCALE_MODEL)
	//	};

	//	// ���[���h�}�g���N�X����
	//	XMMATRIX mtxWorld = GetWorldMatrix(GetPos(), { 0.0f, 0.0f, 0.0f }, scl);

	//	SetWorldMatrix(&mtxWorld);	// �V�F�[�_�[�ɑ���

	//	DrawModel(GetModel());	// �`��

	//}
};

// �C�ӕ����̃o�E���f�B���O�{�b�N�X
//�iOriented Bounding Box�j
class OBB : public Shape
{
private:
	XMFLOAT3	_dir[3];	// �����x�N�g��
	float		_len[3];	// ����
	XMFLOAT3	_rot;

public:
	OBB()
		:_dir{ XMFLOAT3(1.0f, 0.0f ,0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		_len{0.0f, 0.0f, 0.0f},  _rot(XMFLOAT3(0.0f, 0.0f, 0.0f))
	{
		//LoadModel(BOX_MODEL_NAME, &_model, LOAD_MODEL_DEFAULT);

	};
	~OBB()
	{
		//UnloadModel(&_model);
	};

	void SetDir(XMFLOAT3 rot)
	{
		_rot = rot;

		_dir[0] = VectorRotation(XMFLOAT3(1.0f, 0.0f, 0.0f), RotationToQuaternion(rot));
		_dir[0] = VectorRotation(XMFLOAT3(0.0f, 1.0f, 0.0f), RotationToQuaternion(rot));
		_dir[0] = VectorRotation(XMFLOAT3(0.0f, 0.0f, 1.0f), RotationToQuaternion(rot));

	};
	void SetLen(float w, float h, float d)
	{
		_len[0] = w;
		_len[1] = h;
		_len[2] = d;
	};


	XMFLOAT3 * GetDir(void) { return _dir; };
	XMFLOAT3 GetRot(void) { return _rot; };
	float * GetLen(void) { return _len; };

	SHAPETYPE GetType(void) override
	{
		return SHAPE_TYPE_OBB;
	};

	BOOL CheckHit(Shape *targetShape) override
	{
		return GetCollisionTable(SHAPE_TYPE_OBB, targetShape->GetType())(this, targetShape);

	};


	//void Draw(void) override
	//{
	//	// ���X�^���C�U�ݒ�
	//	SetCullingMode(CULL_MODE_BACK);

	//	XMFLOAT3 scl = {
	//		GetLen()[0] / (OFFSET_LEN * SCALE_MODEL),
	//		GetLen()[1] / (OFFSET_LEN * SCALE_MODEL),
	//		GetLen()[2] / (OFFSET_LEN * SCALE_MODEL)
	//	};

	//	// ���[���h�}�g���N�X����
	//	XMMATRIX mtxWorld = GetWorldMatrix(GetPos(), _rot, scl);

	//	SetWorldMatrix(&mtxWorld);	// �V�F�[�_�[�ɑ���

	//	DrawModel(&_model);	// �`��

	//}


};

// �J�v�Z��
class Capsule : public Shape
{
private:

	XMFLOAT3	_vec;	// �x�N�g��
	float		_r;		// ���a
public:
	Capsule() {};
	~Capsule() {};
	void SetVec(XMFLOAT3 vec) { _vec = vec; };
	void SetR(float r) { _r = r; };

	XMFLOAT3 GetVec(void) { return _vec; };
	float GetR(void) { return _r; };

};


// ���C�i�����j
class Ray : public Shape
{
private:

	XMFLOAT3 _vec;
public:
	Ray() {};
	~Ray() {};

	void SetVec(XMFLOAT3 vec) { _vec = vec; };

	XMFLOAT3 GetVec(void) { return _vec; };

};


// �~��
class Disk : public Shape
{
private:


	XMFLOAT3	_normal;	// �@��
	float		_r;		// ���a

public:
	Disk() {};
	~Disk() {};

	void SetNormal(XMFLOAT3 nor) { _normal = nor; };
	void SetR(float r) { _r = r; };

	XMFLOAT3 GetNormal(void) { return _normal; };
	float GetR(void) { return _r; };

};




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
float DotProduct(XMVECTOR *v1, XMVECTOR *v2);
void  CrossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2);

BOOL CollisionBSvsBS(Shape *bs1, Shape *bs2);

BOOL CollisionAABBvsRay(XMFLOAT3 *hitMin, XMFLOAT3 *hitMax, AABB aabb, Ray ray);
BOOL CollisionAABBvsRay2(float *hitMin, float *hitMax, AABB aabb, Ray ray);

BOOL CollisionAABBvsBS(Shape *aabb, Shape *bs);
BOOL CollisionBSvsAABB(Shape *bs, Shape *aabb);
BOOL CollisionAABBvsAABB(Shape *aabb1, Shape *aabb2);

BOOL CollisionOBBvsOBB(Shape *obb1, Shape *obb2);
BOOL CollisionOBBvsAABB(Shape *obb, Shape *aabb);
BOOL CollisionAABBvsOBB(Shape *aabb, Shape *obb);

BOOL CollisionOBBvsBS(Shape *obb, Shape *bs);
BOOL CollisionBSvsOBB(Shape *bs, Shape *obb);

void SetOBB(OBB *obb, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 size);
void CalcKabezuri(XMVECTOR *ret, XMFLOAT3 *dir, XMFLOAT3 *nor);

BOOL CollisionDiskvsRay(float *t, Disk disk, Ray ray);

BOOL CollisionBSvsRay(float *t, BS bs, Ray ray);
BOOL CollisionOBBvsRay(float *t, OBB obb, Ray ray);

float CollisionBSvsCAPSULE(Shape *bs, Shape *capsule);
float GetLengthPointvsRectangle(XMFLOAT3 point, XMFLOAT3 v1, XMFLOAT3 v2, XMFLOAT3 v3);


