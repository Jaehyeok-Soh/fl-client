#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::NAVIGATION;
	typedef struct tagNavigationDesc
	{
		const _float4x4* pParentMatrix = { nullptr };
		_int iCurrentIndex = { -1 };
		_float3 vPosition = { 0.f, 0.f, 0.f };
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

	virtual HRESULT Initialize_Prototype(const POLYGON_SAVEDATA& polygonData);
	virtual HRESULT Initialize(void* pArg) override;
public:
	_int Get_CurrentCellIndex() const { return m_iCurrentCellIndex; }
	void Set_CurrentCellIndeX(const _int iIndex) { m_iCurrentCellIndex = iIndex; }
	_vector Get_CellPos();
	_vector Get_CellNormal();
	_vector Get_CellNormal_World();
	_vector SetUp_OnNavigation(_fvector vWorldPos);
	_vector Project_OnNavigation(_fvector vWorldPos);
	void Sync_Index(_fvector vWorldPos);
	_bool Is_Move(_fvector vResultPos);
private:
	HRESULT SetUp_Cells(const POLYGON_SAVEDATA& polygonData);
private:
	const _float4x4* m_pParentMatrix = { nullptr };
private:
	_uint m_iCellCount = { 0 };
	_int m_iCurrentCellIndex = { -1 };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	vector<class CCell*> m_vecCells;
public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const POLYGON_SAVEDATA &polygonData);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END