#pragma once


class CCamera : public CGameObject
{
private:

	RECT m_Viewport;

public:
	void Init();
	void Uninit();
	void Update();
	void Draw();


};