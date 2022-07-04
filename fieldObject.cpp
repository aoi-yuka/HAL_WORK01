#include "fieldObject.h"


#define READ_BUFFER_MAX		(512)
#define TOKEN_NUM_MAX		(16)
#define TOKEN_LEN_MAX		(64)
#define MAX_OBJ				(64)

static FieldObject *fieldObject[MAX_OBJ];

void InPortField(char *path)
{
	// 今あるフィールドオブジェクトをリセットする
	for (int i = 0; i < MAX_OBJ; i++)
	{
		if (fieldObject[i] == NULL) continue;

		delete fieldObject[i];
		fieldObject[i] = NULL;
	}

	FILE * fp = fopen(path, "rt");
	if (fp == NULL)
	{
		MessageBox(NULL, "インポート失敗：ファイルを開けませんでした。", NULL, MB_OK);
		return;
	}


	char buffer[READ_BUFFER_MAX];


	// データを取得
	int cnt = 0;
	while (true)
	{
		fgets(buffer, READ_BUFFER_MAX, fp);
		if (feof(fp)) break;

		char token[TOKEN_NUM_MAX][TOKEN_LEN_MAX];
		char delimiter[] = "\t\n";

		strcpy(token[0], strtok(buffer, delimiter));

		// ２単語目以降を取得
		int i = 1;
		char *p;
		while (p = strtok(NULL, delimiter))
		{
			strcpy(token[i], p);
			i++;
		}

		if (strcmp(token[0], "o") == 0)
		{
			fieldObject[cnt] = new FieldObject;


			char modelPath[TOKEN_LEN_MAX] = "data/MODEL/";

			strcat(modelPath, token[1]);

			DX11_MODEL model;
			LoadModel(modelPath, &model);

			fieldObject[cnt]->SetModel(model);

			fieldObject[cnt]->SetInUse(TRUE);

			XMFLOAT3 pos = { (float)atof(token[2]), (float)atof(token[3]), (float)atof(token[4]) };
			XMFLOAT3 rot = { (float)atof(token[5]), (float)atof(token[6]), (float)atof(token[7]) };
			XMFLOAT3 scl = { (float)atof(token[8]), (float)atof(token[9]), (float)atof(token[10]) };

			fieldObject[cnt]->SetSRT(pos, rot, scl);
			fieldObject[cnt]->SetOldPos(pos);
			fieldObject[cnt]->SetOldRot(rot);
			fieldObject[cnt]->SetOldScl(scl);

			cnt++;

		}
		if (strcmp(token[0], "bs") == 0)
		{
			fieldObject[cnt - 1]->SetInCollidable(true);

			fieldObject[cnt - 1]->NewShape(SHAPE_TYPE_BS);

			XMFLOAT3 pos = { (float)atof(token[1]), (float)atof(token[2]), (float)atof(token[3]) };
			float r = (float)atof(token[4]);

			BS *bs = (BS *)fieldObject[cnt - 1]->GetShape();

			bs->SetPos(pos);
			bs->SetR(r);

			fieldObject[cnt - 1]->SetShape(bs);

		}
		if (strcmp(token[0], "aabb") == 0)
		{
			fieldObject[cnt - 1]->SetInCollidable(true);

			fieldObject[cnt - 1]->NewShape(SHAPE_TYPE_AABB);

			XMFLOAT3 min = { (float)atof(token[1]), (float)atof(token[2]), (float)atof(token[3]) };
			XMFLOAT3 max = { (float)atof(token[4]), (float)atof(token[5]), (float)atof(token[6]) };

			AABB *aabb = (AABB *)fieldObject[cnt - 1]->GetShape();

			aabb->SetMin(min);
			aabb->SetMax(max);

			fieldObject[cnt - 1]->SetShape(aabb);

		}
		if (strcmp(token[0], "obb") == 0)
		{
			fieldObject[cnt - 1]->SetInCollidable(true);

			fieldObject[cnt - 1]->NewShape(SHAPE_TYPE_OBB);

			XMFLOAT3 pos = { (float)atof(token[1]), (float)atof(token[2]), (float)atof(token[3]) };
			XMFLOAT3 rot = { (float)atof(token[4]), (float)atof(token[5]), (float)atof(token[6]) };
			float len[3] = { (float)atof(token[7]), (float)atof(token[8]), (float)atof(token[9]) };

			OBB *obb = (OBB *)fieldObject[cnt - 1]->GetShape();

			obb->SetPos(pos);
			obb->SetLen(len[0], len[1], len[2]);
			obb->SetDir(rot);

			fieldObject[cnt - 1]->SetShape(obb);

		}

	}


	fclose(fp);


}

FieldObject::FieldObject()
{
	_inUse = FALSE;
	_inCollidable = false;
	_shape = NULL;
	_oldPos = { 0.0f, 0.0f, 0.0f };

}

FieldObject::~FieldObject()
{
}

void FieldObject::SetSRT(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scl)
{
	_pos = pos;
	_rot = rot;
	_scl = scl;
}

void FieldObject::NewShape(int i)
{
	switch (i)
	{
	case SHAPE_TYPE_BS:

		_shape = new BS;

		break;

	case SHAPE_TYPE_AABB:

		_shape = new AABB;

		break;

	case SHAPE_TYPE_OBB:

		_shape = new OBB;

		break;

	case SHAPE_TYPE_MAX:
		_shape = new Shape;
	}


}
