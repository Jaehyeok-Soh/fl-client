#pragma once
#include "GameObject.h"
#include "Camera.h"

NS_BEGIN(Engine)

enum class CameraType
{
	STATIC,
	DYNAMIC,
	END
};

class ENGINE_DLL CCameraMan abstract : public CGameObject
{
	using Super = CGameObject;
public:

protected:
	explicit CCameraMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CameraType eType);
	explicit CCameraMan(const CCameraMan& rhs);
	virtual ~CCameraMan() = default;

	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;

	CameraType Get_Type() const { return m_eType; }
	CGameObject* Get_Actor() { return m_pActor; }
	inline void Change_Actor(CGameObject* pGo);
private:
	HRESULT Ready_Components(void *pArg);
protected:
	CameraType m_eType = { CameraType::STATIC };
	CGameObject* m_pActor = { nullptr };
public:
	virtual CGameObject* Clone(void *pArg) PURE;
	virtual void Free() override;
};

NS_END