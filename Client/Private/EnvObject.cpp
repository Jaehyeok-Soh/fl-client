#include "pch.h"
#include "EnvObject.h"
#include "Effect.h"
#include "GameInstance.h"

CEnvObject::CEnvObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext), m_vecEffect{}
{
}

CEnvObject::CEnvObject(const CEnvObject& rhs)
	: CMapObject(rhs), m_vecEffect{rhs.m_vecEffect}
{
	for (auto& Effect : m_vecEffect)
		Safe_AddRef(Effect);
}

HRESULT CEnvObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CEnvObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;


	ENVOBJECT_DESC* pDesc = static_cast<ENVOBJECT_DESC*>(pArg);

	if (FAILED(Ready_Effect(pDesc->vecEnvEffectInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvObject::Ready_Effect(const vector<ENV_EFFECT_INFO>& vecEnvEffectInfo)
{
	if (vecEnvEffectInfo.empty()) return E_FAIL;


	CTransform* pTs = Get_Component<CTransform>();
	if (pTs == nullptr) return E_FAIL;

	Effect* pEffect{ nullptr };
	for (auto& Info : vecEnvEffectInfo)
	{
		_uint iHash = Engine_Utils::ToHash(Info.strTags.c_str());
		void* pDesc = m_pGameInstance->Find_EffectData(iHash);
		if (pDesc == nullptr) return E_FAIL;
		pEffect = static_cast<Effect*>(m_pGameInstance->Clone_Prototype(
			EPrototypeType::GAMEOBJECT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Effect", pDesc));
		if (pEffect == nullptr) return E_FAIL;

		EFFECT_SPAWN_DESC tSpawnDesc{};
		tSpawnDesc.matWorld = { pTs->Get_WorldMatrix() };
		tSpawnDesc.iSimulationType = (int)tagEffectSpawnDesc::E_VFX_SIMULTYPE::VFX_WORLD;


		Matrix EffectLocalMatrix = Matrix::CreateScale(Info.tDesc.VFX_Scale) * Matrix::CreateFromYawPitchRoll(Info.tDesc.VFX_Rotation.y, Info.tDesc.VFX_Rotation.x, Info.tDesc.VFX_Rotation.z)
									* Matrix::CreateTranslation(Info.tDesc.VFX_Target_Position);;

		tSpawnDesc.matWorld = EffectLocalMatrix * pTs->Get_WorldMatrix();

		pEffect->Enable_VFX(&tSpawnDesc);
		
		
		m_vecEffect.push_back(pEffect);
	}

	return S_OK;
}


HRESULT CEnvObject::Awake(const _uint iCurrentLevelID)
{
	if(FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;


	for (auto& Effect : m_vecEffect)
		Effect->Awake(iCurrentLevelID);

	return S_OK;
}

void CEnvObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);


	for (auto& Effect : m_vecEffect)
		Effect->Update_Priority(fTimeDelta);
}

void CEnvObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	for (auto& Effect : m_vecEffect)
		Effect->Update(fTimeDelta);
}

void CEnvObject::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

	for (auto& Effect : m_vecEffect)
		Effect->Update_Late(fTimeelta);
}

void CEnvObject::Ready_Before_Render(const _float fTimeDelta)
{
	for (auto& Effect : m_vecEffect)
		Effect->Ready_Before_Render(fTimeDelta);
	return;
}

HRESULT CEnvObject::Render()
{
	for (auto& Effect : m_vecEffect)
		Effect->Render();

	return S_OK;
}

CEnvObject* CEnvObject::Create(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext)
{
	CEnvObject* pEnvObject = new CEnvObject(pDeivce,pContext);

	if (FAILED(pEnvObject->Initialize_Prototype()))
	{
		Safe_Release(pEnvObject);
		MSG_BOX("EnvObject is Failed To Create");
		return pEnvObject;
	}

	return pEnvObject;
}

CGameObject* CEnvObject::Clone(void* pArg)
{
	CEnvObject* pEnvObject = new CEnvObject(*this);

	if (FAILED(pEnvObject->Initialize(pArg)))
	{
		Safe_Release(pEnvObject);
		MSG_BOX("EnvObject is Failed To Clone");
		return pEnvObject;
	}

	return pEnvObject;
}



void CEnvObject::Free()
{
	Super::Free();

	/* EnvObject::Free() */
	for (auto& Effect : m_vecEffect)
		Safe_Release(Effect);
}

