#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
	using Super = CVIBuffer;
public:
	typedef struct tagVIBufferTerrainOriginDesc : public Super::VIBUFFER_ORIGIN_DESC
	{
		const _tchar* pHeightFileMapPath = { nullptr };
		_uint iRow_VerticesCount = { 65 };
		_uint iCol_VerticesCount = { 65 };
		_float fHeightRatio = { 10.f };
	}VIBUFFER_TERRAIN_ORIGIN_DESC;
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Resize(VIBUFFER_TERRAIN_ORIGIN_DESC *pDesc);
	_uint Get_ColCount() const { return m_iCol_VerticesCount; }
	_uint Get_RowCount() const { return m_iRow_VerticesCount; }
private:
	HRESULT Set_HeightMapTerrain(const _tchar* pHeightFileMapPath);
	HRESULT Set_DefaultTerrain();
	HRESULT Resize_HeightMap();
	HRESULT Resize_Default();
private:
	_float m_fHeightRatio = { 10.f };
	_uint m_iCol_VerticesCount = { 65 };
	_uint m_iRow_VerticesCount = { 65 };
public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END