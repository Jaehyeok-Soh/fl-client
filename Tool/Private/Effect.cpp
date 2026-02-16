#include "pch.h"
#include "Effect.h"
#include "DataStruct_Effect.h"
#include "DataDocument_Effect.h"
#include "CEffectObject.h"
#include "Engine_Utils.h"

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

	EFFECT_CONTAINERDESC* pDesc = static_cast<EFFECT_CONTAINERDESC*>(pArg);

	if (pDesc != nullptr)
		m_eSimulationSpace = pDesc->_Effect_SimulationType;

	else
		MSG_BOX("EFFECT_CONTAINERDESC nullptr");

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
	if (eCategory != DTO::ECategory::EFFECT)
		return false;

	CDataDocument_Effect* pEffectDoc = static_cast<CDataDocument_Effect*>(pDocument);

	// 부모(Container) 데이터 생성 및 기본 정보 기입
	DTO::TEFFECT_ContainerData tContainerData;
	tContainerData.strTag = Get_Name();
	tContainerData.EffectContainerName = tContainerData.strTag;
	tContainerData.vWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix();
	tContainerData._Effect_SimulationType = ENUM_TO_UINT(m_eSimulationSpace);

	// 자식(Parts) 데이터 수집
	for (auto& pPart : m_vecPartObjects)
	{
		CEffectObject* pEffectPart = dynamic_cast<CEffectObject*>(pPart);
		if (nullptr == pEffectPart) continue;

		DTO::TEFFECT_PartsData tPartData = pEffectPart->Get_EffectDesc();

		// =========== 정보 보정 (실시간 갱신이 필요한 값들) ===========
		tPartData.EffectPartsName = pEffectPart->Get_Name();
		tPartData.ParentsName = tContainerData.EffectContainerName;

		// 실시간 툴 조작으로 변한 Transform 행렬 업데이트
		tPartData.vWorldMatrix = pEffectPart->Get_Component<CTransform>()->Get_WorldMatrix();

		// 수집된 파츠 데이터를 컨테이너 리스트에 추가
		tContainerData._ChildData.push_back(tPartData);
	}

	// 부모 Document에 최종 컨테이너 추가 및 직렬화 준비
	return SUCCEEDED(pEffectDoc->Try_Add(tContainerData));
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
	Super::Set_Dead(wstrLayerTag);
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
