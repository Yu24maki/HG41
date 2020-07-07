#pragma once

#include <list>
#include <vector>
#include <typeinfo>
#include "main.h"
#include "input.h"

#include "game_object.h"

#include "camera.h"
#include "model.h"
#include "sky.h"
#include "field.h"

class CScene
{
protected:
	static const int m_LayerMax = 5;

	std::list<CGameObject*>	m_GameObject[ m_LayerMax ];

public:
	CScene(){}
	virtual ~CScene(){}


	virtual void Init()
	{
		AddGameObject<CCamera>( 0 );
		AddGameObject<CSky>( 1 );
		AddGameObject<CField>( 1 );
	}

	virtual void Uninit()
	{
		for( int i = 0; i < m_LayerMax; i++ )
		{
			for (CGameObject* object : m_GameObject[i])
			{
				object->Uninit();
				delete object;
			}
		}
	}


	virtual void Update()
	{
		for( int i = 0; i < m_LayerMax; i++ )
		{
			for( CGameObject* object : m_GameObject[i] )
				object->Update();

			m_GameObject[i].remove_if( [](CGameObject* object){ return object->Destroy(); } );
		}
	}


	virtual void Draw()
	{
		for( int i = 0; i < m_LayerMax; i++ )
		{
			for( CGameObject* object : m_GameObject[i] )
				object->Draw();
		}
	}


	template <typename T>
	T* AddGameObject( int LayerIndex )
	{
		T* gameObject = new T();
		gameObject->Init();
		m_GameObject[ LayerIndex ].push_back( gameObject );

		return gameObject;
	}


	void DestroyGameObject( CGameObject* GameObject )
	{
		GameObject->SetDestroy();
	}


	template <typename T>
	T* GetGameObject( int LayerIndex )
	{
		for( CGameObject* object : m_GameObject[ LayerIndex ] )
		{
			if( typeid(*object) == typeid(T) )
			{
				return (T*)object;
			}
		}

		return nullptr;
	}

	template <typename T>
	std::vector<T*> GetGameObjects( int LayerIndex )
	{
		std::vector<T*> objects;
		for( CGameObject* object : m_GameObject[ LayerIndex ] )
		{
			if( typeid(*object) == typeid(T) )
			{
				objects.push_back((T*)object);
			}
		}

		return objects;
	}

};