#pragma once
#include "PartObject.h"

NS_BEGIN(Client)

class CTriggerCollidePart final : public CPartObject
{
	using Super = CPartObject;
public:
	typedef struct tagColliderPartDesc : public CPartObject::PARTOBJ_DESC
	{
		PHYSICSRIGIDBODY_DESC* pRigidbodyDesc = { nullptr };
		PHYSICSCOLLIDER_DESC* pColliderDesc = { nullptr };
		const Matrix* pMatSocket = { nullptr }; // 필요 없다면 안 넣어줘도 됨
		Matrix vPreScale = { Matrix::Identity };
	}TRIGGER_COLLIDEPART_DESC;
private:
	enum EState
	{
		None = 0,
		WithBone,
		OnlyOwner
	};
private:
	CTriggerCollidePart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTriggerCollidePart(const CTriggerCollidePart& rhs);
	virtual ~CTriggerCollidePart() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

	virtual HRESULT Render() override;

	// getter setter funcs
public:
	const Vec3& Get_CollidedPos() const { return m_vColliedPos; }

private:
	HRESULT Ready_Components(TRIGGER_COLLIDEPART_DESC* pDesc);
private:
	EState m_eState{ EState::None };
	const Matrix* m_pMatSocket = { nullptr };
	Matrix m_matPreScale{ Matrix::Identity };

private:
	Vec3 m_vColliedPos = { Vec3::Zero };
	_int m_iColliedID = {-1};

public:
	static CTriggerCollidePart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

