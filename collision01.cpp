//=============================================================================
//
// �t�@�C��	: �����蔻�菈�� [collsion.cpp]
// �쐬��	: GP11A132 11 �����y��
//			  GP12B183 30 ���M�m	
//=============================================================================
#include "main.h"
#include "collision01.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
float LenSegOnSeparateAxis(XMVECTOR *sep, XMVECTOR *e1, XMVECTOR *e2, XMVECTOR* e3);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
//enum SHAPETYPE
//{
//	SHAPE_TYPE_BS,
//	SHAPE_TYPE_AABB,
//	SHAPE_TYPE_OBB,
//	SHAPE_TYPE_MAX
//
//};


//*****************************************************************************
// �����蔻��̊֐��e�[�u��
// �����`���ǉ������ꍇ�A���̃e�[�u���ɂ����̕������֐���ǉ�����
//*****************************************************************************
BOOL(*CollisionTable[SHAPE_TYPE_MAX][SHAPE_TYPE_MAX])(Shape *shape1, Shape *shape2) = {

	CollisionBSvsBS,		CollisionBSvsAABB,		CollisionBSvsOBB,
	CollisionAABBvsBS,		CollisionAABBvsAABB,	CollisionAABBvsOBB,
	CollisionOBBvsBS,		CollisionOBBvsAABB,		CollisionOBBvsOBB,


};


//=============================================================================
// �֐��̃e�[�u�����擾����
// ������g���ČĂяo���Ƃ��A���̊֐��̈�����ID�ƌĂяo�����֐��̈����̏��Ԃ͍��킹�Ă�
//=============================================================================
COLLISION GetCollisionTable(int selfIdx, int targetIdx)
{
	return CollisionTable[selfIdx][targetIdx];
}

//=============================================================================
// ����(dot)
//=============================================================================
float DotProduct(XMVECTOR *v1, XMVECTOR *v2)
{
	XMVECTOR temp = XMVector3Dot(*v1, *v2);
	float ans = 0.0f;
	XMStoreFloat(&ans, temp);

	//float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;

	return(ans);
}


//=============================================================================
// �O��(cross)
//=============================================================================
void CrossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2)
{
	*ret = XMVector3Cross(*v1, *v2);

	//ret->x = v1->y * v2->z - v1->z * v2->y;
	//ret->y = v1->z * v2->x - v1->x * v2->z;
	//ret->z = v1->x * v2->y - v1->y * v2->x;
}



//=============================================================================
// BS��BS�̓����蔻�菈��
//=============================================================================
BOOL CollisionBSvsBS(Shape *bs1, Shape *bs2)
{
	BS *s1, *s2;
	s1 = (BS *)bs1;
	s2 = (BS *)bs2;


	float len = (s1->GetR() + s2->GetR()) * (s1->GetR() + s2->GetR());	// ���a��2�悵����

	XMVECTOR temp = XMLoadFloat3(&s1->GetPos()) - XMLoadFloat3(&s2->GetPos());
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�����j

	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// ���a��2�悵������苗�����Z����
	if (len > lenSq)
	{
		return TRUE;
	}

	return FALSE;;
}


//=============================================================================
// AABB��AABB�̓����蔻�菈��
//=============================================================================
BOOL CollisionAABBvsAABB(Shape *aabb1, Shape *aabb2)
{
	AABB *b1 = (AABB *)aabb1;
	AABB *b2 = (AABB *)aabb2;


	if ((b1->GetMin().x <= b2->GetMax().x && b1->GetMax().x >= b2->GetMin().x) &&
		(b1->GetMin().y <= b2->GetMax().y && b1->GetMax().y >= b2->GetMin().y) &&
		(b1->GetMin().z <= b2->GetMax().z && b1->GetMax().z >= b2->GetMin().z))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//=============================================================================
// AABB�Ɛ����̓����蔻�菈��
//=============================================================================
BOOL CollisionAABBvsRay(XMFLOAT3 *hitMin, XMFLOAT3 *hitMax, AABB aabb, Ray ray)
{
	// ����
	float tMin, tMax;

	float t;
	XMFLOAT3 dirfrac;
	dirfrac.x = 1.0f / ray.GetVec().x;
	dirfrac.y = 1.0f / ray.GetVec().y;
	dirfrac.z = 1.0f / ray.GetVec().z;
	float t1 = (aabb.GetMin().x - ray.GetPos().x) * dirfrac.x;
	float t2 = (aabb.GetMax().x - ray.GetPos().x) * dirfrac.x;
	float t3 = (aabb.GetMin().y - ray.GetPos().y) * dirfrac.y;
	float t4 = (aabb.GetMax().y - ray.GetPos().y) * dirfrac.y;
	float t5 = (aabb.GetMin().z - ray.GetPos().z) * dirfrac.z;
	float t6 = (aabb.GetMax().z - ray.GetPos().z) * dirfrac.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	if (tmax < 0)
	{
		t = tmax;
		return FALSE;
	}

	if (tmin > tmax)
	{
		t = tmax;
		return FALSE;
	}

	t = tmin;
	if (t < 0.1f) t = 0.1f;
	XMVECTOR min = XMLoadFloat3(&ray.GetVec()) * t;
	XMVECTOR max = XMLoadFloat3(&ray.GetVec()) * t;

	XMStoreFloat3(hitMin, min);
	XMStoreFloat3(hitMax, max);

	return TRUE;	//// X���̈�̔���


	float txMin, txMax;
	if (ray.GetVec().x < 0.01f)
	{
		if (ray.GetPos().x < aabb.GetMin().x || ray.GetPos().x > aabb.GetMax().x)
		{
			return FALSE;
		}

		txMin = 0.0f;
		txMax = 1.0f;
	}
	else
	{
		float t0 = (aabb.GetMin().x - ray.GetPos().x) / ray.GetVec().x;
		float t1 = (aabb.GetMax().x - ray.GetPos().x) / ray.GetVec().x;

		if (t0 < t1)
		{
			txMin = t0;
			txMax = t1;
		}
		else
		{
			txMin = t1;
			txMax = t0;
		}

		if (txMax < 0.0f || txMin > 1.0f) return FALSE;
	}
	tMin = txMin;
	tMax = txMax;


	// Y���̈�̔���
	float tyMin, tyMax;
	if (ray.GetVec().y < 0.01f)
	{
		if (ray.GetPos().y < aabb.GetMin().y || ray.GetPos().y > aabb.GetMax().y)
		{
			return FALSE;
		}

		tyMin = 0.0f;
		tyMax = 1.0f;
	}
	else
	{
		float t0 = (aabb.GetMin().y - ray.GetPos().y) / ray.GetVec().y;
		float t1 = (aabb.GetMax().y - ray.GetPos().y) / ray.GetVec().y;

		if (t0 < t1)
		{
			tyMin = t0;
			tyMax = t1;
		}
		else
		{
			tyMin = t1;
			tyMax = t0;
		}

		if (tyMax < 0.0f || tyMin > 1.0f) return FALSE;
	}

	if (tMax < tyMin || tMin > tyMax) return FALSE;

	// X����Y��������Ă��Ȃ�
	if (tMin < tyMin) tMin = tyMin;
	if (tMax > tyMax) tMax = tyMax;


	// Z���̈�̔���
	float tzMin, tzMax;
	if (ray.GetPos().z < 0.01f)
	{
		if (ray.GetPos().z < aabb.GetMin().z || ray.GetPos().z > aabb.GetMax().z)
		{
			return FALSE;
		}

		tzMin = 0.0f;
		tzMax = 1.0f;
	}
	else
	{
		float t0 = (aabb.GetMin().z - ray.GetPos().z) / ray.GetVec().z;
		float t1 = (aabb.GetMax().z - ray.GetPos().z) / ray.GetVec().z;

		if (t0 < t1)
		{
			tzMin = t0;
			tzMax = t1;
		}
		else
		{
			tzMin = t1;
			tzMax = t0;
		}

		if (tzMax < 0.0f || tzMin > 1.0f) return FALSE;
	}

	if (tMax < tzMin || tMin > tzMax) return FALSE;

	// X, Y����Z��������Ă��Ȃ�
	if (tMin < tzMin) tMin = tzMin;
	if (tMax > tzMax) tMax = tzMax;

	// ���ʗ̈�̃`�F�b�N
	if (tMin > 1.0f || tMax < 0.0f)
	{
		return FALSE;
	}
	else
	{
		tMin = Clamp(tMin, 0.1f, 1.0f);
		XMVECTOR min = XMLoadFloat3(&ray.GetPos()) + XMLoadFloat3(&ray.GetVec()) * tMin;
		XMVECTOR max = XMLoadFloat3(&ray.GetPos()) + XMLoadFloat3(&ray.GetVec()) * tMax;

		XMStoreFloat3(hitMin, min);
		XMStoreFloat3(hitMax, max);

		return TRUE;
	}
}



BOOL CollisionAABBvsRay2(float *hitMin, float *hitMax, AABB aabb, Ray ray)
{

	if (ray.GetVec().x == 0.0f)
	{
		if (ray.GetVec().y == 0.0f && ray.GetVec().z == 0.0f) // ray��0�x�N�g��
			return FALSE;

		if (!CheckFloatRange(ray.GetPos().x, aabb.GetMin().x, aabb.GetMax().x))
			return FALSE;

		if (ray.GetVec().y == 0.0f)	// ray.GetVec().x == 0.0f, ray.GetVec().z != 0.0f
		{
			if (!CheckFloatRange(ray.GetPos().y, aabb.GetMin().y, aabb.GetMax().y))
				return FALSE;

			float t1 = (aabb.GetMin().z - ray.GetPos().z) / ray.GetVec().z;
			float t2 = (aabb.GetMax().z - ray.GetPos().z) / ray.GetVec().z;

			*hitMin = min(t1, t2);
			*hitMax = max(t1, t2);

		}
		else if (ray.GetVec().z == 0.0f) // ray.GetVec().x == 0.0f, ray.GetVec().y != 0.0f
		{
			if (!CheckFloatRange(ray.GetPos().z, aabb.GetMin().z, aabb.GetMax().z))
				return FALSE;

			float t1 = (aabb.GetMin().y - ray.GetPos().y) / ray.GetVec().y;
			float t2 = (aabb.GetMax().y - ray.GetPos().y) / ray.GetVec().y;

			*hitMin = min(t1, t2);
			*hitMax = max(t1, t2);

		}
		else	//ray.GetVec().x == 0.0f, ray.GetVec().y != 0.0f, ray.GetVec().z != 0.0f
		{
			float t1 = (aabb.GetMin().y - ray.GetPos().y) / ray.GetVec().y;
			float t2 = (aabb.GetMax().y - ray.GetPos().y) / ray.GetVec().y;

			float t3 = (aabb.GetMin().z - ray.GetPos().z) / ray.GetVec().z;
			float t4 = (aabb.GetMax().z - ray.GetPos().z) / ray.GetVec().z;




			if (!CheckFloatOverlap(min(t1, t2), max(t1, t2), min(t3, t4), max(t3, t4)))
				return FALSE;

			*hitMin = max(min(t1, t2), min(t3, t4));
			*hitMax = min(max(t1, t2), max(t3, t4));


		}
	}
	else	//ray.GetVec().x != 0.0f
	{
		if (ray.GetVec().y == 0.0f)
		{
			if (!CheckFloatRange(ray.GetPos().y, aabb.GetMin().y, aabb.GetMax().y))
				return FALSE;

			if (ray.GetVec().z == 0.0f)	//ray.GetVec().x != 0.0f, ray.GetVec().y == 0.0f
			{
				if (!CheckFloatRange(ray.GetPos().z, aabb.GetMin().z, aabb.GetMax().z))
					return FALSE;

				float t1 = (aabb.GetMin().x - ray.GetPos().x) / ray.GetVec().x;
				float t2 = (aabb.GetMax().x - ray.GetPos().x) / ray.GetVec().x;

				*hitMin = min(t1, t2);
				*hitMax = max(t1, t2);

			}
			else	// ray.x != 0.0f, ray.GetVec().y == 0.0f, ray.GetVec().z != 0.0f
			{
				float t1 = (aabb.GetMin().x - ray.GetPos().x) / ray.GetVec().x;
				float t2 = (aabb.GetMax().x - ray.GetPos().x) / ray.GetVec().x;

				float t3 = (aabb.GetMin().z - ray.GetPos().z) / ray.GetVec().z;
				float t4 = (aabb.GetMax().z - ray.GetPos().z) / ray.GetVec().z;




				if (!CheckFloatOverlap(min(t1, t2), max(t1, t2), min(t3, t4), max(t3, t4)))
					return FALSE;

				*hitMin = max(min(t1, t2), min(t3, t4));
				*hitMax = min(max(t1, t2), max(t3, t4));

			}

		}
		else	//ray.GetVec().y != 0.0f
		{
			if (ray.GetVec().z == 0.0f) // ray.x != 0.0f, ray.GetVec().y != 0.0f
			{
				if (!CheckFloatRange(ray.GetPos().z, aabb.GetMin().z, aabb.GetMax().z))
					return FALSE;

				float t1 = (aabb.GetMin().x - ray.GetPos().x) / ray.GetVec().x;
				float t2 = (aabb.GetMax().x - ray.GetPos().x) / ray.GetVec().x;

				float t3 = (aabb.GetMin().y - ray.GetPos().y) / ray.GetVec().y;
				float t4 = (aabb.GetMax().y - ray.GetPos().y) / ray.GetVec().y;




				if (!CheckFloatOverlap(min(t1, t2), max(t1, t2), min(t3, t4), max(t3, t4)))
					return FALSE;

				*hitMin = max(min(t1, t2), min(t3, t4));
				*hitMax = min(max(t1, t2), max(t3, t4));

			}
			else	// ray.x != 0.0f, ray.GetVec().y != 0.0f, ray.GetVec().z != 0.0f
			{

				float t1 = (aabb.GetMin().x - ray.GetPos().x) / ray.GetVec().x;
				float t2 = (aabb.GetMax().x - ray.GetPos().x) / ray.GetVec().x;

				float t3 = (aabb.GetMin().y - ray.GetPos().y) / ray.GetVec().y;
				float t4 = (aabb.GetMax().y - ray.GetPos().y) / ray.GetVec().y;



				if (!CheckFloatOverlap(min(t1, t2), max(t1, t2), min(t3, t4), max(t3, t4)))
					return FALSE;

				*hitMin = max(min(t1, t2), min(t3, t4));
				*hitMax = min(max(t1, t2), max(t3, t4));


				float t5 = (aabb.GetMin().z - ray.GetPos().z) / ray.GetVec().z;
				float t6 = (aabb.GetMax().z - ray.GetPos().z) / ray.GetVec().z;


				if (!CheckFloatOverlap(*hitMin, *hitMax, min(t3, t4), max(t3, t4)))
					return FALSE;


				*hitMin = max(*hitMin, min(t5, t6));
				*hitMax = min(*hitMax, max(t5, t6));


			}

		}
	}

	return TRUE;
}

//=============================================================================
// AABB��BS�̓����蔻�菈��
//=============================================================================
BOOL CollisionAABBvsBS(Shape* aabb, Shape* bs)
{
	AABB *b = (AABB *)aabb;
	BS *s = (BS *)bs;


	float lenSq = 0.0f;

	// X
	if (s->GetPos().x < b->GetMin().x)
	{
		lenSq += (s->GetPos().x - b->GetMin().x) * (s->GetPos().x - b->GetMin().x);
	}
	if (s->GetPos().x > b->GetMax().x)
	{
		lenSq += (s->GetPos().x - b->GetMax().x) * (s->GetPos().x - b->GetMax().x);
	}

	// Y
	if (s->GetPos().y < b->GetMin().y)
	{
		lenSq += (s->GetPos().y - b->GetMin().y) * (s->GetPos().y - b->GetMin().y);
	}
	if (s->GetPos().y > b->GetMax().y)
	{
		lenSq += (s->GetPos().y - b->GetMax().y) * (s->GetPos().y - b->GetMax().y);
	}

	// Z
	if (s->GetPos().z < b->GetMin().z)
	{
		lenSq += (s->GetPos().z - b->GetMin().z) * (s->GetPos().z - b->GetMin().z);
	}
	if (s->GetPos().z > b->GetMax().z)
	{
		lenSq += (s->GetPos().z - b->GetMax().z) * (s->GetPos().z - b->GetMax().z);
	}

	if (lenSq <= s->GetR() * s->GetR())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CollisionBSvsAABB(Shape *bs, Shape *aabb)
{
	return CollisionAABBvsBS(aabb, bs); 
}


//=============================================================================
// OBB��BS�̓����蔻�菈��
//=============================================================================
BOOL CollisionOBBvsBS(XMFLOAT3 *ret, OBB *obb, BS *bs)
{
	XMVECTOR hit = XMLoadFloat3(&obb->GetPos());

	XMVECTOR diff = XMLoadFloat3(&bs->GetPos()) - XMLoadFloat3(&obb->GetPos());
	
	float dist;
	for (int i = 0; i < 3; i++)
	{
		XMVECTOR dir = XMLoadFloat3(&obb->GetDir()[i]);
		dist = DotProduct(&diff, &dir);
		if (dist > obb->GetLen()[i])
		{
			dist = obb->GetLen()[i];
		}
		if (dist < -obb->GetLen()[i])
		{
			dist = -obb->GetLen()[i];
		}
		hit += dist * dir;
	}

	XMVECTOR vec = hit - XMLoadFloat3(&bs->GetPos());
	XMStoreFloat3(ret, hit);
	if (DotProduct(&vec, &vec) <= bs->GetR() * bs->GetR())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CollisionOBBvsBS(Shape *obb, Shape *bs)
{
	OBB *b = (OBB *)obb;
	BS *s = (BS *)bs;


	XMVECTOR hit = XMLoadFloat3(&b->GetPos());

	XMVECTOR diff = XMLoadFloat3(&s->GetPos()) - XMLoadFloat3(&b->GetPos());

	float dist;
	for (int i = 0; i < 3; i++)
	{
		XMVECTOR dir = XMLoadFloat3(&b->GetDir()[i]);
		dist = DotProduct(&diff, &dir);
		if (dist > b->GetLen()[i])
		{
			dist = b->GetLen()[i];
		}
		if (dist < -b->GetLen()[i])
		{
			dist = -b->GetLen()[i];
		}
		hit += dist * dir;
	}

	XMVECTOR vec = hit - XMLoadFloat3(&s->GetPos());
	if (DotProduct(&vec, &vec) <= s->GetR() * s->GetR())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CollisionBSvsOBB(Shape *bs, Shape *obb)
{
	return CollisionOBBvsBS(obb, bs);
}


//=============================================================================
// OBB��OBB�̓����蔻�菈��
//=============================================================================
BOOL CollisionOBBvsOBB(Shape *obb1, Shape *obb2)
{
	OBB *b1 = (OBB *)obb1;
	OBB *b2 = (OBB *)obb2;


	XMVECTOR NAe1 = XMLoadFloat3(&b1->GetDir()[0]);
	XMVECTOR NAe2 = XMLoadFloat3(&b1->GetDir()[1]);
	XMVECTOR NAe3 = XMLoadFloat3(&b1->GetDir()[2]);
	XMVECTOR NBe1 = XMLoadFloat3(&b2->GetDir()[0]);
	XMVECTOR NBe2 = XMLoadFloat3(&b2->GetDir()[1]);
	XMVECTOR NBe3 = XMLoadFloat3(&b2->GetDir()[2]);

	XMVECTOR Ae1 = NAe1 * b1->GetLen()[0];
	XMVECTOR Ae2 = NAe2 * b1->GetLen()[1];
	XMVECTOR Ae3 = NAe3 * b1->GetLen()[2];
	XMVECTOR Be1 = NBe1 * b2->GetLen()[0];
	XMVECTOR Be2 = NBe2 * b2->GetLen()[1];
	XMVECTOR Be3 = NBe3 * b2->GetLen()[2];

	XMVECTOR interval = XMLoadFloat3(&b1->GetPos()) - XMLoadFloat3(&b2->GetPos());

	float rA, rB, L;
	XMVECTOR cross;

	// �������FAe1
	XMStoreFloat(&rA, XMVector3Length(Ae1));
	rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	L = fabsf(DotProduct(&interval, &NAe1));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FAe2
	XMStoreFloat(&rA, XMVector3Length(Ae2));
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabsf(DotProduct(&interval, &NAe2));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FAe3
	XMStoreFloat(&rA, XMVector3Length(Ae3));
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabsf(DotProduct(&interval, &NAe3));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FBe1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	XMStoreFloat(&rB, XMVector3Length(Be1));
	L = fabsf(DotProduct(&interval, &NBe1));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FBe2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	XMStoreFloat(&rB, XMVector3Length(Be2));
	L = fabsf(DotProduct(&interval, &NBe2));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FBe3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	XMStoreFloat(&rB, XMVector3Length(Be3));
	L = fabsf(DotProduct(&interval, &NBe3));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC11
	CrossProduct(&cross, &NAe1, &NBe1);
	rA = LenSegOnSeparateAxis(&cross, &Ae2, &Ae3, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be2, &Be3, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC12
	CrossProduct(&cross, &NAe1, &NBe2);
	rA = LenSegOnSeparateAxis(&cross, &Ae2, &Ae3, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be3, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC13
	CrossProduct(&cross, &NAe1, &NBe3);
	rA = LenSegOnSeparateAxis(&cross, &Ae2, &Ae3, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be2, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC21
	CrossProduct(&cross, &NAe2, &NBe1);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae3, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be2, &Be3, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC22
	CrossProduct(&cross, &NAe2, &NBe2);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae3, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be3, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC23
	CrossProduct(&cross, &NAe2, &NBe3);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae3, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be2, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC31
	CrossProduct(&cross, &NAe3, &NBe1);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae2, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be2, &Be3, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC32
	CrossProduct(&cross, &NAe3, &NBe2);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae2, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be3, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	// �������FC33
	CrossProduct(&cross, &NAe3, &NBe3);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae2, NULL);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be2, NULL);
	L = fabsf(DotProduct(&interval, &cross));
	if (L > rA + rB)
	{
		return FALSE;
	}

	return TRUE;
}


//=============================================================================
// OBB��AABB�̓����蔻�菈��
//=============================================================================
BOOL CollisionOBBvsAABB(Shape *obb, Shape *aabb)
{
	OBB obb2;
	AABB *aabb2 = (AABB *)aabb;
	
	// AABB��OBB�ɕϊ�����
	XMFLOAT3 min = aabb2->GetMin();
	XMFLOAT3 max = aabb2->GetMax();

	XMFLOAT3 pos = XMFLOAT3( (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
	obb2.SetPos(pos);


	obb2.SetDir(XMFLOAT3(0.0f, 0.0f, 0.0f));

	float len[3] = {max.x - min.x, max.y - min.y, max.z - min.z};
	obb2.SetLen(len[0], len[1], len[2]);
	
	return CollisionOBBvsOBB(obb, &obb2);

}

BOOL CollisionAABBvsOBB(Shape *aabb, Shape *obb)
{
	return CollisionOBBvsAABB(obb, aabb);
}

//=============================================================================
// �~�Ղƃ��C�̓����蔻��
//=============================================================================
BOOL CollisionDiskvsRay(float *t, Disk disk, Ray ray)
{

	XMVECTOR v1, v2, v3, v4, v5;

	v1 = XMLoadFloat3(&ray.GetPos());
	v2 = XMLoadFloat3(&disk.GetPos());
	v3 = XMLoadFloat3(&ray.GetVec());
	v4 = XMLoadFloat3(&disk.GetNormal());
	v5 = v2 - v1;

	*t = DotProduct(&v5, &v4) / DotProduct(&v3, &v4);

	if (*t <= 0.0f)
		return FALSE;

	float len;
	XMVECTOR l;
	l = XMVector3Length(v1 + *t * v3 - v2);
	XMStoreFloat(&len, l);

	if (len > disk.GetR())
		return FALSE;
	else
		return TRUE;
}


//=============================================================================
// OBB�ƃ��C�̓����蔻��
//=============================================================================
BOOL CollisionOBBvsRay(float *t, OBB obb, Ray ray)
{
	Ray ray2;
	AABB aabb;

	// OBB�����_���S��AABB�ɕϊ�����
	aabb.SetMin({ obb.GetLen()[0] * (-0.5f), obb.GetLen()[1] * (-0.5f), obb.GetLen()[2] * (-0.5f) });
	aabb.SetMax({ obb.GetLen()[0] * 0.5f, obb.GetLen()[1] * 0.5f, obb.GetLen()[2] * 0.5f });

	// ray������ɍ��킹�ĕϊ�����
	XMFLOAT3 rot = { obb.GetRot().x, obb.GetRot().y, obb.GetRot().z };
	XMFLOAT3 rotInv = { -rot.x, -rot.y, -rot.z };
	ray2.SetVec(VectorRotation(ray.GetVec(), RotationToQuaternion(rotInv)));

	XMFLOAT3 pos = { ray.GetPos().x - obb.GetPos().x,
					 ray.GetPos().y - obb.GetPos().y,
					 ray.GetPos().z - obb.GetPos().z };


	XMFLOAT4 q = QuaternionMultiply(RotationToQuaternion(rotInv),
									QuaternionMultiply(XMFLOAT4(pos.x, pos.y, pos.z, 0.0f),
														RotationToQuaternion(rot)));
	ray2.SetPos({q.x, q.y, q.z});


	float tMax;

	return CollisionAABBvsRay2(t, &tMax, aabb, ray2);
}


//=============================================================================
// BS�ƃ��C�̓����蔻��
//=============================================================================
BOOL CollisionBSvsRay(float *t, BS bs, Ray ray)
{


	XMVECTOR v1, v2, v3, v4;
	v1 = XMLoadFloat3(&bs.GetPos());
	v2 = XMLoadFloat3(&ray.GetPos());
	v3 = XMLoadFloat3(&ray.GetVec());
	v4 = v1 - v2;

	// �}��ϐ�
	*t = DotProduct(&v3, &v4) / DotProduct(&v3, &v3);


	if (*t <= 0.0f)
		return FALSE;


	// ���C��BS�̋���
	float l;
	XMVECTOR len;
	len = XMVector3Length(v2 + *t * v3 - v1);
	XMStoreFloat(&l, len);

	if (l > bs.GetR())
		return FALSE;
	else
		return TRUE;

}

//=============================================================================
// BS�ƃJ�v�Z���̓����蔻��
//=============================================================================
float CollisionBSvsCAPSULE(Shape *bs, Shape *capsule)
{
	BS *s = (BS *)bs;
	Capsule *c = (Capsule*)capsule;


	BOOL ans = FALSE;

	XMVECTOR v1, v2, v3, v4;
	v1 = XMLoadFloat3(&s->GetPos());
	v2 = XMLoadFloat3(&c->GetPos());
	v3 = XMLoadFloat3(&c->GetVec());
	v4 = v1 - v2;

	// �}��ϐ�
	float t = DotProduct(&v3, &v4) / DotProduct(&v3, &v3);
	t = Clamp(t, 0.0f, 1.0f);

	// ������BS�̋���
	float l;
	XMVECTOR len;
	len = XMVector3Length(v2 + v3 * t - v1);
	XMStoreFloat(&l, len);

	if (l < s->GetR() + c->GetR())
		ans = TRUE;

	if (ans == FALSE)
		t = -1.0f;

	return t;

}

//=============================================================================
// �_�ƒ����`�̍ŒZ�����̌v�Z
//=============================================================================
float GetLengthPointvsRectangle(XMFLOAT3 point, XMFLOAT3 v1, XMFLOAT3 v2, XMFLOAT3 v3)
{

	XMVECTOR A, P, V1, V2;

	A = XMLoadFloat3(&v1);
	P = XMLoadFloat3(&point);
	V1 = XMLoadFloat3(&v2);
	V2 = XMLoadFloat3(&v3);

	V1 -= A;
	V2 -= A;
	P -= A;

	// �}��ϐ�
	float s, t;

	s = DotProduct(&P, &V1) / DotProduct(&V1, &V1);
	t = DotProduct(&P, &V2) / DotProduct(&V2, &V2);

	s = Clamp(s, 0.0f, 1.0f);
	t = Clamp(t, 0.0f, 1.0f);

	XMVECTOR len = s * V1 + t * V2 - P;
	len = XMVector3Length(len);

	float ans;
	XMStoreFloat(&ans, len);

	return ans;
}

//=============================================================================
// OBB�𐶐�����
//=============================================================================
void SetOBB(OBB *obb, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 size)
{
	obb->GetPos() = pos;

	XMMATRIX temp = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMFLOAT4X4 mtxRot;
	XMStoreFloat4x4(&mtxRot, temp);

	obb->GetDir()[0] = { mtxRot._11, mtxRot._12, mtxRot._13 };
	obb->GetDir()[1] = { mtxRot._21, mtxRot._22, mtxRot._23 };
	obb->GetDir()[2] = { mtxRot._31, mtxRot._32, mtxRot._33 };

	obb->GetLen()[0] = size.x * 0.5f;
	obb->GetLen()[1] = size.y * 0.5f;
	obb->GetLen()[2] = size.z * 0.5f;
}


//=============================================================================
// �ǂ���x�N�g�������߂�
//=============================================================================
void CalcKabezuri(XMVECTOR *ret, XMFLOAT3 *dir, XMFLOAT3 *nor)
{
	XMVECTOR vDir = XMVector3Normalize(XMLoadFloat3(dir));
	XMVECTOR nNor = XMVector3Normalize(XMLoadFloat3(nor));
	*ret = vDir - DotProduct(&vDir, &nNor) * nNor;
}


//=============================================================================
// �������ɓ��e���ꂽ���������瓊�e�����������߂�
//=============================================================================
float LenSegOnSeparateAxis(XMVECTOR *sep, XMVECTOR *e1, XMVECTOR *e2, XMVECTOR* e3)
{
	// ���e������ = �R�̓��ς̐�Βl�̘a
	float r1 = fabsf(DotProduct(sep, e1));
	float r2 = fabsf(DotProduct(sep, e2));
	float r3 = e3 != NULL ? fabsf(DotProduct(sep, e3)) : 0;

	return r1 + r2 + r3;
}







