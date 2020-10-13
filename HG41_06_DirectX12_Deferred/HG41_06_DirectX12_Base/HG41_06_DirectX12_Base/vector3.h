//
//	vector3.h
//
//					Author:Yu Nishimaki	Date:2019/12/09
//-----------------------------------------------------
//	�ύX�_
//	Date:yyyy/mm/dd
//
//=====================================================

//=====================================================
//	�C���N���[�h�K�[�h
//=====================================================
#pragma once


//=====================================================
//	�C���N���[�h	include
//=====================================================
#include <cmath>


//=====================================================
//	Vector3�N���X		class
//=====================================================

class Vector3
{
public:
	//-----------------------------------------
	//	�����o�ϐ�
	//-----------------------------------------
	float x;
	float y;
	float z;

public:
	//-----------------------------------------
	//	�R���X�g���N�^
	//-----------------------------------------
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3(XMFLOAT3 _invec) :Vector3(_invec.x, _invec.y, _invec.z) {}
	Vector3(){}


	//-----------------------------------------
	//	�@�\
	//-----------------------------------------
	float LengthSq()const { return x * x + y * y + z * z; }		// �����̓��
	float Length()const { return std::sqrt(this->LengthSq()); }		// ����
	Vector3 Normalized()const { float L = Length(); return (L != 0)?Vector3(x / L, y / L, z / L) :Vector3(0.0f, 0.0f, 0.0f); }		// �P�ʃx�N�g��
	void Normalize() { *(this) = this->Normalized(); }		// �P�ʃx�N�g��
	XMFLOAT3 ToXMFLOAT3()const { return XMFLOAT3(x, y, z); }
	static float dot(Vector3 a, Vector3 b) { return (a.x * b.x + a.y * b.y + a.z * b.z); }		// ����
	static Vector3 cross(Vector3 a, Vector3 b) { return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }		// �O��


	//-----------------------------------------
	//	���Z�q�I�[�o�[���[�h
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

