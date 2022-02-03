//=============================================================================
//
// 海藻の処理 [seaweed.cpp]
// Author : 湯川 蒼
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "seaweed.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(5)				// テクスチャの数

#define	TREE_WIDTH			(50.0f)			// 頂点サイズ
#define	TREE_HEIGHT			(80.0f)			// 頂点サイズ

#define	MAX_TREE			(256)			// 木最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	BOOL		bUse;			// 使用しているかどうか

} TREE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexSeaweed(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static TREE					g_aSeaweed[MAX_TREE];	// 木ワーク
static int					g_TexNo;			// テクスチャ番号
static BOOL					g_bAlpaTest;		// アルファテストON/OFF
//static int				g_nAlpha;			// アルファテストの閾値

static BOOL					g_Load = FALSE;


static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/seaweed000.png",
	"data/TEXTURE/seaweed001.png",
	"data/TEXTURE/seaweed002.png",
	"data/TEXTURE/seaweed003.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSeaweed(void)
{
	MakeVertexSeaweed();

	// テクスチャ生成
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

	// 木ワークの初期化
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

	// 木の設定
	SetSeaweed(XMFLOAT3(80.0f, -17.0f, -50.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSeaweed(XMFLOAT3(300.0f, -17.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSeaweed(XMFLOAT3(-180.0f, -17.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSeaweed(XMFLOAT3(0.0f, -17.0f, 150.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSeaweed(void)
{
	if (g_Load == FALSE) return;

	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSeaweed(void)
{

	for(int nCntSeaweed = 0; nCntSeaweed < MAX_TREE; nCntSeaweed++)
	{
		if(g_aSeaweed[nCntSeaweed].bUse)
		{
			// 影の位置設定
			SetPositionShadow(g_aSeaweed[nCntSeaweed].nIdxShadow, XMFLOAT3(g_aSeaweed[nCntSeaweed].pos.x, 0.1f, g_aSeaweed[nCntSeaweed].pos.z));
		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

	//// アルファテストの閾値変更
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
// 描画処理
//=============================================================================
void DrawSeaweed(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE)
	{
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_TREE; i++)
	{
		if(g_aSeaweed[i].bUse)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
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

			// スケールを反映
			mtxScl = XMMatrixScaling(g_aSeaweed[i].scl.x, g_aSeaweed[i].scl.y, g_aSeaweed[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_aSeaweed[i].pos.x, g_aSeaweed[i].pos.y, g_aSeaweed[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_aSeaweed[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexSeaweed(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 木のパラメータをセット
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

			// 影の設定
			g_aSeaweed[nCntSeaweed].nIdxShadow = CreateShadow(g_aSeaweed[nCntSeaweed].pos, 0.5f, 0.5f);

			nIdxSeaweed = nCntSeaweed;

			break;
		}
	}

	return nIdxSeaweed;
}
