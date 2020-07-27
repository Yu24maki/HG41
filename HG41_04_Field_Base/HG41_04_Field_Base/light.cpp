
#include "main.h"
#include "renderer.h"
#include "game_object.h"
#include "light.h"
#include "input.h"

LIGHT CLight::m_Light = {};

void CLight::Init()
{
	m_Rotation = 0.0f;

	LIGHT light;
	light.Direction = XMFLOAT4(0.0f, -cosf(m_Rotation), sinf(m_Rotation), 0.0f);
	light.Diffuse = COLOR(0.9f, 0.9f, 0.9f, 1.0f);
	light.Ambient = COLOR(0.1f, 0.1f, 0.1f, 1.0f);

	m_Light = light;
}

void CLight::Uninit()
{

}

void CLight::Update()
{
	if (CInput::GetKeyPress('W'))
		m_Rotation += 0.01f;
	if (CInput::GetKeyPress('S'))
		m_Rotation -= 0.01f;


	LIGHT light;
	light.Direction = XMFLOAT4(0.0f, -cosf(m_Rotation), sinf(m_Rotation), 0.0f);
	light.Diffuse = COLOR(0.9f, 0.9f, 0.9f, 1.0f);
	light.Ambient = COLOR(0.1f, 0.1f, 0.1f, 1.0f);

	m_Light = light;
}

void CLight::Draw()
{

}