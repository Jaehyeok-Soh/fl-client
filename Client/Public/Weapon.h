#pragma once
#include "PartObject.h"

NS_BEGIN(Client)

class CWeapon final : public CPartObject
{
	using Super = CPartObject;
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJ_DESC
	{
		wstring wstrModelPrototypeName = { L"" };
		const Matrix* pMatHandSocket = { nullptr };
		const Matrix* pMatSocket = { nullptr };
	}WEAPON_DESC;
private:
	enum State
	{
		HAND,
		NONE
	};
private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnCollision(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual HRESULT Render() override;
	void Set_HandSocket();
	void Set_DefaultSocket();
	_bool Is_Hand() const { return m_eState == HAND; }
private:
	HRESULT Ready_Components(WEAPON_DESC* pDesc);
	HRESULT Bind_ShaderResources();
private:
	State m_eState = { State::NONE };
	const Matrix* m_pMatHandSocket = { nullptr };
	const Matrix* m_pMatSocket = { nullptr };
public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END