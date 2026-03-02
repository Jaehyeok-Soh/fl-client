#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUIDynamic_Image abstract : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUIDimageDesc : public GENERIC_UI_DESC
	{
		DTO::EUIDImageSubClassType eSubClassType;
	}DIMAGE_DESC;

protected:
	CUIDynamic_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIDynamic_Image(const CUIDynamic_Image& rhs);
	virtual ~CUIDynamic_Image() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components(DIMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();

protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;
	virtual HRESULT Despawn_FromPool()override;
protected:
	DTO::EUIDImageSubClassType m_eDImageSubClass = {};




public:
	CGameObject* Clone(void* pArg)PURE;
	virtual void Free()override;
};

NS_END