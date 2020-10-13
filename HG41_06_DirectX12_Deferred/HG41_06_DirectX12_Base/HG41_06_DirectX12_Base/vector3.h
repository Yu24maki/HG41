//
//	vector3.h
//
//					Author:Yu Nishimaki	Date:2019/12/09
//-----------------------------------------------------
//	変更点
//	Date:yyyy/mm/dd
//
//=====================================================

//=====================================================
//	インクルードガード
//=====================================================
#pragma once


//=====================================================
//	インクルード	include
//=====================================================
#include <cmath>


//=====================================================
//	Vector3クラス		class
//=====================================================

class Vector3
{
public:
	//-----------------------------------------
	//	メンバ変数
	//-----------------------------------------
	float x;
	float y;
	float z;

public:
	//-----------------------------------------
	//	コンストラクタ
	//-----------------------------------------
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3(XMFLOAT3 _invec) :Vector3(_invec.x, _invec.y, _invec.z) {}
	Vector3(){}


	//-----------------------------------------
	//	機能
	//-----------------------------------------
	float LengthSq()const { return x * x + y * y + z * z; }		// 長さの二乗
	float Length()const { return std::sqrt(this->LengthSq()); }		// 長さ
	Vector3 Normalized()const { float L = Length(); return (L != 0)?Vector3(x / L, y / L, z / L) :Vector3(0.0f, 0.0f, 0.0f); }		// 単位ベクトル
	void Normalize() { *(this) = this->Normalized(); }		// 単位ベクトル
	XMFLOAT3 ToXMFLOAT3()const { return XMFLOAT3(x, y, z); }
	static float dot(Vector3 a, Vector3 b) { return (a.x * b.x + a.y * b.y + a.z * b.z); }		// 内積
	static Vector3 cross(Vector3 a, Vector3 b) { return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }		// 外積


	//-----------------------------------------
	//	演算子オーバーロード
	//-----------------------------------------
	Vector3 operator +()const { return *this; }
	Vector3 operator -()const { return { -x, -y, -z }; }
	Vector3 operator +(Vector3 other)const { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 operator -(Vector3 other)const { return { x - other.x, y - other.y, z - other.z }; }
	Vector3 operator *(float f)const { return { x * f, y * f, z * f }; }
	Vector3 operator /(float f)const { return { x / f, y / f, z / f }; }
	Vector3 operator *(Vector3 other)const { return cross(*this, other); }
	Vector3 operator =(XMFLOAT3 other)const { return { other.x, other.y, other.z }; }
};

inline Vector3 operator*(float f, Vector3 F3) { return { f * F3.x, f * F3.y, f * F3.z }; }

