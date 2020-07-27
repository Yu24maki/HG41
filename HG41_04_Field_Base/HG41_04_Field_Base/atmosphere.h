#pragma once


#include "shader.h"


class CAtmosphere : public CGameObject
{
private:

	ID3D11Buffer*	m_VertexBuffer = NULL;
	CShader*		m_Shader;

	CTexture*		m_Texture[2];

	XMFLOAT4		m_Blend;

	float			m_LightRotation;

public:
	void Init();
	void Uninit();
	void Update();
	void Draw();


};