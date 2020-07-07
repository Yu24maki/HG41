#pragma once


class CSky : public CGameObject
{
private:

	ID3D11Buffer*	m_VertexBuffer = NULL;
	ID3D11Buffer*	m_IndexBuffer = NULL;
	CTexture*		m_Texture = NULL;


	static const int SKY_X = 32;
	static const int SKY_Z = 32;

	VERTEX_3D m_Vertex[SKY_X * SKY_Z];


	float			m_LightRotation;


public:
	void Init();
	void Uninit();
	void Update();
	void Draw();


};