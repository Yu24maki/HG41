

#include "main.h"
#include "input.h"
#include "renderer.h"
#include "game_object.h"
#include "model.h"
#include "texture.h"
#include "tree.h"


#define ROLL_LIMIT (40)
#define GET_ROT (XMMatrixRotationRollPitchYaw(0.0f, XMConvertToRadians(rand() % 360), XMConvertToRadians(rand() % ROLL_LIMIT)))
#define SCALING (0.9f)
#define CLAMP(a, min, max) ((a > max) ? max : (a < min) ? min: a)


void CTree::Init()
{
	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);


	m_Model = new CModel();
	m_Model->Load("data/MODEL/branch.obj");

	m_Seed = 0;
}


void CTree::Uninit()
{

	m_Model->Unload();

}


void CTree::Update()
{

	m_Rotation.y += 0.01f;


	if (CInput::GetKeyTrigger(VK_F1))
		m_Seed++;
	if (CInput::GetKeyTrigger(VK_F2))
		m_Seed--;

}

void CTree::Draw()
{

	srand(m_Seed);

	// マトリクス設定
	XMMATRIX world;
	world = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	world *= XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	world *= XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	CRenderer::SetWorldMatrix(&world);

	m_Model->Draw();

	DrawChild(world, 1.0f);
}



//再帰的に枝描画/////////////////////////////////////////////////
void CTree::DrawChild(XMMATRIX Parent, float Scale)
{
	if (Scale < 0.6f)
		return;

	float rot_z = (float)rand() / RAND_MAX * 1.0f - 0.5f;
	XMMATRIX world, Rotation;
	for (int i = 0; i < 3; i++)
	{
		world = XMMatrixScaling(Scale, Scale, Scale);
		world *= GET_ROT;
		world *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		world *= XMMatrixTranslation(0.0f, 2.0f, 0.0f);
		world *= Parent;
		CRenderer::SetWorldMatrix(&world);

		m_Model->Draw();

		DrawChild(world, Scale * SCALING);
	}
}


