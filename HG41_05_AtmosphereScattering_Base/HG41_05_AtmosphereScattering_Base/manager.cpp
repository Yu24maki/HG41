
#include "main.h"
#include "manager.h"
#include "input.h"
#include "renderer.h"
#include "game_object.h"
#include "field.h"
#include "model.h"
#include "scene.h"


CScene* CManager::m_Scene;


void CManager::Init()
{

	CRenderer::Init();
	CInput::Init();

	m_Scene = new CScene();
	m_Scene->Init();

}

void CManager::Uninit()
{

	m_Scene->Uninit();
	delete m_Scene;

	CInput::Uninit();
	CRenderer::Uninit();

}

void CManager::Update()
{

	CInput::Update();

	m_Scene->Update();

}

void CManager::Draw()
{

	CRenderer::Begin();

	m_Scene->Draw();

	CRenderer::End();

}

