#include "pch.h"
#include "Boss_Xibi_Body.h"
#include "ComputeShader.h"
#include "Model.h"
#include "EffectHandler.h"
#include "RenderFx.h"
#include "GameInstance.h"

CBoss_Xibi_Body::CBoss_Xibi_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CBoss_Xibi_Body::CBoss_Xibi_Body(const CBoss_Xibi_Body& rhs)
	: Super(rhs)
{
}

HRESULT CBoss_Xibi_Body::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi_Body::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Boss_Xibi_Body");
	return S_OK;
}

HRESULT CBoss_Xibi_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	CModel* pMyModle = Get_Component<CModel>();

	// Mixing Face
	{
		_uint iFaceAnimIdx = Get_Component<CModel>()->Get_AnimationIndex(L"Animation_Xibi_FaceNone");
		vector<CModel::DATA_ANIMIX> vecMix = { { 139, false, 1.f } };
		CComputeShader* pAnimMixCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimMix")));
		pMyModle->Set_MixAnim(true);
		pMyModle->Set_MixAnim_ResetSize(1);
		pMyModle->Make_MixRatio(iFaceAnimIdx, vecMix, pAnimMixCS);
		pMyModle->Set_MixAnim_AnimIndex(0, iFaceAnimIdx);
	}

	// root motion offset : eunbi's present
	{
		pMyModle->Set_Animtion_MotionOffset_All(0.01f);
	}

	// 피격 Emissive 색상
	if (CRenderFx* pRenderFx = Get_Component<CRenderFx>())
	{
		pRenderFx->Change_EmissiveColor(Vec3{0.62f, 0.30f, 1.00f});
	}

	return S_OK;
}

void CBoss_Xibi_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBoss_Xibi_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CBoss_Xibi_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CBoss_Xibi_Body::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CBoss_Xibi_Body::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi_Body::Ready_DissolveEffect_Setting()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_SPAWN_START,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
	m_tDissolveDesc.Set_Dissolve_Setting(10.f, 1.f);
	m_tDissolveDesc.Set_Spawn_Setting(1.f, 1.f);
	m_tDissolveDesc.Set_ObjectType(DS::DISSOLVE_OBJECTTYPE::TYPE_BOSS);

	// 스폰 시간 & 디졸브 시간
	m_tDissolveDesc.ShaderData.fDissolveEdgeColor = SimpleMath::Vector3(1.56f, 0.11f, 0.f);
	m_tDissolveDesc.ShaderData.fDissolveEdgeWidth = 0.05f;

	return S_OK;
}

void CBoss_Xibi_Body::DissolveStart()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_DISSOLVE_START, DS::BIT_USE_EDGE,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
}

CBoss_Xibi_Body* CBoss_Xibi_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBoss_Xibi_Body* pInsatnce = new CBoss_Xibi_Body(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CBoss_Xibi_Body::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CBoss_Xibi_Body::Clone(void* pArg)
{
	CBoss_Xibi_Body* pInstance = new CBoss_Xibi_Body(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBoss_Xibi_Body::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Xibi_Body::Free()
{
	Super::Free();
}