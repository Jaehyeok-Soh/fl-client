#include "pch.h"
#include "Boss_Lianhuo_Body.h"
#include "ComputeShader.h"
#include "Model.h"
#include "EffectHandler.h"
#include "RenderFx.h"
#include "GameInstance.h"

CBoss_Lianhuo_Body::CBoss_Lianhuo_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CBoss_Lianhuo_Body::CBoss_Lianhuo_Body(const CBoss_Lianhuo_Body& rhs)
	: Super(rhs)
{
}

HRESULT CBoss_Lianhuo_Body::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Lianhuo_Body::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Boss_Lianhuo_Body");
	return S_OK;
}

HRESULT CBoss_Lianhuo_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	CModel* pMyModle = Get_Component<CModel>();

	// root motion offset : eunbi's present
	{
		pMyModle->Set_Animtion_MotionOffset_All(0.01f);
	}

	// 피격 Emissive 색상
	if (CRenderFx* pRenderFx = Get_Component<CRenderFx>())
	{
		pRenderFx->Change_EmissiveColor(Vec3{ 1.f, 0.8f, 0.7f });
	}

	return S_OK;
}

void CBoss_Lianhuo_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBoss_Lianhuo_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
	// GhostTrail
	Get_Component<CModel>()->Update_GhostTrail(fTimeDelta);
}

void CBoss_Lianhuo_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CBoss_Lianhuo_Body::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	CModel* pModel = Get_Component<CModel>();
	if (pModel->Has_GhostTrailSnapshots() || pModel->Is_ActiveGhostTrail())
	{
		m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::GHOST_TRAIL, this);
	}
}

HRESULT CBoss_Lianhuo_Body::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Lianhuo_Body::Render_GhostTrail()
{
	Get_Component<CModel>()->Capture_Ghsot(m_pBoneCombineCS, m_matCombinedWorld);
	CShader* pShader = Get_Component<CShader>();
	return Get_Component<CModel>()->Render_GhostTrail(
		pShader,
		m_pBoneMeshCS,
		m_pBoneCombineCS,
		/*  */10);
}

HRESULT CBoss_Lianhuo_Body::Ready_DissolveEffect_Setting()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_SPAWN_START,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
	m_tDissolveDesc.Set_Dissolve_Setting(8.f, 1.f);
	m_tDissolveDesc.Set_Spawn_Setting(1.f, 1.f);
	m_tDissolveDesc.Set_ObjectType(DS::DISSOLVE_OBJECTTYPE::TYPE_BOSS);

	// 스폰 시간 & 디졸브 시간
	m_tDissolveDesc.ShaderData.fDissolveEdgeColor = SimpleMath::Vector3(1.56f, 0.11f, 0.f);
	m_tDissolveDesc.ShaderData.fDissolveEdgeWidth = 0.05f;

	return S_OK;
}

void CBoss_Lianhuo_Body::DissolveStart()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_DISSOLVE_START, DS::BIT_USE_EDGE,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
}


CBoss_Lianhuo_Body* CBoss_Lianhuo_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBoss_Lianhuo_Body* pInsatnce = new CBoss_Lianhuo_Body(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CBoss_Lianhuo_Body::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CBoss_Lianhuo_Body::Clone(void* pArg)
{
	CBoss_Lianhuo_Body* pInstance = new CBoss_Lianhuo_Body(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBoss_Lianhuo_Body::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Lianhuo_Body::Free()
{
	Super::Free();
}