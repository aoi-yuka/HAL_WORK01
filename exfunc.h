//=============================================================================
//
// �t�@�C���F�ėp�I�Ȋ֐����܂Ƃ߂��t�@�C�� [exfunc.h]
// ����ҁFGP11A132 11 �����y��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
int RandomRange(int min, int max);
float RandomRangeFloat(float min, float max, int digit);
float Clamp(float num, float min, float max);
float Loop(float num, float min, float max);
int LoopInt(int num, int min, int max);
float Lerp(float start, float end, float t);
float EaseInQuint(float a);
int GetSign(int num);

BOOL CheckFloatRange(float data, float min, float max);
BOOL CheckFloatOverlap(float min1, float max1, float min2, float max2);

XMFLOAT3 XMFloat3Add(XMFLOAT3 x, XMFLOAT3 y);
XMFLOAT3 XMFloat3Sub(XMFLOAT3 x, XMFLOAT3 y);
BOOL XMFloat3Equal(XMFLOAT3 x, XMFLOAT3 y, float epsilon);

XMFLOAT4 QuaternionMultiply(XMFLOAT4 quat1, XMFLOAT4 quat2);
XMFLOAT3 VectorRotation(XMFLOAT3 vec, XMFLOAT4 quat);

XMFLOAT4 RotationToQuaternion(XMFLOAT3 rot);
XMFLOAT3 QuaternionToRotation(XMFLOAT4 quat);

XMFLOAT4 QuaternionRotation(XMFLOAT4 q, XMFLOAT4 quat);
XMFLOAT4 CreateQuaternion(XMFLOAT3 axis, float angle);
