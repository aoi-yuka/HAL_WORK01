//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/wood_character.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_PARTS	"data/MODEL/parts_ring000.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(2.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)						// 影の大きさ

#define PLAYER_PARTS_MAX	(2)							// プレイヤーのパーツの数

#define PLAYER_GRAVITY		(-3.0f)						// プレイヤーに与える重力

#define PLAYER_JUMP_POWER	(11.0f)						// ジャンプ力
#define PLAYER_JUMP_AC		(0.9f)						// ジャンプ力の減衰度(1.0f未満)
#define PLAYER_STEP_AC		(PLAYER_JUMP_AC * 0.9f)		// 踏みつけたときのジャンプ力の減衰度

#define INIT_DASH_POWER		(5.0f)						// ダッシュ速度
#define MAX_DASH_POWER		(10.0f)						// ダッシュの最大速度
#define PLAYER_DASH_AC		(1.8f)						// ダッシュ加速度（〇.〇倍ずつ加速）
#define PLAYER_DASH_DC		(0.8f)						// ダッシュ減速度（〇.〇fずつ減速）

#define PLAYER_SHADOW_HEIGHT		(100.0f)			// ダッシュ減速度（〇.〇fずつ減速）


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void PlayerGravity(void);
void PlayerGround(void);
void PlayerJump(void);
void PlayerDash(void);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		player;							// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static BOOL			g_Load = FALSE;


// プレイヤーの階層アニメーションデータ
static INTERPOLATION_DATA move_tbl0[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(2.0f, 2.0f, 2.0f), 45 },
	{ XMFLOAT3(0.0f, 40.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 2, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 45 },
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(2.0f, 2.0f, 2.0f), 45 },

};

static INTERPOLATION_DATA move_tbl1[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 40.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 2, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(2.5f, 2.5f, 2.5f), 60 },
	{ XMFLOAT3(0.0f, 40.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 2, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};


static int		firstJumpCount  = 0;		// 踏みつけ処理カウント
static BOOL		maxDashFlag = FALSE;		// ダッシュの最大速度に到達したかのフラグ

static BOOL		setGroundSound = FALSE;	// 着地したときのサウンドフラグ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &player.model);
	player.load = TRUE;

	player.pos = { 50.0f, PLAYER_OFFSET_Y, -600.0f };
	player.rot = { 0.0f, 0.0f, 0.0f };
	player.scl = { 0.7f, 0.7f, 0.7f };

	player.spd = 0.0f;			// 移動スピードクリア
	player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	player.use = TRUE;

	player.jumpFlag = FALSE;				// ジャンプフラグの初期値
	player.gravity  = PLAYER_GRAVITY;		// 重力
	player.ground   = TRUE;					// 地面への接地フラグ

	player.jumpPower = PLAYER_JUMP_POWER;	// ジャンプ力

	player.checkHeadCollision = FALSE;		// 踏みつけ判定

	player.dashSpeed = 0.0f;				// ダッシュ速度の初期値
	player.dashSpeedCalc = INIT_DASH_POWER;	// ダッシュ速度の初期値
	player.dashFlag  = FALSE;				// ダッシュフラグの初期値

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	// 階層アニメーション用の初期化処理
	player.parent = NULL;			// 本体（親）なのでNULLを入れる

	// パーツの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &player;		// ← ここに親のアドレスを入れる
	//	g_Parts[腕].parent= &player;		// 腕だったら親は本体（プレイヤー）
	//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i].move_time = 0.0f;	// 実行時間をクリア
		g_Parts[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent   = &player;		// 親をセット
	g_Parts[0].tbl_adr  = move_tbl0;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[0].tbl_size = sizeof(move_tbl0) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_PARTS, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent   = &player;		// 親をセット
	g_Parts[1].tbl_adr  = move_tbl1;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[1].tbl_size = sizeof(move_tbl1) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_PARTS, &g_Parts[1].model);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// モデルの解放処理
	if (player.load)
	{
		UnloadModel(&player.model);
		player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	// 重力
	PlayerGravity();

	// 地面へ接地したときの処理
	PlayerGround();

	// 影の処理
	float t = player.pos.y / PLAYER_SHADOW_HEIGHT;
	float newSize = PLAYER_SHADOW_SIZE * (1.0f - t);
	SetShadowScale(player.shadowIdx, XMFLOAT3(newSize, 1.0f, newSize));

	if (player.use == TRUE)
	{
		// 移動処理
		if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
		{	// 左へ移動
			player.spd = VALUE_MOVE;
			player.dir = XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
		{	// 右へ移動
			player.spd = VALUE_MOVE;
			player.dir = -XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
		{	// 上へ移動
			player.spd = VALUE_MOVE;
			player.dir = XM_PI;
		}
		if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
		{	// 下へ移動
			player.spd = VALUE_MOVE;
			player.dir = 0.0f;
		}

		// ゲームパッドでの移動処理
		if (IsButtonPressed(0, BUTTON_DOWN))
		{	// 下へ移動
			player.spd = VALUE_MOVE;
			player.dir = 0.0f;
		}
		else if (IsButtonPressed(0, BUTTON_UP))
		{	// 上へ移動
			player.spd = VALUE_MOVE;
			player.dir = XM_PI;
		}

		if (IsButtonPressed(0, BUTTON_RIGHT))
		{	// 右へ移動
			player.spd = VALUE_MOVE;
			player.dir = -XM_PI / 2;
		}
		else if (IsButtonPressed(0, BUTTON_LEFT))
		{	// 左へ移動
			player.spd = VALUE_MOVE;
			player.dir = XM_PI / 2;
		}

		// ジャンプ処理
		// スペースボタン or Aボタン を押したら実行
		if (GetKeyboardTrigger(DIK_SPACE) || IsButtonPressed(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_jump000);	// ジャンプ音

			player.jumpFlag = TRUE;		// ジャンプ中のフラグ
			player.ground = FALSE;	// 接地していない状態のフラグ
			//SetBullet(player.pos, player.rot);
		}
		// ダッシュ処理
		if (GetKeyboardTrigger(DIK_LSHIFT))// 左シフト
		{
			player.dashFlag = TRUE;			// フラグをTRUEにする
		}
	}


#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		player.pos.z = player.pos.x = 0.0f;
		player.rot.y = player.dir = 0.0f;
		player.spd = 0.0f;
	}
#endif


	// ジャンプ処理
	PlayerJump();

	// プレイヤーのダッシュ処理
	// ダッシュフラグが有効なときに実行
	if (player.dashFlag == TRUE)
	{
		PlayerDash();
	}


	//	// Key入力があったら移動処理する
	if (player.spd > 0.0f)
	{
		player.rot.y = player.dir + cam->rot.y;

		// 入力のあった方向へプレイヤーを向かせて移動させる
		player.pos.x -= sinf(player.rot.y) * player.spd;
		player.pos.z -= cosf(player.rot.y) * player.spd;
	}
	



	// マップ外判定
	if ((player.pos.x - PLAYER_SIZE) < MAP_LEFT)
	{
		player.pos.x = MAP_LEFT + PLAYER_SIZE;
	}
	if ((player.pos.x + PLAYER_SIZE) > MAP_RIGHT)
	{
		player.pos.x = MAP_RIGHT - PLAYER_SIZE;
	}
	if ((player.pos.z + PLAYER_SIZE) > MAP_TOP)
	{
		player.pos.z = MAP_TOP - PLAYER_SIZE;
	}
	if ((player.pos.z - PLAYER_SIZE) < MAP_DOWN)
	{
		player.pos.z = MAP_DOWN + PLAYER_SIZE;
	}



	// レイキャストして足元の高さを求める
	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
	XMFLOAT3 hitPosition;								// 交点
	hitPosition.y = player.pos.y - PLAYER_OFFSET_Y;	// 外れた時用に初期化しておく
			// RayHitField(プレイヤーの座標, 地面との交点, 法線（向き）)
	bool ans = RayHitField(player.pos, &hitPosition, &normal);
	//player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;	// レイキャストの値を有効にして配置
	//player.pos.y = PLAYER_OFFSET_Y;


	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(player.shadowIdx, pos);


	player.spd *= 0.5f;


	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if ((g_Parts[i].use == TRUE)&&(g_Parts[i].tbl_adr != NULL))
		{
			// 移動処理
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1フレームで進める時間
			g_Parts[i].move_time += dt;					// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	// 次の場所
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	// 現在の場所
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	// 次の角度
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	// 現在の角度
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	// 次のScale
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	// 現在のScale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);

		}
	}



	{	// ポイントライトのテスト
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


	//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// ２つのベクトルの外積を取って任意の回転軸を求める
	player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&player.upVector));

	// 求めた回転軸からクォータニオンを作り出す
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// 前回のクォータニオンから今回のクォータニオンまでの回転を滑らかにする
	//	   XMQuaternionSlerp(現在のプレイヤーのクォータニオン情報, 求めたクォータニオン, 〇％ずつ移動)
	quat = XMQuaternionSlerp(XMLoadFloat4(&player.quaternion), quat, 0.05f);

	// 今回のクォータニオンの結果を保存する
	XMStoreFloat4(&player.quaternion, quat);



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", player.pos.x, player.pos.y, player.pos.z);
	PrintDebugProc("接地(Yes→1　No→0) :%d\n", player.ground);
	PrintDebugProc("重力値 :%f\n", player.gravity);
	PrintDebugProc("踏みつけ判定(Yes→1　No→0) :%d\n", player.checkHeadCollision);
	PrintDebugProc("着地音フラグ(鳴らせる状態→1　鳴らせない状態→0) :%d\n", setGroundSound);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// このプレイヤーが使われている時に描画
	if (player.use == TRUE)
	{
		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y + XM_PI, player.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//// クォータニオンを反映
		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	
		// 移動を反映
		mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&player.mtxWorld, mtxWorld);

		// 縁取りの設定
		//SetFuchi(1);

		// モデル描画
		DrawModel(&player.model);



		// パーツの階層アニメーション
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
				// ↑
				// player.mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Parts[i].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_Parts[i].model);

		}

		// 縁取りの設定
		//SetFuchi(0);

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);

	}
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &player;
}


//=============================================================================
// プレイヤーの重力を有効にする
//=============================================================================
void PlayerGravity(void)
{
	if (player.ground == FALSE)// 接地していないときに実行
	{
		player.pos.y += player.gravity;	// 重力をプレイヤーに反映
		setGroundSound = TRUE;
	}

}


//=============================================================================
// プレイヤーが地面へ接地したときの処理
//=============================================================================
void PlayerGround(void)
{
	// プレイヤーの高さ <= 地面の高さ のとき
	if (player.pos.y <= PLAYER_OFFSET_Y)
	{
		// 着地したときにサウンドを再生
		if (setGroundSound == TRUE)
		{
			PlaySound(SOUND_LABEL_SE_ground000);// 着地音
			setGroundSound = FALSE;				// 着地音を鳴らさない

		}

		player.ground = TRUE;				// 接地しているフラグ
		player.jumpFlag = FALSE;			// ジャンプできる状態に戻す
		player.checkHeadCollision = FALSE;// 踏みつけ判定を初期化
		player.pos.y = PLAYER_OFFSET_Y;	// 誤差防止のため初期値を代入

	}


}


//=============================================================================
// プレイヤーのジャンプ処理
//=============================================================================
void PlayerJump(void)
{
	// ジャンプが有効中に実行
	if (player.jumpFlag == TRUE && player.checkHeadCollision == FALSE)
	{
		player.jumpPower *= PLAYER_JUMP_AC;	// 毎フレームジャンプ力を弱めていく
		player.pos.y += player.jumpPower;	// 毎フレームジャンプ力を追加

	}
	// ジャンプしていないときに実行
	else if (player.jumpFlag == FALSE)
	{
		player.jumpPower = PLAYER_JUMP_POWER;// 着地したときにジャンプ力を戻す
		firstJumpCount = 0;					  // 踏みつけ処理カウントを元に戻す

	}

	// エネミーを踏みつけたときの処理
	if (player.checkHeadCollision == TRUE)
	{
		firstJumpCount += 1;
		if (firstJumpCount == 1)
		{
			player.jumpPower = PLAYER_JUMP_POWER;// ジャンプ力を戻す
		}

		// 踏みつけたら軽く上に飛ぶ
		player.jumpPower *= PLAYER_STEP_AC;	// 毎フレームジャンプ力を弱めていく
		player.pos.y += player.jumpPower;		// 毎フレームジャンプ力を追加
	}

}


//=============================================================================
// プレイヤーのダッシュ処理
//=============================================================================
void PlayerDash(void)
{
	float calc = 5.0f;
	// 最大速度に達していないとき
	if (maxDashFlag == FALSE)
	{
		player.dashSpeedCalc *= PLAYER_DASH_AC;		// 毎フレーム加速する値を計算
	}

	// 設定した最大速度に到達したとき
	if (player.dashSpeedCalc >= MAX_DASH_POWER && maxDashFlag == FALSE)
	{
		maxDashFlag = TRUE;						// 最大速度到達フラグ
		player.dashSpeedCalc = MAX_DASH_POWER;	// 最大値速度をセット
	}
	// 最大速度に到達後の処理
	else if (maxDashFlag == TRUE)
	{
		if (player.dashSpeedCalc > 0.0f)
		{
			player.dashSpeedCalc -= PLAYER_DASH_DC;	// 毎フレーム減速する値を計算
		}
		else
		{
			player.dashSpeedCalc = 0.0f;
			player.dashFlag = FALSE;
			maxDashFlag = FALSE;
		}
	}

	// 計算した値を代入
	player.dashSpeed = player.dashSpeedCalc;

	// 計算した移動量 + ダッシュスピードを足す
	player.spd = player.spd + player.dashSpeed;


	if (player.dashFlag == FALSE)
	{
		player.dashSpeedCalc = INIT_DASH_POWER;	// ダッシュ計算の初期値を戻す

	}

}