#include "pch.h"
#include "Fog.h"
#include "Effect.h"
#include "GameInstance.h"

CFog::CFog(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext)
	: CEnvObject(pDeivce,pContext)
{
}

CFog::CFog(const CFog& rhs)
	: CEnvObject(rhs)
{
}

HRESULT CFog::Initialize_Prototype()
{

	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	/* Effect 미리 생성 */
	Effect* pEffect{ nullptr };
	void* pDesc = m_pGameInstance->Find_EffectData(Engine_Utils::ToHash("ENV_Fog1"));
	if (pDesc == nullptr) return E_FAIL;
	pEffect = static_cast<Effect*>(m_pGameInstance->Clone_Prototype(
			EPrototypeType::GAMEOBJECT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Effect" , pDesc));
	
	/* Effect */
	m_vecEffect.push_back(pEffect);

	CTransform* pTs = Get_Component<CTransform>();

	EFFECT_SPAWN_DESC tSpawnDesc{};
	tSpawnDesc.matWorld = {pTs->Get_WorldMatrix()};
	tSpawnDesc.iSimulationType = (int)tagEffectSpawnDesc::E_VFX_SIMULTYPE::VFX_WORLD;
	pEffect->Enable_VFX(&tSpawnDesc);


	return S_OK;
}

HRESULT CFog::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFog::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CFog::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);

}

void CFog::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CFog::Update_Late(const _float fTimeelta)
{
	Super::Update(fTimeelta);

}

void CFog::Ready_Before_Render(const _float fTimeDelta)
{

}

HRESULT CFog::Render()
{


	return S_OK;
}

CFog* CFog::Create(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext)
{
	CFog* pFog = new CFog(pDeivce,pContext);

	if (FAILED(pFog->Initialize_Prototype()))
	{
		Safe_Release(pFog);
		MSG_BOX("Fog is Failed To Create");
		return pFog;
	}

	return pFog;
}

CGameObject* CFog::Clone(void* pArg)
{
	CFog* pFog = new CFog(*this);

	if (FAILED(pFog->Initialize(pArg)))
	{
		Safe_Release(pFog);
		MSG_BOX("Fog is Failed To Clone");
		return pFog;
	}

	return pFog;
}

void CFog::Free()
{
	Super::Free();
}

