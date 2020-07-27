#pragma once

class CLight : public CGameObject
{
private:
	float m_Rotation;
	static LIGHT m_Light;

public:
	static LIGHT GetInstance() { return m_Light; }

public:
	void Init();
	void Uninit();
	void Update();
	void Draw();
};