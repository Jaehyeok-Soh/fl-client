#pragma once
#include "Base.h"
#include "Animation_Defines.h"
#include "Anim_Event_Info.h"
#include "PhysicsAttackOverlap.h"

NS_BEGIN(Engine)
class  CGameInstance;
NS_END

NS_BEGIN(Tool)

class CEvent_Overlap_Module final : public CBase
{
public:
	using Super = CBase;

private:
	CEvent_Overlap_Module(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CEvent_Overlap_Module() = default;

	HRESULT Initialize();

public:
	void Update(const _float fTimeDelta);

	void Render();

	void SetAttackOverlap(CPhysicsAttackOverlap* pAttackOverlap, CAnimObj* pOwner);
	void SetOwner(CAnimObj* pOwner);
	vector<DTO::ATTACKEVENT>& GetEvents() { return m_pOverlap->GetEvents(); }

	void Modify_AttackOverlap(_uint eventIdx, DTO::ATTACKEVENT event);
	void Modify_AttackOverlap(vector<DTO::ATTACKEVENT> events);

	void Awake();

private:
	void Create_AttackOverlap(vector<DTO::ATTACKEVENT> events);

private:
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pDeviceContext{};

	CGameInstance* m_pGameInstance{ nullptr };

private:
	ANIMCTRLINFO* m_pAnimControllInfo{};

	DTO::ANIM_EVENT_INFO1* m_pEventInfo{};

	CPhysicsAttackOverlap* m_pOverlap = { nullptr };

public:
	static CEvent_Overlap_Module* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END
