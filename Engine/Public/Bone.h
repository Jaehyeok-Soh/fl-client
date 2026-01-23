#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
	using Super = CBase;
public:
	typedef struct tagBoneDesc
	{
		string strName = { "" };
		_int iIndex = { -1 };
		_int iParentIndex = { -1 };
		Matrix matTransform = Matrix::Identity;
	}BONE_DESC;
private:
	CBone();
	virtual ~CBone() = default;

	HRESULT Initialize(BONE_DESC* pDesc);
public:
	_bool Compare_Name(const _char* pName) { return !::strcmp(pName, m_szName); }
	void Set_TransformationMatrix(const Matrix& matTransformation) { m_matTransform = matTransformation; }
	const Matrix &Get_CombinedTransformMatrix() { return m_matCombinedTransform; }
	const Matrix &Get_BindPoseTransformMatrix() { return m_matBindPoseTransform; }
	_int Get_Index() const { return m_iIndex; }
	_int Get_ParentIndex() const { return m_iParentIndex; }
	void Update_CombinedTransformMatrix(const vector<CBone*>& Bones, const Matrix &PreTransformMatrix);
	void Setup_BindPoseTransformMatrix(const vector<CBone*>& Bones, const Matrix &PreTransformMatrix);
private:
	_int m_iIndex = { -1 };
	_int m_iParentIndex = { -1 };
	Matrix m_matTransform = {};
	Matrix m_matBindPoseTransform = {};
	Matrix m_matCombinedTransform = {};
	_char m_szName[MAX_NAME] = {};
public:
	static CBone* Create(BONE_DESC* pDesc);
	CBone* Clone();
	virtual void Free() override;
};

NS_END