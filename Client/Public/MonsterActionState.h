#pragma once
#include "ActionState.h"

NS_BEGIN(Client)

class CMonsterActionState final : public CActionState
{
	using Super = CActionState;

public:
	typedef struct tagMonsterActionStateDesc : public ACTIONSTATE_DESC
	{
		wstring wstrMonsterStateTag = {};
		_uint iLevelIndex = {0};
	}MONSTERACTIONSTATE_DESC;

private:
	CMonsterActionState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonsterActionState(const CMonsterActionState& rhs);
	virtual ~CMonsterActionState() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Bind_State(std::set<string> setState);
	DTO::MONSTERSTATE_DESC LoadStateFile(std::filesystem::path path, _uint iLevelIndex);
	HRESULT LoadStates(wstring stateTag, _uint iLevelIndex);

public:
	CControlContext* GetOwnerControlContext() { return m_pOwnerControlContext; }

	unordered_map<string, _int>& GetUmapState() { return m_umapState; }
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	DTO::MONSTERSTATE_DESC m_tDesc{};

	unordered_map<string, _int> m_umapState;

public:
	static CMonsterActionState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	friend CMonsterState_Factory;
};

NS_END