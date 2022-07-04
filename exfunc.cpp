//=============================================================================
//
// �t�@�C��	�F�ėp�I�Ȋ֐����܂Ƃ߂��t�@�C�� [exfunc.cpp]
// �����	�FGP11A132 11 �����y��
//
//=============================================================================
#include "main.h"
#include "exfunc.h"
#include "collision01.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

//=============================================================================
// �w��͈͓��̃����_���Ȓl���擾����֐��i�����j
// ---------------------------------------------------------------------------
//�y�����z
// �ŏ��l	min
// �ő�l	max
//=============================================================================
int RandomRange(int min, int max)
{
	return rand() % (max - min + 1) + min;
}


//=============================================================================
// �w��͈͓��̃����_���Ȓl���擾����֐��i�����j
// ---------------------------------------------------------------------------
//�y�����z
// �ŏ��l	min
// �ő�l	max
// ����		digit
//=============================================================================
float RandomRangeFloat(float min, float max, int digit)
{
	int tempMin = (int)min * digit;
	int tempMax = (int)max * digit;
	int rand = RandomRange(tempMin, tempMax);

	return (float)rand / digit;
}


//=============================================================================
// �l���ŏ��E�ő�Ɏ��܂�悤�ɂ���֐�
// ---------------------------------------------------------------------------
//�y�����z
// �l		num
// �ŏ��l	min
// �ő�l	max
//=============================================================================
float Clamp(float num, float min, float max)
{
	if (num < min)
	{
		num = min;
	}
	else if (num > max)
	{
		num = max;
	}

	return num;
}


//=============================================================================
// �l���ŏ��E�ő�Ń��[�v������֐�
// ---------------------------------------------------------------------------
//�y�����z
// �l		num
// �ŏ��l	min
// �ő�l	max
//=============================================================================
float Loop(float num, float min, float max)
{
	if (num < min)
	{
		num = max;
	}
	else if (num > max)
	{
		num = min;
	}

	return num;
}


int LoopInt(int num, int min, int max)
{
	if (num < min)
	{
		num = max;
	}
	else if (num > max)
	{
		num = min;
	}

	return num;
}

//=============================================================================
// ���`��Ԋ֐�
// ---------------------------------------------------------------------------
//�y�����z
// �n�l		start
// �I�l		end
// �ω���	t
//=============================================================================
float Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}


//=============================================================================
// �C�[�W���O�֐��P�i�r���܂łقڂO�A���������C�ɂP�ցj
// ---------------------------------------------------------------------------
//�y�����z
// �ω����i�O�`�P�j	a
//=============================================================================
float EaseInQuint(float a)
{
	return a * a * a * a * a;
}


//=============================================================================
// �������擾����֐�
//=============================================================================
int GetSign(int num)
{
	return num >= 0 ? 1 : -1;
}

//=============================================================================
// float�ϐ����͈͓��Ɏ��܂��Ă��邩���肷��֐�
// ---------------------------------------------------------------------------
//�y�����z
// data	���ׂ����ϐ�
// min	�͈͂̍ŏ��l
// max	�͈͂̍ő�l
//=============================================================================
BOOL CheckFloatRange(float data, float min, float max)
{
	if (data >= min && data <= max)
		return TRUE;
	else return FALSE;

}


//=============================================================================
// 2��float�ϐ��͈̔͂��d�Ȃ��Ă��邩���肷��֐�
// ---------------------------------------------------------------------------
//�y�����z
// data	���ׂ����ϐ�
// min	�͈͂̍ŏ��l
// max	�͈͂̍ő�l
//=============================================================================
BOOL CheckFloatOverlap(float min1, float max1, float min2, float max2)
{
	if (min1 > max2 || max1 < min2) return FALSE;
	else return TRUE;
}

//=============================================================================
// XMFLOAT3�̑����Z
//=============================================================================
XMFLOAT3 XMFloat3Add(XMFLOAT3 x, XMFLOAT3 y)
{
	XMFLOAT3 ans;
	XMVECTOR temp = XMLoadFloat3(&x) + XMLoadFloat3(&y);
	XMStoreFloat3(&ans, temp);

	return ans;
}

//=============================================================================
// XMFLOAT3�̈����Z
//=============================================================================
XMFLOAT3 XMFloat3Sub(XMFLOAT3 x, XMFLOAT3 y)
{
	XMFLOAT3 ans;
	XMVECTOR temp = XMLoadFloat3(&x) - XMLoadFloat3(&y);
	XMStoreFloat3(&ans, temp);

	return ans;
}

//=============================================================================
// XMFLOAT3�̔�r
//=============================================================================
BOOL XMFloat3Equal(XMFLOAT3 x, XMFLOAT3 y, float epsilon)
{
	float abs1 = fabsf(x.x) + fabsf(x.y) + fabsf(x.z);
	float abs2 = fabsf(y.x) + fabsf(y.y) + fabsf(y.z);

	return (fabsf(abs1 - abs2) < epsilon);
}


//=============================================================================
// �N�I�[�^�j�I���̊|���Z
//=============================================================================
XMFLOAT4 QuaternionMultiply(XMFLOAT4 quat1, XMFLOAT4 quat2)
{

	XMFLOAT3 q1, q2, ans;
	XMVECTOR Q1, Q2, Q3;

	q1 = XMFLOAT3(quat1.x, quat1.y, quat1.z);
	q2 = XMFLOAT3(quat2.x, quat2.y, quat2.z);

	Q1 = XMLoadFloat3(&q1);
	Q2 = XMLoadFloat3(&q2);

	CrossProduct(&Q3, &Q1, &Q2);

	XMStoreFloat3(&ans, (Q1 * quat2.w + Q2 * quat1.w + Q3));

	return XMFLOAT4(ans.x, ans.y, ans.z, (quat1.w * quat2.w - DotProduct(&Q1, &Q2)));



}

//=============================================================================
// �x�N�g���̉�]
//=============================================================================
XMFLOAT3 VectorRotation(XMFLOAT3 vec, XMFLOAT4 quat)
{
	XMFLOAT4 qinv = XMFLOAT4(-quat.x, -quat.y, -quat.z, quat.w);


	XMFLOAT4 v;

	v = QuaternionMultiply(quat, QuaternionMultiply(XMFLOAT4(vec.x, vec.y, vec.z, 0.0f), qinv));

	return XMFLOAT3(v.x, v.y, v.z);

}


//=============================================================================
// rot����N�H�[�^�j�I���𐶐�����
//=============================================================================
XMFLOAT4 RotationToQuaternion(XMFLOAT3 rot)
{

	XMFLOAT3 axis;
	float angle;

	XMStoreFloat(&angle, XMVector3Length(XMLoadFloat3(&rot)));
	XMStoreFloat3(&axis, XMVector3Normalize(XMLoadFloat3(&rot)));

	XMFLOAT4 ans = XMFLOAT4(axis.x * sinf(angle * 0.5f), axis.y * sinf(angle * 0.5f), axis.z * sinf(angle * 0.5f), cosf(angle * 0.5f));

	return ans;
}

//=============================================================================
// �N�H�[�^�j�I������rot�𐶐�����
//=============================================================================
XMFLOAT3 QuaternionToRotation(XMFLOAT4 quat)
{
	float angle = acosf(quat.w) * 2.0f;

	if (angle == 0.0f)
	{
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	XMFLOAT3 axis = XMFLOAT3(quat.x / sinf(angle * 0.5f), quat.y / sinf(angle * 0.5f), quat.z / sinf(angle * 0.5f));


	return XMFLOAT3(axis.x * angle, axis.y * angle, axis.z * angle);
}



//=============================================================================
// �N�I�[�^�j�I���̉�](Draw�֐��Ŏg�����)�A�E���狤���N�H�[�^�j�I�����|���Ȃ��Ă���
//=============================================================================
XMFLOAT4 QuaternionRotation(XMFLOAT4 q, XMFLOAT4 quat)
{
	XMFLOAT4 q1;



	q1 = QuaternionMultiply(q, quat);

	XMVECTOR Q = XMLoadFloat4(&q1);
	Q = XMVector4Normalize(Q);
	XMStoreFloat4(&q1, Q);

	return q1;

}


//=============================================================================
// ��]���Ɖ�]�p����N�H�[�^�j�I���𐶐�����
//=============================================================================
XMFLOAT4 CreateQuaternion(XMFLOAT3 axis, float angle)
{
	XMFLOAT4 q1;

	q1 = XMFLOAT4(axis.x * sinf(angle * 0.5f), axis.y * sinf(angle * 0.5f), axis.z * sinf(angle * 0.5f), cosf(angle * 0.5f));

	return q1;

}
