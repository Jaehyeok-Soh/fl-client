#pragma once
#include "Plants.h"

class CTree : public CPlants
{
	using Super = Client::CPlants;
public:
	typedef struct Tree_Desc : public CPlants::PLANTS_DESC
	{

	}TREE_DESC;
protected:
	CTree(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTree(const CTree& rhs);
	virtual ~CTree() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(TREE_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	static CTree*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual					void Free() override;
};

