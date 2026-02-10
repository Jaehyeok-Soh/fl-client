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

    // 1. 부모(Container) 데이터 생성 및 기본 정보 기입
    DTO::TEFFECT_ContainerData tContainerData;
    tContainerData.strTag = Get_Name();
    tContainerData.EffectContainerName = tContainerData.strTag;
    tContainerData.vWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix();
    tContainerData._Effect_SimulationType = ENUM_TO_UINT(m_eSimulationSpace);

    // 2. 자식(Parts) 데이터 수집
    for (auto& pPart : m_vecPartObjects)
    {
        CEffectObject* pEffectPart = dynamic_cast<CEffectObject*>(pPart);
        if (nullptr == pEffectPart) continue;

        const CEffectObject::Effect_Desc& tPartDesc = pEffectPart->Get_EffectDesc();
        DTO::TEFFECT_PartsData tPartData;

        // =========== Document 기본 정보 ===========
        tPartData.strTag = pEffectPart->Get_Name();
        tPartData.EffectPartsName = tPartData.strTag;
        tPartData.ParentsName = tContainerData.EffectContainerName;
        tPartData.vWorldMatrix = pEffectPart->Get_Component<CTransform>()->Get_WorldMatrix();

        // =========== Effect 타입 및 모양 (Emission 추가) ===========
        tPartData.eEffectSystemType = ENUM_TO_UINT(tPartDesc.eEffectSystemType);
        tPartData.eEffectParticleType = ENUM_TO_UINT(tPartDesc.eEffectParticleType);
        tPartData.eEffectType = ENUM_TO_UINT(tPartDesc.eEffectType);
        tPartData._Effect_ShapeType = ENUM_TO_UINT(tPartDesc._Effect_ShapeType);
        tPartData._Effect_EmissionType = ENUM_TO_UINT(tPartDesc._Effect_EmissionType);

        // =========== Resource & Shader ===========
        tPartData._Effect_Model_Tag = tPartDesc._Effect_Model_Tag;
        tPartData._Effect_DiffuseTexture_Tag = tPartDesc._Effect_DiffuseTexture_Tag;
        tPartData._Effect_NoiseTexture_Tag = tPartDesc._Effect_NoiseTexture_Tag;
        tPartData._Effect_MaskingTexture_Tag = tPartDesc._Effect_MaskingTexture_Tag;
        tPartData._Effect_GradationTexture_Tag = tPartDesc._Effect_GradationTexture_Tag;
        tPartData._Effect_TrailTexture_Tag = tPartDesc._Effect_TrailTexture_Tag;
        tPartData._Effect_NormalTexture_Tag = tPartDesc._Effect_NormalTexture_Tag;
        tPartData._Effect_Shader_Path = tPartDesc._Effect_Shader_Path;
        tPartData._Effect_Shader_Tag = tPartDesc._Effect_Shader_Tag;
        tPartData._Effect_ShaderPass = tPartDesc._Effect_ShaderPass;

        // =========== 물리 및 수치 데이터 (Spiral/Discard 추가) ===========
        tPartData._Effect_ScrollSpeed = tPartDesc._Effect_ScrollSpeed;
        tPartData._Effect_DistortionScale = tPartDesc._Effect_DistortionScale;
        tPartData._Effect_StartScale = tPartDesc._Effect_StartScale;
        tPartData._Effect_EndScale = tPartDesc._Effect_EndScale;
        tPartData._Effect_Color = tPartDesc._Effect_Color;
        tPartData._Effect_DiscardValue = tPartDesc._Effect_DiscardValue;
        tPartData._Effect_Range = tPartDesc._Effect_Range;
        tPartData._Effect_ParticleSize = tPartDesc._Effect_ParticleSize;
        tPartData._Effect_Spiral_Radius = tPartDesc._Effect_Spiral_Radius;
        tPartData._Effect_Spiral_Speed = tPartDesc._Effect_Spiral_Speed;

        // =========== 애니메이션 & 파티클 시스템 ===========
        tPartData._Effect_bUseSprite = tPartDesc._Effect_bUseSprite;
        tPartData._Effect_TileCount = { tPartDesc._Effect_TileCount.x, tPartDesc._Effect_TileCount.y };
        tPartData._Effect_bPlayAnim = tPartDesc._Effect_bPlayAnim;
        tPartData._Effect_AnimSpeed = tPartDesc._Effect_AnimSpeed;
        tPartData.m_iCurSpriteNumber = tPartDesc.m_iCurSpriteNumber;

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

        // =========== 중력 및 외부 힘 설정 (커브 변환 포함) ===========
        tPartData._Effect_Gravity_Value = tPartDesc._Effect_Gravity_Value;
        tPartData._Effect_GravityModifier = tPartDesc._Effect_GravityModifier;
        tPartData._Effect_GravityDir = tPartDesc._Effect_GravityDir;
        tPartData._bUseGlobalGravityCurve = tPartDesc._bUseGlobalGravityCurve;
        tPartData._bUseExternalForceCurve = tPartDesc._bUseExternalForceCurve;
        tPartData.fExternalForceStrength = tPartDesc.fExternalForceStrength;

        // 중력 커브 데이터 변환 복사
        for (const auto& key : tPartDesc._vecGlobalGravityCurve)
            tPartData._vecGlobalGravityCurve.push_back({ key.fTimeKey, key.fValue });
        for (const auto& key : tPartDesc._vecExternalForceCurve)
            tPartData._vecExternalForceCurve.push_back({ key.fTimeKey, key.fValue });

        // =========== 회전 설정 (각 축별 커브 변환 포함) ===========
        tPartData._Effect_StartRotation = tPartDesc._Effect_StartRotation;
        tPartData._Effect_TargetRotation = tPartDesc._Effect_TargetRotation;
        tPartData._bUseStartRotation = tPartDesc._bUseStartRotation;
        tPartData._bUseRotationCurve = tPartDesc._bUseRotationCurve;
        tPartData._bSeparateAxes = tPartDesc._bSeparateAxes;

        // 회전 커브 데이터 변환 복사
        for (const auto& key : tPartDesc._vecRotationCurveX)
            tPartData._vecRotationCurveX.push_back({ key.fTimeKey, key.fValue });
        for (const auto& key : tPartDesc._vecRotationCurveY)
            tPartData._vecRotationCurveY.push_back({ key.fTimeKey, key.fValue });
        for (const auto& key : tPartDesc._vecRotationCurveZ)
            tPartData._vecRotationCurveZ.push_back({ key.fTimeKey, key.fValue });

        //=======  개별 텍스처 스크롤 가중치 복원 =======
        tPartData._Effect_DiffuseTexture_ScrollWeight = tPartDesc._Effect_DiffuseTexture_ScrollWeight;
        tPartData._Effect_NoiseTexture_ScrollWeight = tPartDesc._Effect_NoiseTexture_ScrollWeight;
        tPartData._Effect_MaskingTexture_ScrollWeight = tPartDesc._Effect_MaskingTexture_ScrollWeight;
        tPartData._Effect_GradationTexture_ScrollWeight = tPartDesc._Effect_GradationTexture_ScrollWeight;

        //=======  툴 전용 스크롤 체크박스 상태 복원 =======
        tPartData._Effect_Tool_UseScroll_Diffuse = tPartDesc._Effect_Tool_UseScroll_Diffuse;
        tPartData._Effect_Tool_UseScroll_Noise = tPartDesc._Effect_Tool_UseScroll_Noise;
        tPartData._Effect_Tool_UseScroll_Masking = tPartDesc._Effect_Tool_UseScroll_Masking;
        tPartData._Effect_Tool_UseScroll_Gradation = tPartDesc._Effect_Tool_UseScroll_Gradation;

        // =========== 플래그 및 툴 전용 데이터 ===========
        tPartData._Effect_TextureFlag = tPartDesc._Effect_TextureFlag;
        tPartData._Effect_RenderFlag = tPartDesc._Effect_RenderFlag;
        tPartData._Effect_SamplerStateFlag = tPartDesc._Effect_SamplerStateFlag;
        tPartData._Effect_TextureRotationFlag = tPartDesc._Effect_TextureRotationFlag;
        tPartData._Effect_TextureOperatorFlag = tPartDesc._Effect_TextureOperatorFlag;

        tPartData._Effect_Tool_DiffuseTexture = tPartDesc._Effect_Tool_DiffuseTexture;
        tPartData._Effect_Tool_NoiseTexture = tPartDesc._Effect_Tool_NoiseTexture;
        tPartData._Effect_Tool_MaskingTexture = tPartDesc._Effect_Tool_MaskingTexture;
        tPartData._Effect_Tool_GradationTexture = tPartDesc._Effect_Tool_GradationTexture;
        tPartData._Effect_Tool_UseBillboard = tPartDesc._Effect_Tool_UseBillboard;
        tPartData._Effect_Tool_UseScroll = tPartDesc._Effect_Tool_UseScroll;
        tPartData._Effect_Tool_RightScroll = tPartDesc._Effect_Tool_RightScroll;
        tPartData._Effect_Tool_DownScroll = tPartDesc._Effect_Tool_DownScroll;
        tPartData._Effect_Tool_DiffuseSamplerState_Flag = tPartDesc._Effect_Tool_DiffuseSamplerState_Flag;
        tPartData._Effect_Tool_NoiseSamplerState_Flag = tPartDesc._Effect_Tool_NoiseSamplerState_Flag;
        tPartData._Effect_Tool_MaskingSamplerState_Flag = tPartDesc._Effect_Tool_MaskingSamplerState_Flag;
        tPartData._Effect_Tool_GradationSamplerState_Flag = tPartDesc._Effect_Tool_GradationSamplerState_Flag;

        tContainerData._ChildData.push_back(tPartData);
    }

    // 3. 부모 Document에 최종 컨테이너 추가
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
