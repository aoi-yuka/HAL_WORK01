//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : ���� ��
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
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/monster001.obj"	// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��

#define ENEMY_POP_TIME	(0.001f)						// �G�l�~�[�ďo������(0.999�ȉ�)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[

static BOOL				g_Load = FALSE;


static INTERPOLATION_DATA move_tbl0[] = {// -X�����ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-420.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl1[] = {// X�����ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(420.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl2[] = {// ��(Z����)�ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl3[] = {// ��O(-Z����)�ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl4[] = {// ����O�ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-620.0, ENEMY_OFFSET_Y, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl5[] = {// �����ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-620.0, ENEMY_OFFSET_Y, 420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl6[] = {// �E���ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(620.0, ENEMY_OFFSET_Y, 420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl7[] = {// �E��O�ֈړ�
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(620.0, ENEMY_OFFSET_Y, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 5 },

};

static INTERPOLATION_DATA move_tbl8[] = {// ����O���玞�v���
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};

static INTERPOLATION_DATA move_tbl9[] = {// �������玞�v���
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};

static INTERPOLATION_DATA move_tbl10[] = {// �E�����玞�v���
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};

static INTERPOLATION_DATA move_tbl11[] = {// �E��O���玞�v���
	// pos,											rot,					scl,						frame
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(-320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, 320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },
	{ XMFLOAT3(320.0f, ENEMY_OFFSET_Y, -320.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 8 },

};


static float			enemyPopCount[MAX_ENEMY];		// �G�l�~�[�o���̃J�E���g

//=============================================================================
// ����������
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

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Enemy[i].use = TRUE;			// TRUE:�����Ă�

		enemyPopCount[i] = 1.0f;		// �G�l�~�[�o�����Ԃ̏����l

		g_Enemy[i].interpolationUse = FALSE;	// �S�Ă̐��`��Ԃ𖢎g�p�ɂ���


	}



	// 0�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[0].tbl_adr = move_tbl0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[0].tbl_size = sizeof(move_tbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 1�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[1].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[1].tbl_adr = move_tbl1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[1].tbl_size = sizeof(move_tbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 2�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[2].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[2].tbl_adr = move_tbl2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[2].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 3�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[3].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[3].tbl_adr = move_tbl3;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[3].tbl_size = sizeof(move_tbl3) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 4�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[4].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[4].tbl_adr = move_tbl4;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[4].tbl_size = sizeof(move_tbl4) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 5�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[5].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[5].tbl_adr = move_tbl5;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[5].tbl_size = sizeof(move_tbl5) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 6�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[6].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[6].tbl_adr = move_tbl6;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[6].tbl_size = sizeof(move_tbl6) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 7�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[7].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[7].tbl_adr = move_tbl7;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[7].tbl_size = sizeof(move_tbl7) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 8�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[8].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[8].tbl_adr = move_tbl8;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[8].tbl_size = sizeof(move_tbl8) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 9�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[9].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[9].tbl_adr = move_tbl9;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[9].tbl_size = sizeof(move_tbl9) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 10�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[10].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[10].tbl_adr = move_tbl10;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[10].tbl_size = sizeof(move_tbl10) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 11�Ԃ���`��Ԃœ������Ă݂�
	g_Enemy[11].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[11].tbl_adr = move_tbl11;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[11].tbl_size = sizeof(move_tbl11) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{

	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
		{
			enemyPopCount[i] -= ENEMY_POP_TIME;			// �G�l�~�[������ł���Ƃ��ɃJ�E���g���J�n

			if (enemyPopCount[i] <= 0.0f)
			{
				g_Enemy[i].use = TRUE;
				enemyPopCount[i] = 1.0f;		// �G�l�~�[�o�����Ԃ̏�����

			}


		}

		if (g_Enemy[i].use == TRUE)// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_Enemy[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Enemy[i].move_time;
				float	time = g_Enemy[i].move_time - index;
				int		size = g_Enemy[i].tbl_size;

				float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Enemy[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Enemy[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

			}

			// �e���v���C���[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		// �����̐ݒ�
		//SetFuchi(1);

		// ���f���`��
		DrawModel(&g_Enemy[i].model);

		// �����̐ݒ�
		//SetFuchi(0);


	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

