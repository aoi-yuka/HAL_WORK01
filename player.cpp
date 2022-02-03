//=============================================================================
//
// ���f������ [player.cpp]
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
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/wood_character.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS	"data/MODEL/parts_ring000.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(2.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define PLAYER_SHADOW_SIZE	(1.0f)						// �e�̑傫��

#define PLAYER_PARTS_MAX	(2)							// �v���C���[�̃p�[�c�̐�

#define PLAYER_GRAVITY		(-3.0f)						// �v���C���[�ɗ^����d��

#define PLAYER_JUMP_POWER	(11.0f)						// �W�����v��
#define PLAYER_JUMP_AC		(0.9f)						// �W�����v�͂̌����x(1.0f����)
#define PLAYER_STEP_AC		(PLAYER_JUMP_AC * 0.9f)		// ���݂����Ƃ��̃W�����v�͂̌����x

#define INIT_DASH_POWER		(5.0f)						// �_�b�V�����x
#define MAX_DASH_POWER		(10.0f)						// �_�b�V���̍ő呬�x
#define PLAYER_DASH_AC		(1.8f)						// �_�b�V�������x�i�Z.�Z�{�������j
#define PLAYER_DASH_DC		(0.8f)						// �_�b�V�������x�i�Z.�Zf�������j

#define PLAYER_SHADOW_HEIGHT		(100.0f)			// �_�b�V�������x�i�Z.�Zf�������j


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void PlayerGravity(void);
void PlayerGround(void);
void PlayerJump(void);
void PlayerDash(void);
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		player;							// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static BOOL			g_Load = FALSE;


// �v���C���[�̊K�w�A�j���[�V�����f�[�^
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


static int		firstJumpCount  = 0;		// ���݂������J�E���g
static BOOL		maxDashFlag = FALSE;		// �_�b�V���̍ő呬�x�ɓ��B�������̃t���O

static BOOL		setGroundSound = FALSE;	// ���n�����Ƃ��̃T�E���h�t���O

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &player.model);
	player.load = TRUE;

	player.pos = { 50.0f, PLAYER_OFFSET_Y, -600.0f };
	player.rot = { 0.0f, 0.0f, 0.0f };
	player.scl = { 0.7f, 0.7f, 0.7f };

	player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	player.use = TRUE;

	player.jumpFlag = FALSE;				// �W�����v�t���O�̏����l
	player.gravity  = PLAYER_GRAVITY;		// �d��
	player.ground   = TRUE;					// �n�ʂւ̐ڒn�t���O

	player.jumpPower = PLAYER_JUMP_POWER;	// �W�����v��

	player.checkHeadCollision = FALSE;		// ���݂�����

	player.dashSpeed = 0.0f;				// �_�b�V�����x�̏����l
	player.dashSpeedCalc = INIT_DASH_POWER;	// �_�b�V�����x�̏����l
	player.dashFlag  = FALSE;				// �_�b�V���t���O�̏����l

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�

	// �K�w�A�j���[�V�����p�̏���������
	player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	// �p�[�c�̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &player;		// �� �����ɐe�̃A�h���X������
	//	g_Parts[�r].parent= &player;		// �r��������e�͖{�́i�v���C���[�j
	//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i].move_time = 0.0f;	// ���s���Ԃ��N���A
		g_Parts[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent   = &player;		// �e���Z�b�g
	g_Parts[0].tbl_adr  = move_tbl0;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[0].tbl_size = sizeof(move_tbl0) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_PARTS, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent   = &player;		// �e���Z�b�g
	g_Parts[1].tbl_adr  = move_tbl1;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[1].tbl_size = sizeof(move_tbl1) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_PARTS, &g_Parts[1].model);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
	if (player.load)
	{
		UnloadModel(&player.model);
		player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	// �d��
	PlayerGravity();

	// �n�ʂ֐ڒn�����Ƃ��̏���
	PlayerGround();

	// �e�̏���
	float t = player.pos.y / PLAYER_SHADOW_HEIGHT;
	float newSize = PLAYER_SHADOW_SIZE * (1.0f - t);
	SetShadowScale(player.shadowIdx, XMFLOAT3(newSize, 1.0f, newSize));

	if (player.use == TRUE)
	{
		// �ړ�����
		if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
		{	// ���ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
		{	// �E�ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = -XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
		{	// ��ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = XM_PI;
		}
		if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
		{	// ���ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = 0.0f;
		}

		// �Q�[���p�b�h�ł̈ړ�����
		if (IsButtonPressed(0, BUTTON_DOWN))
		{	// ���ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = 0.0f;
		}
		else if (IsButtonPressed(0, BUTTON_UP))
		{	// ��ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = XM_PI;
		}

		if (IsButtonPressed(0, BUTTON_RIGHT))
		{	// �E�ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = -XM_PI / 2;
		}
		else if (IsButtonPressed(0, BUTTON_LEFT))
		{	// ���ֈړ�
			player.spd = VALUE_MOVE;
			player.dir = XM_PI / 2;
		}

		// �W�����v����
		// �X�y�[�X�{�^�� or A�{�^�� ������������s
		if (GetKeyboardTrigger(DIK_SPACE) || IsButtonPressed(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_jump000);	// �W�����v��

			player.jumpFlag = TRUE;		// �W�����v���̃t���O
			player.ground = FALSE;	// �ڒn���Ă��Ȃ���Ԃ̃t���O
			//SetBullet(player.pos, player.rot);
		}
		// �_�b�V������
		if (GetKeyboardTrigger(DIK_LSHIFT))// ���V�t�g
		{
			player.dashFlag = TRUE;			// �t���O��TRUE�ɂ���
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


	// �W�����v����
	PlayerJump();

	// �v���C���[�̃_�b�V������
	// �_�b�V���t���O���L���ȂƂ��Ɏ��s
	if (player.dashFlag == TRUE)
	{
		PlayerDash();
	}


	//	// Key���͂���������ړ���������
	if (player.spd > 0.0f)
	{
		player.rot.y = player.dir + cam->rot.y;

		// ���͂̂����������փv���C���[���������Ĉړ�������
		player.pos.x -= sinf(player.rot.y) * player.spd;
		player.pos.z -= cosf(player.rot.y) * player.spd;
	}
	



	// �}�b�v�O����
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



	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
	XMFLOAT3 hitPosition;								// ��_
	hitPosition.y = player.pos.y - PLAYER_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
			// RayHitField(�v���C���[�̍��W, �n�ʂƂ̌�_, �@���i�����j)
	bool ans = RayHitField(player.pos, &hitPosition, &normal);
	//player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;	// ���C�L���X�g�̒l��L���ɂ��Ĕz�u
	//player.pos.y = PLAYER_OFFSET_Y;


	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(player.shadowIdx, pos);


	player.spd *= 0.5f;


	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[i].use == TRUE)&&(g_Parts[i].tbl_adr != NULL))
		{
			// �ړ�����
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Parts[i].move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	// ���̊p�x
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	// ����Scale
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	// ���݂�Scale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);

		}
	}



	{	// �|�C���g���C�g�̃e�X�g
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
	// �p������
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// �Q�̃x�N�g���̊O�ς�����ĔC�ӂ̉�]�������߂�
	player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&player.upVector));

	// ���߂���]������N�H�[�^�j�I�������o��
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// �O��̃N�H�[�^�j�I�����獡��̃N�H�[�^�j�I���܂ł̉�]�����炩�ɂ���
	//	   XMQuaternionSlerp(���݂̃v���C���[�̃N�H�[�^�j�I�����, ���߂��N�H�[�^�j�I��, �Z�����ړ�)
	quat = XMQuaternionSlerp(XMLoadFloat4(&player.quaternion), quat, 0.05f);

	// ����̃N�H�[�^�j�I���̌��ʂ�ۑ�����
	XMStoreFloat4(&player.quaternion, quat);



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", player.pos.x, player.pos.y, player.pos.z);
	PrintDebugProc("�ڒn(Yes��1�@No��0) :%d\n", player.ground);
	PrintDebugProc("�d�͒l :%f\n", player.gravity);
	PrintDebugProc("���݂�����(Yes��1�@No��0) :%d\n", player.checkHeadCollision);
	PrintDebugProc("���n���t���O(�点���ԁ�1�@�点�Ȃ���ԁ�0) :%d\n", setGroundSound);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// ���̃v���C���[���g���Ă��鎞�ɕ`��
	if (player.use == TRUE)
	{
		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y + XM_PI, player.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//// �N�H�[�^�j�I���𔽉f
		XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	
		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&player.mtxWorld, mtxWorld);

		// �����̐ݒ�
		//SetFuchi(1);

		// ���f���`��
		DrawModel(&player.model);



		// �p�[�c�̊K�w�A�j���[�V����
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
				// ��
				// player.mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Parts[i].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_Parts[i].model);

		}

		// �����̐ݒ�
		//SetFuchi(0);

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);

	}
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &player;
}


//=============================================================================
// �v���C���[�̏d�͂�L���ɂ���
//=============================================================================
void PlayerGravity(void)
{
	if (player.ground == FALSE)// �ڒn���Ă��Ȃ��Ƃ��Ɏ��s
	{
		player.pos.y += player.gravity;	// �d�͂��v���C���[�ɔ��f
		setGroundSound = TRUE;
	}

}


//=============================================================================
// �v���C���[���n�ʂ֐ڒn�����Ƃ��̏���
//=============================================================================
void PlayerGround(void)
{
	// �v���C���[�̍��� <= �n�ʂ̍��� �̂Ƃ�
	if (player.pos.y <= PLAYER_OFFSET_Y)
	{
		// ���n�����Ƃ��ɃT�E���h���Đ�
		if (setGroundSound == TRUE)
		{
			PlaySound(SOUND_LABEL_SE_ground000);// ���n��
			setGroundSound = FALSE;				// ���n����炳�Ȃ�

		}

		player.ground = TRUE;				// �ڒn���Ă���t���O
		player.jumpFlag = FALSE;			// �W�����v�ł����Ԃɖ߂�
		player.checkHeadCollision = FALSE;// ���݂������������
		player.pos.y = PLAYER_OFFSET_Y;	// �덷�h�~�̂��ߏ����l����

	}


}


//=============================================================================
// �v���C���[�̃W�����v����
//=============================================================================
void PlayerJump(void)
{
	// �W�����v���L�����Ɏ��s
	if (player.jumpFlag == TRUE && player.checkHeadCollision == FALSE)
	{
		player.jumpPower *= PLAYER_JUMP_AC;	// ���t���[���W�����v�͂���߂Ă���
		player.pos.y += player.jumpPower;	// ���t���[���W�����v�͂�ǉ�

	}
	// �W�����v���Ă��Ȃ��Ƃ��Ɏ��s
	else if (player.jumpFlag == FALSE)
	{
		player.jumpPower = PLAYER_JUMP_POWER;// ���n�����Ƃ��ɃW�����v�͂�߂�
		firstJumpCount = 0;					  // ���݂������J�E���g�����ɖ߂�

	}

	// �G�l�~�[�𓥂݂����Ƃ��̏���
	if (player.checkHeadCollision == TRUE)
	{
		firstJumpCount += 1;
		if (firstJumpCount == 1)
		{
			player.jumpPower = PLAYER_JUMP_POWER;// �W�����v�͂�߂�
		}

		// ���݂�����y����ɔ��
		player.jumpPower *= PLAYER_STEP_AC;	// ���t���[���W�����v�͂���߂Ă���
		player.pos.y += player.jumpPower;		// ���t���[���W�����v�͂�ǉ�
	}

}


//=============================================================================
// �v���C���[�̃_�b�V������
//=============================================================================
void PlayerDash(void)
{
	float calc = 5.0f;
	// �ő呬�x�ɒB���Ă��Ȃ��Ƃ�
	if (maxDashFlag == FALSE)
	{
		player.dashSpeedCalc *= PLAYER_DASH_AC;		// ���t���[����������l���v�Z
	}

	// �ݒ肵���ő呬�x�ɓ��B�����Ƃ�
	if (player.dashSpeedCalc >= MAX_DASH_POWER && maxDashFlag == FALSE)
	{
		maxDashFlag = TRUE;						// �ő呬�x���B�t���O
		player.dashSpeedCalc = MAX_DASH_POWER;	// �ő�l���x���Z�b�g
	}
	// �ő呬�x�ɓ��B��̏���
	else if (maxDashFlag == TRUE)
	{
		if (player.dashSpeedCalc > 0.0f)
		{
			player.dashSpeedCalc -= PLAYER_DASH_DC;	// ���t���[����������l���v�Z
		}
		else
		{
			player.dashSpeedCalc = 0.0f;
			player.dashFlag = FALSE;
			maxDashFlag = FALSE;
		}
	}

	// �v�Z�����l����
	player.dashSpeed = player.dashSpeedCalc;

	// �v�Z�����ړ��� + �_�b�V���X�s�[�h�𑫂�
	player.spd = player.spd + player.dashSpeed;


	if (player.dashFlag == FALSE)
	{
		player.dashSpeedCalc = INIT_DASH_POWER;	// �_�b�V���v�Z�̏����l��߂�

	}

}