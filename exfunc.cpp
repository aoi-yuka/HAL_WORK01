//=============================================================================
//
// ファイル	：汎用的な関数をまとめたファイル [exfunc.cpp]
// 製作者	：GP11A132 11 小峰遥一
//
//=============================================================================
#include "main.h"
#include "exfunc.h"
#include "collision01.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// 指定範囲内のランダムな値を取得する関数（整数）
// ---------------------------------------------------------------------------
//【引数】
// 最小値	min
// 最大値	max
//=============================================================================
int RandomRange(int min, int max)
{
	return rand() % (max - min + 1) + min;
}


//=============================================================================
// 指定範囲内のランダムな値を取得する関数（小数）
// ---------------------------------------------------------------------------
//【引数】
// 最小値	min
// 最大値	max
// 桁数		digit
//=============================================================================
float RandomRangeFloat(float min, float max, int digit)
{
	int tempMin = (int)min * digit;
	int tempMax = (int)max * digit;
	int rand = RandomRange(tempMin, tempMax);

	return (float)rand / digit;
}


//=============================================================================
// 値を最小・最大に収まるようにする関数
// ---------------------------------------------------------------------------
//【引数】
// 値		num
// 最小値	min
// 最大値	max
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
// 値を最小・最大でループさせる関数
// ---------------------------------------------------------------------------
//【引数】
// 値		num
// 最小値	min
// 最大値	max
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
// 線形補間関数
// ---------------------------------------------------------------------------
//【引数】
// 始値		start
// 終値		end
// 変化率	t
//=============================================================================
float Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}


//=============================================================================
// イージング関数１（途中までほぼ０、そこから一気に１へ）
// ---------------------------------------------------------------------------
//【引数】
// 変化率（０～１）	a
//=============================================================================
float EaseInQuint(float a)
{
	return a * a * a * a * a;
}


//=============================================================================
// 符号を取得する関数
//=============================================================================
int GetSign(int num)
{
	return num >= 0 ? 1 : -1;
}

//=============================================================================
// float変数が範囲内に収まっているか判定する関数
// ---------------------------------------------------------------------------
//【引数】
// data	調べたい変数
// min	範囲の最小値
// max	範囲の最大値
//=============================================================================
BOOL CheckFloatRange(float data, float min, float max)
{
	if (data >= min && data <= max)
		return TRUE;
	else return FALSE;

}


//=============================================================================
// 2つのfloat変数の範囲が重なっているか判定する関数
// ---------------------------------------------------------------------------
//【引数】
// data	調べたい変数
// min	範囲の最小値
// max	範囲の最大値
//=============================================================================
BOOL CheckFloatOverlap(float min1, float max1, float min2, float max2)
{
	if (min1 > max2 || max1 < min2) return FALSE;
	else return TRUE;
}

//=============================================================================
// XMFLOAT3の足し算
//=============================================================================
XMFLOAT3 XMFloat3Add(XMFLOAT3 x, XMFLOAT3 y)
{
	XMFLOAT3 ans;
	XMVECTOR temp = XMLoadFloat3(&x) + XMLoadFloat3(&y);
	XMStoreFloat3(&ans, temp);

	return ans;
}

//=============================================================================
// XMFLOAT3の引き算
//=============================================================================
XMFLOAT3 XMFloat3Sub(XMFLOAT3 x, XMFLOAT3 y)
{
	XMFLOAT3 ans;
	XMVECTOR temp = XMLoadFloat3(&x) - XMLoadFloat3(&y);
	XMStoreFloat3(&ans, temp);

	return ans;
}

//=============================================================================
// XMFLOAT3の比較
//=============================================================================
BOOL XMFloat3Equal(XMFLOAT3 x, XMFLOAT3 y, float epsilon)
{
	float abs1 = fabsf(x.x) + fabsf(x.y) + fabsf(x.z);
	float abs2 = fabsf(y.x) + fabsf(y.y) + fabsf(y.z);

	return (fabsf(abs1 - abs2) < epsilon);
}


//=============================================================================
// クオータニオンの掛け算
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
// ベクトルの回転
//=============================================================================
XMFLOAT3 VectorRotation(XMFLOAT3 vec, XMFLOAT4 quat)
{
	XMFLOAT4 qinv = XMFLOAT4(-quat.x, -quat.y, -quat.z, quat.w);


	XMFLOAT4 v;

	v = QuaternionMultiply(quat, QuaternionMultiply(XMFLOAT4(vec.x, vec.y, vec.z, 0.0f), qinv));

	return XMFLOAT3(v.x, v.y, v.z);

}


//=============================================================================
// rotからクォータニオンを生成する
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
// クォータニオンからrotを生成する
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
// クオータニオンの回転(Draw関数で使うやつ)、右から共役クォータニオンを掛けなくていい
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
// 回転軸と回転角からクォータニオンを生成する
//=============================================================================
XMFLOAT4 CreateQuaternion(XMFLOAT3 axis, float angle)
{
	XMFLOAT4 q1;

	q1 = XMFLOAT4(axis.x * sinf(angle * 0.5f), axis.y * sinf(angle * 0.5f), axis.z * sinf(angle * 0.5f), cosf(angle * 0.5f));

	return q1;

}
