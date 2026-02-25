#pragma once
#include "Plants.h"

class CGrass : public CPlants
{
	using Super = Client::CPlants;
public:
	typedef struct Grass_Desc : public CPlants::PLANTS_DESC
	{

	}GRASS_DESC;
protected:
	CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGrass(const CGrass& rhs);
	virtual ~CGrass() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(GRASS_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	static CGrass*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual					void Free() override;
};

