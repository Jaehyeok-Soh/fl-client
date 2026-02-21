#pragma once
#include "Tool_ContainerObject.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CGameObject;

NS_END

NS_BEGIN(Tool)

class Effect : public Tool_ContainerObject
{
	using Super = Tool_ContainerObject;

public:
	typedef struct tagToolObjectDesc : public Super::TOOLOBJECT_DESC
	{
		DTO::E_SIMULATION_SPACE _Effect_SimulationType = DTO::E_SIMULATION_SPACE::NONE;
		vector<DTO::TEFFECT_PartsData>	_childData = {};
	}EFFECT_CONTAINERDESC;

protected:
	Effect(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Effect(const Tool_ContainerObject& rhs);
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
	virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
	virtual void Set_ParentsWorldMatrix(Matrix* worldMatrix) { m_pParentsWorldMatrix = worldMatrix; }
	virtual void Set_SimulationSpace(DTO::E_SIMULATION_SPACE Space) { m_eDesc._Effect_SimulationType = Space; }

	virtual const DTO::E_SIMULATION_SPACE& Get_SimulationSpace() { return m_eDesc._Effect_SimulationType; }
	virtual Matrix* Get_ParentsWorldMatrix() { return m_pParentsWorldMatrix; }

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
	Matrix*							m_pParentsWorldMatrix = { nullptr };
	Matrix							m_matCombinedWorld = {};
};

NS_END