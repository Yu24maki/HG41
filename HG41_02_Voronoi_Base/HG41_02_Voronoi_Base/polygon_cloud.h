#pragma once


#include "shader.h"


class CPolygonCloud : public CGameObject
{
private:

	ID3D11Buffer*	m_VertexBuffer = NULL;
	CShader*		m_Shader;

public:
	void Init();
	void Uninit();
	void Update();
	void Draw();


};