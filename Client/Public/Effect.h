#pragma once
#include "Client_Defines.h"
#include "EffectBase.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CGameInstance;
NS_END

NS_BEGIN(Client)

class Effect : public CEffectBase
{
	using Super = CEffectBase;

public:
	typedef struct tagEffectContainerDesc : public Super::GAMEOBJECT_DESC
	{
		DTO::E_SIMULATION_SPACE _Effect_SimulationType = DTO::E_SIMULATION_SPACE::NONE;

		vector<DTO::TEFFECT_PartsData>	_childData = {};
	}EFFECT_CONTAINERDESC;

protected:
	Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Effect(const Effect& rhs);
	virtual ~Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:
	HRESULT Ready_PartsData(void* pArg);

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _bool Picking(OUT Vec3& vOut) override;
	virtual _bool Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument) override;
	virtual void Set_Dead(const wstring& wstrLayerTag) override;

protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;
	virtual HRESULT Despawn_FromPool() override;

public:
	virtual void Set_ParentsWorldMatrix(Matrix* worldMatrix) { m_pParentsWorldMatrix = worldMatrix; }
	virtual void Set_SimulationSpace(DTO::E_SIMULATION_SPACE Space) { m_eSimulationSpace = Space; }

	virtual const DTO::E_SIMULATION_SPACE& Get_SimulationSpace() { return m_eSimulationSpace; }
	virtual Matrix* Get_ParentsWorldMatrix() { return m_pParentsWorldMatrix; }

protected:
	void Update_CombinedWorldMatrix(const Matrix* pMatParent);
	void IsEffectFinish();

public:
	static Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

protected:
	DTO::E_SIMULATION_SPACE				m_eSimulationSpace = {};
	Matrix*								m_pParentsWorldMatrix = { nullptr };
	Matrix								m_matCombinedWorld = {};

protected:
	string								m_szName = {};
	CGameInstance*						m_pGameInstance = { nullptr };
};

NS_END