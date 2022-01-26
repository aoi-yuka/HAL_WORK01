//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "seaweed.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "sound.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DETECTION_HEIGHT	(1.0f)				// 踏みつけ判定エリアの高さ(float)

#define OFFSET_HEAD			(3.0f)				// 頭上の余白

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF

BOOL jumpCheck = FALSE;			// 踏みつけの当たり判定に使う

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// 壁(裏側用の半透明)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// 海藻を生やす
	InitSeaweed();

	// 弾の初期化
	InitBullet();

	// スコアの初期化
	InitScore();

	// パーティクルの初期化
	//InitParticle();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_game000);


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	//UninitParticle();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// 海藻の終了処理
	UninitSeaweed();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();


}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// 地面処理の更新
	UpdateMeshField();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// 壁処理の更新
	UpdateMeshWall();

	// 海藻の更新処理
	UpdateSeaweed();

	// 弾の更新処理
	UpdateBullet();

	// パーティクルの更新処理

	//UpdateParticle();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	// スコアの更新処理
	UpdateScore();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	//DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// 弾の描画処理
	DrawBullet();

	// 壁の描画処理
	DrawMeshWall();

	// 海藻の描画処理
	DrawSeaweed();

	// パーティクルの描画処理
	DrawParticle();


	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// スコアの描画処理
	DrawScore();


	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	BULLET *bullet = GetBullet();	// 弾のポインターを初期化

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// プレイヤーは倒される
			player->use = FALSE;
			ReleaseShadow(player->shadowIdx);

			PlaySound(SOUND_LABEL_SE_playerDestroy000);	// プレイヤーダメージ音

		}




		//BBで踏みつけの為の計算
		/*プレイヤー*/
		XMFLOAT3 pSyokiAshimoto = XMFLOAT3(0.0f, PLAYER_OFFSET_Y, 0.0f);			// オフセット分を代入
		XMVECTOR pAshimotoCalc = XMLoadFloat3(&player->pos) - XMLoadFloat3(&pSyokiAshimoto);// 現在のポジションから引く
		XMFLOAT3 pAshimoto = { 0.0f, 0.0f, 0.0f };		// 結果を代入する変数を用意
		XMStoreFloat3(&pAshimoto, pAshimotoCalc);		// ashimoto へ代入
		/*エネミー*/
		XMFLOAT3 eSyokiAshimoto = XMFLOAT3(0.0f, ENEMY_OFFSET_Y + enemy[i].size/2, 0.0f);			// 頭上の高さを代入
		XMVECTOR eAshimotoCalc = XMLoadFloat3(&enemy[i].pos) + XMLoadFloat3(&eSyokiAshimoto);// 現在のポジションから足す
		XMFLOAT3 eAshimoto = { 0.0f, 0.0f, 0.0f };		// 結果を代入する変数を用意
		XMStoreFloat3(&eAshimoto, eAshimotoCalc);		// ashimoto へ代入


		//BBで踏みつけの当たり判定
		// プレイヤーの足元が一度エネミーの頭上に来ていたかをチェック
		if (pAshimoto.y > eAshimoto.y + OFFSET_HEAD)
		{
			jumpCheck = TRUE;// 来ていた
		}
		// 来ていたときの処理
		if (jumpCheck == TRUE)
		{
			//BBで踏みつけの当たり判定
			if (CollisionBB(pAshimoto, player->size, DETECTION_HEIGHT, player->size,
				eAshimoto, enemy[i].size*4.0f, DETECTION_HEIGHT, enemy[i].size*4.0f))// 少し大きめに4倍にしている
			{

				// エネミーは倒される
				enemy[i].use = FALSE;

				ReleaseShadow(enemy[i].shadowIdx);

				PlaySound(SOUND_LABEL_SE_jumpkill000);	// 踏みつけ音

				// スコアを足す
				AddScore(100);

				// 踏みつけた判定を保存
				BOOL checkCollision = TRUE;
				player->checkHeadCollision = checkCollision;
			}
			// 地面に接地したときの処理
			else if (player->ground == TRUE)
			{
				jumpCheck = FALSE;// 頭上チェックを初期化
			}
		}




		int ans = CollisionBB(pAshimoto, player->size, DETECTION_HEIGHT, player->size,
								eAshimoto, enemy[i].size, DETECTION_HEIGHT, enemy[i].size);
#ifdef _DEBUG	// デバッグ情報を表示する
		PrintDebugProc("Playerの足元のY座標 :%f\n", pAshimoto.y);
		PrintDebugProc("CollisionBB(TRUE→1　FALSE→0) :%d\n", ans);
		PrintDebugProc("P足元>=E足元　確認(TRUE→1　FALSE→0) :%d\n", jumpCheck);

#endif


	}


	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].use == FALSE)
			continue;

		// 敵と当たってるか調べる
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//敵の有効フラグをチェックする
			if (enemy[j].use == FALSE)
				continue;

			//BCの当たり判定
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{
				// 当たったから未使用に戻す
				bullet[i].use = FALSE;
				ReleaseShadow(bullet[i].shadowIdx);

				// 敵キャラクターは倒される
				enemy[j].use = FALSE;
				ReleaseShadow(enemy[j].shadowIdx);

				// スコアを足す
				AddScore(10);
			}
		}

	}


	// エネミーが全部死亡したら状態遷移
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	// エネミーが０匹？
	if (enemy_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

	// プレイヤーが全部死亡したら状態遷移
	int player_count = 0;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (player[i].use == FALSE) continue;
		player_count++;
	}

	// プレイヤーが０人？
	if (player_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

}


