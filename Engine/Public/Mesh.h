#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CRay;
class CComputeShader;
class StructuredBuffer;

typedef struct tagMeshRayHitInformation
{
	Vec3 vHitPos = { 0.f, 0.f, 0.f };
	Vec3 vNormal = { 0.f, 0.f, 0.f };
	_float fDistance = { 0.f };
	_int iTriangleIndex = { -1 };
}MESH_RAY_HITINFO;

class ENGINE_DLL CMesh final : public CVIBuffer
{
	using Super = CVIBuffer;
public:
	typedef struct tagMeshOriginDesc : public Super::VIBUFFER_ORIGIN_DESC
	{
		string strName = { "" };
		EModelType eModelType = { EModelType::END };
		_uint iMaterialIndex = { 0 };

		// VB
		std::span<VTXANIMMESH> spanVertex;
		_uint iVertexCount = { 0 };

		// IB
		std::span<_uint> spanIndex;
		_uint iIndexCount = { 0 };

		// AffectBones
		std::span<_uint> spanAffectBoneIndex;
		_uint iAffectBoneCount = { 0 };

		// OffsetMatrix
		std::span<Matrix> spanOffsetMatrices;
		_uint iOffsetMatricesCount = { 0 };

		// MinMax
		std::span<Vec3> spanMinMax;
		_uint iMinMaxCount = { 0 };

		// ETC
		D3D11_PRIMITIVE_TOPOLOGY ePrimitiveType = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

		// Flag
		_bool bSaveNormal = { false };
	}MESH_ORIGIN_DESC;

private:
	enum class CS_BONEMESH_IDX {IMMU_OFFSETMAT, MU_COMBINEMAT};
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	Vec3*			Get_Normals(_uint iIndex) const { return m_pNormals; }
	_uint			Get_MaterialIndex() const { return m_iMaterialIndex; }
	void			Set_MaterialIndex(_uint iIndex) { m_iMaterialIndex = iIndex; }
	HRESULT			Bind_Bones(class CShader* pShader, const vector<class CBone*>& vecBones, _uint iIndexDistance = 0);
	HRESULT			Bind_Bones(class CShader* pShader, CComputeShader* pBoneMeshCS, CComputeShader* pBoneCombineCS,_uint iTotalBoneNum,  _uint iIndexDistance = 0);
	_bool			IsSame(const _char* szName) { return ::strcmp(m_szName, szName) != 0; }
	_bool			IntsersectWithPlane(OUT Vec3& vOut);
	_bool			IntsersectWithPlane_CloseCam(OUT Vec3& vOut , const Vec3& vLocalCamPos);
	_bool			IntsersectWithPlane(CRay* const pRay, Matrix matWorld, _float fMaxDistance, OUT MESH_RAY_HITINFO& outHit);
	const			Vec3* Get_MinMax() const { return m_pMinMax; }
private:
	HRESULT			Load_AnimVertices(std::span<VTXANIMMESH> spanVertex);
	HRESULT			Load_NonAnimVertices(std::span<VTXANIMMESH> spanVertex);

public:
	HRESULT Ready_BindCSBuffer(CComputeShader* pBoneMeshCS);
private:
	_char m_szName[MAX_NAME] = {};

	_bool m_bHasMinMax{ false };
	_uint m_iMaterialIndex = { 0 };
	_uint m_iAffectBoneCount = { 0 };
	_uint m_iOffsetMatrixCount = { 0 };

	SHADER_BONEDESC m_tBoneMatrices;
	Vec3* m_pMinMax{nullptr};
	_uint* m_pAffectBoneIndices{ nullptr };
	Matrix* m_pOffsetMatrices{ nullptr };
	Vec3* m_pNormals = { nullptr };


	EModelType m_eModelType = { EModelType::END };

private:
	StructuredBuffer*						m_pBoneMesh_ImmuBuffer	= { nullptr };
	ID3DX11EffectShaderResourceVariable*	m_pBoneMeshSB_SRV		= { nullptr };

private:
	HRESULT Ready_CS_Buffer();

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END