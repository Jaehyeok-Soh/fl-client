#pragma once
#include "MapObject.h"

class CWater : public CMapObject
{
	using Super = Client::CMapObject;
public:
	typedef struct Water_Desc : public CMapObject::MAPOBJECT_DESC
	{
		Vec4		 vMI_TintColor{1.f,1.f ,1.f ,1.f };
		CB_WaterData tCBWaterData{};
		/* 텍슬러 SRV들을 그대로받아오기 */
		array<ID3D11ShaderResourceView*, ENUM_TO_UINT(EWaterTextureType::END)>	arrayWaterSRVs{};
	}WATER_DESC;
protected:
	CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWater(const CWater& rhs);
	virtual ~CWater() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(WATER_DESC* pDesc);
public:
	HRESULT					Bind_Water(class CShader* pShader);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
private:
	Vec4									m_vMI_TintColor{1.f,1.f,1.f,1.f};
	array<ID3D11ShaderResourceView*, ENUM_TO_UINT(EWaterTextureType::END)>	m_arrayWaterSRVs{};
	CB_WaterData							m_tCBWaterData{};
	ID3DX11EffectConstantBuffer*			m_pEffectCBBuffer{};
	ID3DX11EffectShaderResourceVariable*	m_pEffectSRV;
public:
	static CWater*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual					void Free() override;
};

