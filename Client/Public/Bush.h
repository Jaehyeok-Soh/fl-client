#pragma once
#include "Plants.h"

class CBush : public CPlants
{
	using Super = Client::CPlants;
public:
	typedef struct Bush_Desc : public CPlants::PLANTS_DESC
	{

	}BUSH_DESC;
protected:
	CBush(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBush(const CBush& rhs);
	virtual ~CBush() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(BUSH_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	static CBush*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual					void Free() override;
};

