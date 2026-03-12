#pragma once
#include "Tool_EffectBase.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CGameObject;

NS_END

NS_BEGIN(Tool)

class Effect : public CTool_EffectBase
{
	using Super = CTool_EffectBase;

public:
	typedef struct tagToolObjectDesc : public Super::TOOLOBJECT_DESC
	{
		DTO::E_SIMULATION_SPACE _Effect_SimulationType = DTO::E_SIMULATION_SPACE::WORLD;
		_bool	_IsPoolingEffect = true;
		vector<DTO::TEFFECT_PartsData>	_childData = {};
	}EFFECT_CONTAINERDESC;

protected:
	Effect(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Effect(const Effect& rhs);
	virtual ~Effect() = default;

protected:
	HRESULT Ready_PartsData(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _bool Picking(OUT Vec3& vOut) override;
	virtual _bool Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument) override;
	virtual void Draw_ImGui() override;
public:
	void Set_SimulationSpace(DTO::E_SIMULATION_SPACE Space) { m_eDesc._Effect_SimulationType = Space; }
	const DTO::E_SIMULATION_SPACE& Get_SimulationSpace() { return m_eDesc._Effect_SimulationType; }

	EFFECT_CONTAINERDESC* Get_ContainerData() { return &m_eDesc; }

protected:
	void Update_CombinedWorldMatrix(const Matrix* pMatParent);
	void IsEffectFinish();

protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;
	virtual HRESULT Despawn_FromPool() override;

public:
	static Effect* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

protected:
	EFFECT_CONTAINERDESC			m_eDesc = {};
	Matrix							m_pOffsetMartix = {};
	const Matrix*					m_pBoneMatrix = { nullptr };
	const Matrix*					m_pBoneOwnerMatrix = { nullptr };
	_uint							m_iBoneFlag = {};
	Matrix							m_matCombinedWorld = {};
};

NS_END