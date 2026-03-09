#pragma once
#include "HybridGameObject_Base.h"

NS_BEGIN(Client)

class CHybrid_WarningSpace :
	public CHybridGameObject_Base
{
	using Super = CHybridGameObject_Base;
public:
	enum class EWarningState
	{
		NONE = -1,
		WARNING = 0,
		EXPLOSION = 1,
		END
	};

	typedef struct tagOrigin_HybridWarningDesc : public Super::Origin_HybridDesc
	{

	}Origin_HybridWarningDesc;

protected:
	// TODO - type 다 지정해줘야됨, 현재 툴때문이 임시로
	CHybrid_WarningSpace(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHybrid_WarningSpace(const CHybrid_WarningSpace& rhs);
	virtual ~CHybrid_WarningSpace() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render()  override;
	inline virtual HRESULT Spawn_FromPool(void* pArg)  override;
	inline virtual HRESULT Despawn_FromPool()  override;

private:
	virtual void Change_HybridState(_uint iState) override;
	virtual void Update_CheckState() override;

public:
	static CHybrid_WarningSpace* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	vector<EFFECT_WARNING_DESC>			m_tSpawnDesc = {};
};

NS_END