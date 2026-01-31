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

    // 1. 부모(Container) 데이터 생성 및 기본 정보 기입
    DTO::TEFFECT_ContainerData tContainerData;
    tContainerData.strTag = Get_Name();
    tContainerData.EffectContainerName = tContainerData.strTag;      // 툴 표시용 이름
    tContainerData.vWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix(); // 월드 행렬
    tContainerData._Effect_SimulationType = ENUM_TO_UINT(m_eSimulationSpace);      // 시뮬레이션 공간 (LOCAL/WORLD)

    // 2. 자식(Parts) 데이터 수집
    for (auto& pPart : m_vecPartObjects) // m_PartObjects는 부모 클래스에서 관리하는 자식 리스트
    {
		CEffectObject* pEffectPart = static_cast<CEffectObject*>(pPart);

		if (nullptr == pEffectPart) continue;

		const CEffectObject::Effect_Desc& tPartDesc = pEffectPart->Get_EffectDesc();
		DTO::TEFFECT_PartsData tPartData;

		// [기존 코드 유지 부분]
		tPartData.strTag = pEffectPart->Get_Name();
		tPartData.EffectPartsName = tPartData.strTag;
		tPartData.ParentsName = tContainerData.EffectContainerName;
		tPartData.vWorldMatrix = pEffectPart->Get_Component<CTransform>()->Get_WorldMatrix();

		tPartData.eEffectSystemType = ENUM_TO_UINT(tPartDesc.eEffectSystemType);
		tPartData.eEffectParticleType = ENUM_TO_UINT(tPartDesc.eEffectParticleType);
		tPartData.eEffectType = ENUM_TO_UINT(tPartDesc.eEffectType);
		tPartData._Effect_ShapeType = ENUM_TO_UINT(tPartDesc._Effect_ShapeType);

		// [리소스/셰이더]
		tPartData._Effect_Model_Tag = tPartDesc._Effect_Model_Tag;
		tPartData._Effect_DiffuseTexture_Tag = tPartDesc._Effect_DiffuseTexture_Tag;
		tPartData._Effect_NoiseTexture_Tag = tPartDesc._Effect_NoiseTexture_Tag;
		tPartData._Effect_MaskingTexture_Tag = tPartDesc._Effect_MaskingTexture_Tag;
		tPartData._Effect_GradationTexture_Tag = tPartDesc._Effect_GradationTexture_Tag;
		tPartData._Effect_TrailTexture_Tag = tPartDesc._Effect_TrailTexture_Tag;
		tPartData._Effect_NormalTexture_Tag = tPartDesc._Effect_NormalTexture_Tag;
		tPartData._Effect_Shader_Tag = tPartDesc._Effect_Shader_Tag;
		tPartData._Effect_ShaderPass = tPartDesc._Effect_ShaderPass;

		// [수치 데이터]
		tPartData._Effect_ScrollSpeed = tPartDesc._Effect_ScrollSpeed;
		tPartData._Effect_DistortionScale = tPartDesc._Effect_DistortionScale;
		tPartData._Effect_StartScale = tPartDesc._Effect_StartScale;
		tPartData._Effect_EndScale = tPartDesc._Effect_EndScale;
		tPartData._Effect_Color = tPartDesc._Effect_Color;
		tPartData._Effect_DiscardValue = tPartDesc._Effect_DiscardValue;
		tPartData._Effect_Range = tPartDesc._Effect_Range;
		tPartData._Effect_ParticleSize = tPartDesc._Effect_ParticleSize;

		// [스프라이트 애니메이션]
		tPartData._Effect_bUseSprite = tPartDesc._Effect_bUseSprite;
		tPartData._Effect_TileCount = { tPartDesc._Effect_TileCount.x, tPartDesc._Effect_TileCount.y };
		tPartData._Effect_bPlayAnim = tPartDesc._Effect_bPlayAnim;
		tPartData._Effect_AnimSpeed = tPartDesc._Effect_AnimSpeed;
		tPartData.m_iCurSpriteNumber = tPartDesc.m_iCurSpriteNumber;

		// [파티클 시스템 상세]
		tPartData._Effect_Duration = tPartDesc._Effect_Duration;
		tPartData._Effect_Looping = tPartDesc._Effect_Looping;
		tPartData._Effect_IsRandomSeed = tPartDesc._Effect_IsRandomSeed;
		tPartData._Effect_StartDelay = tPartDesc._Effect_StartDelay;
		tPartData._Effect_LifeTime = tPartDesc._Effect_LifeTime;
		tPartData._Effect_PlayBackSpeed = tPartDesc._Effect_PlayBackSpeed;
		tPartData._Effect_StartSpeed = tPartDesc._Effect_StartSpeed;
		tPartData._Effect_MaxParticle = tPartDesc._Effect_MaxParticle;
		tPartData._Effect_RateOverTime = tPartDesc._Effect_RateOverTime;
		tPartData._Effect_RateOverDistance = tPartDesc._Effect_RateOverDistance;

		// [플래그 정보]
		tPartData._Effect_TextureFlag = tPartDesc._Effect_TextureFlag;
		tPartData._Effect_RenderFlag = tPartDesc._Effect_RenderFlag;
		tPartData._Effect_SamplerStateFlag = tPartDesc._Effect_SamplerStateFlag;

		// [툴 전용 데이터까지 넣어줘야 JSON 로드 시 툴 UI 상태가 복원됨]
		tPartData._Effect_Tool_DiffuseTexture = tPartDesc._Effect_Tool_DiffuseTexture;
		tPartData._Effect_Tool_NoiseTexture = tPartDesc._Effect_Tool_NoiseTexture;

		tContainerData._ChildData.push_back(tPartData);
    }
    // 부모의 자식 리스트에 추가
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
