//=============================================================================
//
// �C���̏��� [seaweed.cpp]
// Author : ���� ��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "seaweed.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(5)				// �e�N�X�`���̐�

#define	TREE_WIDTH			(50.0f)			// ���_�T�C�Y
#define	TREE_HEIGHT			(80.0f)			// ���_�T�C�Y

#define	MAX_TREE			(256)			// �؍ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	BOOL		bUse;			// �g�p���Ă��邩�ǂ���

} TREE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexSeaweed(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static TREE					g_aSeaweed[MAX_TREE];	// �؃��[�N
static int					g_TexNo;			// �e�N�X�`���ԍ�
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF
//static int				g_nAlpha;			// �A���t�@�e�X�g��臒l

static BOOL					g_Load = FALSE;


static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/seaweed000.png",
	"data/TEXTURE/seaweed001.png",
	"data/TEXTURE/seaweed002.png",
	"data/TEXTURE/seaweed003.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSeaweed(void)
{
	MakeVertexSeaweed();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �؃��[�N�̏�����
	for(int nCntSeaweed = 0; nCntSeaweed < MAX_TREE; nCntSeaweed++)
	{
		ZeroMemory(&g_aSeaweed[nCntSeaweed].material, sizeof(g_aSeaweed[nCntSeaweed].material));
		g_aSeaweed[nCntSeaweed].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aSeaweed[nCntSeaweed].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aSeaweed[nCntSeaweed].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aSeaweed[nCntSeaweed].fWidth = TREE_WIDTH;
		g_aSeaweed[nCntSeaweed].fHeight = TREE_HEIGHT;
		g_aSeaweed[nCntSeaweed].bUse = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	// �؂̐ݒ�
	SetSeaweed(XMFLOAT3(80.0f, -17.0f, -50.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSeaweed(XMFLOAT3(300.0f, -17.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSeaweed(XMFLOAT3(-180.0f, -17.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSeaweed(XMFLOAT3(0.0f, -17.0f, 150.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSeaweed(void)
{
	if (g_Load == FALSE) return;

	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSeaweed(void)
{

	for(int nCntSeaweed = 0; nCntSeaweed < MAX_TREE; nCntSeaweed++)
	{
		if(g_aSeaweed[nCntSeaweed].bUse)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aSeaweed[nCntSeaweed].nIdxShadow, XMFLOAT3(g_aSeaweed[nCntSeaweed].pos.x, 0.1f, g_aSeaweed[nCntSeaweed].pos.z));
		}
	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

	//// �A���t�@�e�X�g��臒l�ύX
	//if(GetKeyboardPress(DIK_I))
	//{
	//	g_nAlpha--;
	//	if(g_nAlpha < 0)
	//	{
	//		g_nAlpha = 0;
	//	}
	//}
	//if(GetKeyboardPress(DIK_K))
	//{
	//	g_nAlpha++;
	//	if(g_nAlpha > 255)
	//	{
	//		g_nAlpha = 255;
	//	}
	//}
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSeaweed(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_TREE; i++)
	{
		if(g_aSeaweed[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_aSeaweed[i].scl.x, g_aSeaweed[i].scl.y, g_aSeaweed[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aSeaweed[i].pos.x, g_aSeaweed[i].pos.y, g_aSeaweed[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_aSeaweed[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexSeaweed(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �؂̃p�����[�^���Z�b�g
//=============================================================================
int SetSeaweed(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxSeaweed = -1;

	for(int nCntSeaweed = 0; nCntSeaweed < MAX_TREE; nCntSeaweed++)
	{
		if(!g_aSeaweed[nCntSeaweed].bUse)
		{
			g_aSeaweed[nCntSeaweed].pos = pos;
			g_aSeaweed[nCntSeaweed].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_aSeaweed[nCntSeaweed].fWidth = fWidth;
			g_aSeaweed[nCntSeaweed].fHeight = fHeight;
			g_aSeaweed[nCntSeaweed].bUse = TRUE;

			// �e�̐ݒ�
			g_aSeaweed[nCntSeaweed].nIdxShadow = CreateShadow(g_aSeaweed[nCntSeaweed].pos, 0.5f, 0.5f);

			nIdxSeaweed = nCntSeaweed;

			break;
		}
	}

	return nIdxSeaweed;
}
