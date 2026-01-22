#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
	using Super = CBase;
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;

	HRESULT Initialize(const _float3* pPoints, _int iIndex);
public:
	void Set_Index(_int iIndex) { m_iIndex = iIndex; }
	_int Get_Index() const { return m_iIndex; }
	_vector Project_OnPlane(_fvector vPos) const;
	_vector Get_Point(EPOINT ePoint) const { return ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ePoint)]); }
	_vector Get_Normal(ELINE eLine) const { return ::XMLoadFloat3(&m_vNormals[ENUM_TO_UINT(eLine)]); }
	_vector Get_UpNormal() const { return ::XMLoadFloat3(&m_vUpNormal); }
	void Get_Neighbors(OUT _int* pNeighbors) { ::memcpy(pNeighbors, m_iNeighbors, sizeof(_int) * 3); }
	void Set_Neighbor(ELINE eLine, CCell* pNeighbor) { m_iNeighbors[ENUM_TO_UINT(eLine)] = pNeighbor->m_iIndex; }
	void Set_Neighbor(ELINE eLine, _int iIndex) { m_iNeighbors[ENUM_TO_UINT(eLine)] = iIndex; }
	void Set_Normal(ELINE eLine, const _float3 &vValue) { m_vNormals[ENUM_TO_UINT(eLine)] = vValue; }
	_vector Get_CenterPos();
public:
	_bool Is_In(_fvector vResultPos, _int* pNeighborIndex);
	_bool Compare(_fvector vSourPoint, _fvector vDestPoint);
	_float Compute_Height(_vector vCellPos);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
private:
	_float3 m_vUpNormal = { 0.f, 1.f, 0.f };
	_float3 m_vPoints[ENUM_TO_UINT(EPOINT::END)] = {};
	_float3 m_vNormals[ENUM_TO_UINT(ELINE::END)] = {};
	_int m_iNeighbors[ENUM_TO_UINT(ELINE::END)] = { -1, -1, -1 };
	_int m_iIndex = { -1 };

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

NS_END