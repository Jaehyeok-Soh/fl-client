#include "pch.h"
#include "Effect.h"

Effect::Effect(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Tool_ContainerObject(eType, pDevice, pDeviceContext)
{
}

Effect::Effect(const Tool_ContainerObject& rhs)
	:Tool_ContainerObject(rhs)
{
}

HRESULT Effect::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Effect::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT Effect::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void Effect::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void Effect::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_eSimulationSpace == E_SIMULATION_SPACE::LOCAL && m_pParentsWorldMatrix != nullptr)
	{
		Update_CombinedWorldMatrix(m_pParentsWorldMatrix);
	}
}

void Effect::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void Effect::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT Effect::Render()
{
	return Super::Render();
}

_bool Effect::Picking(OUT Vec3& vOut)
{
	return Super::Picking(vOut);
}

_bool Effect::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
	return false;
}

void Effect::Draw_ImGui()
{
	Super::Draw_ImGui();
}

void Effect::Update_CombinedWorldMatrix(const Matrix* pMatParent)
{
	m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix() * (*pMatParent);
}


void Effect::Set_Dead(const wstring& wstrLayerTag)
{
}

Effect* Effect::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	Effect* pInstance = new Effect(eType, pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : Effect");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : Effect");
		return nullptr;
	}

	return pInstance;
}


CGameObject* Effect::Clone(void* pArg)
{
	Effect* pClone = new Effect(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Effect::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void Effect::Free()
{
	Super::Free();
}
