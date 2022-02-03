//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 湯川 蒼
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/monster001.obj"	// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ

#define ENEMY_POP_TIME	(0.001f)						// エネミー再出現時間(0.999以下)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー

static BOOL				g_Load = FALSE;


static INTERPOLATION_DATA move_tbl0[] = {// -X方向へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-420.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl1[] = {// X方向へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(420.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl2[] = {// 奥(Z方向)へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl3[] = {// 手前(-Z方向)へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl4[] = {// 左手前へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-620.0, ENEMY_OFFSET_Y, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl5[] = {// 左奥へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-620.0, ENEMY_OFFSET_Y, 420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl6[] = {// 右奥へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(620.0, ENEMY_OFFSET_Y, 420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl7[] = {// 右手前へ移動
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(620.0, ENEMY_OFFSET_Y, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl8[] = {// 左手前から時計回り
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};

static INTERPOLATION_DATA move_tbl9[] = {// 左奥から時計回り
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};

static INTERPOLATION_DATA move_tbl10[] = {// 右奥から時計回り
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};

static INTERPOLATION_DATA move_tbl11[] = {// 右手前から時計回り
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};


static float			enemyPopCount[MAX_ENEMY];		// エネミー出現のカウント

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 40.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].use = TRUE;			// TRUE:生きてる

		enemyPopCount[i] = 1.0f;		// エネミー出現時間の初期値

		g_Enemy[i].interpolationUse = FALSE;	// 全ての線形補間を未使用にする


	}



	// 0番を線形補間で動かしてみる
	g_Enemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tbl_adr = move_tbl0;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tbl_size = sizeof(move_tbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 1番を線形補間で動かしてみる
	g_Enemy[1].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[1].tbl_adr = move_tbl1;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[1].tbl_size = sizeof(move_tbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 2番を線形補間で動かしてみる
	g_Enemy[2].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[2].tbl_adr = move_tbl2;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[2].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 3番を線形補間で動かしてみる
	g_Enemy[3].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[3].tbl_adr = move_tbl3;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[3].tbl_size = sizeof(move_tbl3) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 4番を線形補間で動かしてみる
	g_Enemy[4].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[4].tbl_adr = move_tbl4;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[4].tbl_size = sizeof(move_tbl4) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 5番を線形補間で動かしてみる
	g_Enemy[5].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[5].tbl_adr = move_tbl5;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[5].tbl_size = sizeof(move_tbl5) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 6番を線形補間で動かしてみる
	g_Enemy[6].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[6].tbl_adr = move_tbl6;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[6].tbl_size = sizeof(move_tbl6) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 7番を線形補間で動かしてみる
	g_Enemy[7].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[7].tbl_adr = move_tbl7;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[7].tbl_size = sizeof(move_tbl7) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 8番を線形補間で動かしてみる
	g_Enemy[8].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[8].tbl_adr = move_tbl8;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[8].tbl_size = sizeof(move_tbl8) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 9番を線形補間で動かしてみる
	g_Enemy[9].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[9].tbl_adr = move_tbl9;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[9].tbl_size = sizeof(move_tbl9) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 10番を線形補間で動かしてみる
	g_Enemy[10].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[10].tbl_adr = move_tbl10;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[10].tbl_size = sizeof(move_tbl10) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 11番を線形補間で動かしてみる
	g_Enemy[11].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[11].tbl_adr = move_tbl11;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[11].tbl_size = sizeof(move_tbl11) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{

	// エネミーを動かす場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
		{
			enemyPopCount[i] -= ENEMY_POP_TIME;			// エネミーが死んでいるときにカウントを開始

			if (enemyPopCount[i] <= 0.0f)
			{
				g_Enemy[i].use = TRUE;
				enemyPopCount[i] = 1.0f;		// エネミー出現時間の初期化

			}


		}

		if (g_Enemy[i].use == TRUE)// このエネミーが使われている？
		{									// Yes
			if (g_Enemy[i].tbl_adr != NULL)	// 線形補間を実行する？
			{								// 線形補間の処理
				// 移動処理
				int		index = (int)g_Enemy[i].move_time;
				float	time = g_Enemy[i].move_time - index;
				int		size = g_Enemy[i].tbl_size;

				float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1フレームで進める時間
				g_Enemy[i].move_time += dt;							// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					g_Enemy[i].move_time = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// 次の場所
				XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// 現在の場所
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// 次の角度
				XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// 現在の角度
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// 次のScale
				XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// 現在のScale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

			}

			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		// 縁取りの設定
		//SetFuchi(1);

		// モデル描画
		DrawModel(&g_Enemy[i].model);

		// 縁取りの設定
		//SetFuchi(0);


	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

