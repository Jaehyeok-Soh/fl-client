#include "pch.h"
#include "Effect.h"
#include "DataStruct_Effect.h"
#include "DataDocument_Effect.h"
#include "EffectObject.h"
#include "Engine_Utils.h"

#define MAX_EFFECTPART 10

Effect::Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CContainerObject(pDevice, pDeviceContext)
{
	m_vecPartObjects.resize(MAX_EFFECTPART, nullptr);
}

Effect::Effect(const Effect& rhs)
	:CContainerObject(rhs)
{
	m_vecPartObjects.resize(MAX_EFFECTPART, nullptr);
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

	//if (FAILED(Ready_PartsData(pArg)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT Effect::Ready_PartsData(void* pArg)
{
	EFFECT_CONTAINERDESC* pDesc = static_cast<EFFECT_CONTAINERDESC*>(pArg);
	if (pDesc == nullptr) return E_FAIL;

	m_eSimulationSpace = pDesc->_Effect_SimulationType;
	auto ChildDataList = pDesc->_childData;

	// =============== CREATE   PARTS ===================
	_uint index = 0;
	for (DTO::TEFFECT_PartsData& Part : ChildDataList)
	{
		// 1. Transform 데이터 복원 (Matrix -> Vec3)
		Vec3 vScale, vPos;
		Quat vQuat;
		Part.vWorldMatrix.Decompose(vScale, vQuat, vPos);

		CEffectObject::Effect_Desc pEffectDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};

		transformDesc.ScaleMatrix = Matrix::CreateScale(Vec3(vScale));
		transformDesc.RotationMatrix = Matrix::CreateFromQuaternion(vQuat);
		transformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(vPos));
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;

		//===========    부모 행렬 연결 및 기본 설정   ==============
		pEffectDesc.pMatParent = &(Get_Component<CTransform>()->Get_WorldMatrix());
		pEffectDesc.pTransform_Desc = &transformDesc;
		pEffectDesc.iLevelIndex = pDesc->iLevelIndex;

		// ==========   이펙트 타입 및 시스템 설정 ==============
		pEffectDesc.eEffectSystemType = (DTO::E_EffectSystemType)Part.eEffectSystemType;
		pEffectDesc.eEffectParticleType = (DTO::E_PARTICLETYPE)Part.eEffectParticleType;
		pEffectDesc.eEffectType = (DTO::E_EFFECTTYPE)Part.eEffectType;
		pEffectDesc._Effect_ShapeType = (DTO::E_SHAPETYPE)Part._Effect_ShapeType;

		// ==========    리소스 태그 복원  ===============
		pEffectDesc._Effect_Model_Tag = Part._Effect_Model_Tag;
		pEffectDesc._Effect_DiffuseTexture_Tag = Part._Effect_DiffuseTexture_Tag;
		pEffectDesc._Effect_NoiseTexture_Tag = Part._Effect_NoiseTexture_Tag;
		pEffectDesc._Effect_MaskingTexture_Tag = Part._Effect_MaskingTexture_Tag;
		pEffectDesc._Effect_GradationTexture_Tag = Part._Effect_GradationTexture_Tag;
		pEffectDesc._Effect_TrailTexture_Tag = Part._Effect_TrailTexture_Tag;
		pEffectDesc._Effect_NormalTexture_Tag = Part._Effect_NormalTexture_Tag;

		// =====    셰이더 설정   =====================
		pEffectDesc._Effect_Shader_Tag = Part._Effect_Shader_Tag;
		pEffectDesc._Effect_ShaderPass = Part._Effect_ShaderPass;

		// ======   수치 및 컬러 데이터 복원   ===========
		pEffectDesc._Effect_ScrollSpeed = Part._Effect_ScrollSpeed;
		pEffectDesc._Effect_DistortionScale = Part._Effect_DistortionScale;
		pEffectDesc._Effect_StartScale = Part._Effect_StartScale;
		pEffectDesc._Effect_EndScale = Part._Effect_EndScale;
		pEffectDesc._Effect_Color = Part._Effect_Color;
		pEffectDesc._Effect_DiscardValue = Part._Effect_DiscardValue;
		pEffectDesc._Effect_Range = Part._Effect_Range;
		pEffectDesc._Effect_ParticleSize = Part._Effect_ParticleSize;

		// =====   스프라이트 및 애니메이션 설정   ========
		pEffectDesc._Effect_bUseSprite = Part._Effect_bUseSprite;
		pEffectDesc._Effect_TileCount = { Part._Effect_TileCount.x, Part._Effect_TileCount.y };
		pEffectDesc._Effect_bPlayAnim = Part._Effect_bPlayAnim;
		pEffectDesc._Effect_AnimSpeed = Part._Effect_AnimSpeed;
		pEffectDesc.m_iCurSpriteNumber = Part.m_iCurSpriteNumber;

		//=======   파티클 시스템 상세 설정   ========
		pEffectDesc._Effect_Duration = Part._Effect_Duration;
		pEffectDesc._Effect_Looping = Part._Effect_Looping;
		pEffectDesc._Effect_IsRandomSeed = Part._Effect_IsRandomSeed;
		pEffectDesc._Effect_StartDelay = Part._Effect_StartDelay;
		pEffectDesc._Effect_LifeTime = Part._Effect_LifeTime;
		pEffectDesc._Effect_PlayBackSpeed = Part._Effect_PlayBackSpeed;
		pEffectDesc._Effect_StartSpeed = Part._Effect_StartSpeed;
		pEffectDesc._Effect_MaxParticle = Part._Effect_MaxParticle;
		pEffectDesc._Effect_RateOverTime = Part._Effect_RateOverTime;
		pEffectDesc._Effect_RateOverDistance = Part._Effect_RateOverDistance;

		// ===========  및 렌더링 플래그   =============
		pEffectDesc._Effect_TextureFlag = Part._Effect_TextureFlag;
		pEffectDesc._Effect_RenderFlag = Part._Effect_RenderFlag;
		pEffectDesc._Effect_SamplerStateFlag = Part._Effect_SamplerStateFlag;

		Add_Part(index, pDesc->iLevelIndex, L"Prototype_GameObject_Effect_Parts", &pEffectDesc);
		Get_Part<CEffectObject>(index)->Set_Name(Part.EffectPartsName);
		index++;
	}

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

	if (m_eSimulationSpace == DTO::E_SIMULATION_SPACE::LOCAL && m_pParentsWorldMatrix != nullptr)
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

void Effect::Update_CombinedWorldMatrix(const Matrix* pMatParent)
{
	m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix() * (*pMatParent);
}


void Effect::Set_Dead(const wstring& wstrLayerTag)
{
}

HRESULT Effect::Spawn_FromPool(void* pArg)
{


	return S_OK;
}

HRESULT Effect::Despawn_FromPool()
{


	return S_OK;
}

Effect* Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	Effect* pInstance = new Effect(pDevice, pDeviceContext);

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
