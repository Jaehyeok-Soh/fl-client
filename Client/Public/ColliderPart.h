#pragma once
#include "PartObject.h"

NS_BEGIN(Client)

class CColliderPart final : public CPartObject
{
	using Super = CPartObject;
public:
	typedef struct tagColliderPartDesc : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pMatSocket = { nullptr };
	}COLLIDERPART_DESC;
private:
	CColliderPart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CColliderPart(const CColliderPart& rhs);
	virtual ~CColliderPart() = default;

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
private:
	HRESULT Ready_Components(COLLIDERPART_DESC* pDesc);
private:
	const _float4x4* m_pMatSocket = { nullptr };
public:
	static CColliderPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

