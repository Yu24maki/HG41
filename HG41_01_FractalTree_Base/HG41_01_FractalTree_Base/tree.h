#pragma once


class CTree : public CGameObject
{
private:

	CModel* m_Model;

	int		m_Seed;

public:
	void Init();
	void Uninit();
	void Update();
	void Draw();
	void DrawChild(XMMATRIX Parent, float Scale);

};