#pragma once
#include "CameraMan.h"

NS_BEGIN(Tool)

class CCameraMan_Free final : public CCameraMan
{
	using Super = CCameraMan;
public:

private:
	CCameraMan_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCameraMan_Free(const CCameraMan_Free& rhs);
	virtual ~CCameraMan_Free() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
private:

private:

public:
	static CCameraMan_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END