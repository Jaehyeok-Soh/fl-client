#pragma once
#include "GameObject.h"
#include "Camera.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCharacter abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagCharacterDesc : public Super::GAMEOBJECT_DESC
	{
		_uint iA = { 0 };
	}CHARACTER_DESC;
protected:
	explicit CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	
protected:
	
public:
	virtual CGameObject* Clone(void *pArg) PURE;
	virtual void Free() override;
};

NS_END