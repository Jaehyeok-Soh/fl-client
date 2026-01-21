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
		_float4x4 matTransform = {};
	}BONE_DESC;
private:
	CBone();
	virtual ~CBone() = default;

	HRESULT Initialize(BONE_DESC* pDesc);
public:
	_bool Compare_Name(const _char* pName) { return !::strcmp(pName, m_szName); }
	void Set_TransformationMatrix(const _float4x4& matTransformation) { m_matTransform = matTransformation; }
	void Set_TransformationMatrix(_fmatrix matTransformation) { ::XMStoreFloat4x4(&m_matTransform, matTransformation); }
	_matrix Get_CombinedTransformMatrix() { return ::XMLoadFloat4x4(&m_matCombinedTransform); }
	_matrix Get_BindPoseTransformMatrix() { return ::XMLoadFloat4x4(&m_matBindPoseTransform); }
	const _float4x4& Get_BindPoseTransformMatrixFloat() const { return m_matBindPoseTransform; }
	const _float4x4 &Get_CombinedTransformMatrixFloat() const { return m_matCombinedTransform; }
	_int Get_Index() const { return m_iIndex; }
	_int Get_ParentIndex() const { return m_iParentIndex; }
	void Update_CombinedTransformMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	void Setup_BindPoseTransformMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
private:
	_int m_iIndex = { -1 };
	_int m_iParentIndex = { -1 };
	_float4x4 m_matTransform = {};
	_float4x4 m_matBindPoseTransform = {};
	_float4x4 m_matCombinedTransform = {};
	_char m_szName[MAX_NAME] = {};
public:
	static CBone* Create(BONE_DESC* pDesc);
	CBone* Clone();
	virtual void Free() override;
};

NS_END