#include "pch.h"
#include "MonsterActionState.h"

// Builder
#include "BuilderSystem.h"
#include "Builder_MonsterState.h"

#include "DataDocument_MonsterState.h"
#include "DataStruct_MonsterState.h"

// State
#include "State_Monster.h"
#include "GameInstance.h"

CMonsterActionState::CMonsterActionState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(),
	m_pDevice(pDevice),
	m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CMonsterActionState::CMonsterActionState(const CMonsterActionState& rhs)
	: Super(rhs)
{
}

HRESULT CMonsterActionState::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterActionState::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	MONSTERACTIONSTATE_DESC* pDesc = static_cast<MONSTERACTIONSTATE_DESC*>(pArg);
	
	if (FAILED(LoadStates(pDesc->wstrMonsterStateTag, pDesc->iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CMonsterActionState::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Update_CooldownTime(fTimeDelta);
}

HRESULT CMonsterActionState::Bind_State(std::set<string> setState)
{
	m_umapState.clear();

	m_vecStates.resize(setState.size());
	m_umapState.reserve(setState.size());
	for (auto iter = setState.begin(); iter != setState.end(); iter++)
		m_umapState.emplace(*iter, std::distance(setState.begin(), iter));

	return S_OK;
}

DTO::MONSTERSTATE_DESC CMonsterActionState::LoadStateFile(std::filesystem::path path, _uint iLevelIndex)
{
	CBuilderSystem* pBuilderSystem = { nullptr };
	pBuilderSystem = CBuilderSystem::Create();
	if (pBuilderSystem == nullptr)
	{
		MSG_BOX("Failed to read : Monster State");
	}

	if (FAILED(pBuilderSystem->Ready_Builder(DTO::ECategory::MONSTER_STATE, CBuilder_MonsterState::Create(m_pDevice, m_pDeviceContext, iLevelIndex))))
	{
		MSG_BOX("Failed to read : Monster State");
	}

	DTO::ECategory eCategory = DTO::ECategory::MONSTER_STATE;
	_uint iLevelID = iLevelIndex;

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_MonsterState>(iLevelID, eCategory)))
	{
		MSG_BOX("Failed to read : Monster State");
	}

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, path)))
	{
		MSG_BOX("Failed to read : Monster State");
	}

	CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, path);
	if (pDocument == nullptr)
	{
		MSG_BOX("Failed to read : Monster State");
	}

	DTO::MONSTERSTATE_DESC result{};

	CDataDocument_MonsterState* pMonsterStateDoc = static_cast<CDataDocument_MonsterState*>(pDocument);

	if (pMonsterStateDoc == nullptr)
		return result;

	result = static_cast<CDataStruct_MonsterState*>(pMonsterStateDoc->Get_AllList()[0])->Get_Data();

	Safe_Release(pBuilderSystem);

	return result;
}

DTO::MONSTER_STATEBASE_DESC* CMonsterActionState::Get_StateDesc(const string& strStateTag, OUT _int& iStateIndex)
{
	auto itr = m_umapState.find(strStateTag.c_str());
	if (itr == m_umapState.end())
	{
		MSG_BOX("CMonsterActionState::Get_StateDesc, invalid state tag");
		return nullptr;
	}
	_uint iIndex = itr->second;
	if (iIndex >= m_tDesc.vecMonsterStateDesc.size())
	{
		MSG_BOX("CMonsterActionState::Get_StateDesc, invalid mapped index");
		return nullptr;
	}

	DTO::MONSTER_STATEBASE_DESC* pReturn{nullptr};

	for (_uint i = 0; i < m_tDesc.vecMonsterStateDesc.size(); ++i)
	{
		if (std::strcmp(m_tDesc.vecMonsterStateDesc[i].strName.c_str(), strStateTag.c_str()) == 0)
		{
			pReturn = &m_tDesc.vecMonsterStateDesc[i];
			break;
		}
	}

	iStateIndex = iIndex;
	return pReturn;
}

HRESULT CMonsterActionState::LoadStates(wstring stateTag, _uint iLevelIndex)
{
	std::filesystem::path path = L"../../Resources/Data/MonsterState";
	path /= stateTag;
	path.replace_extension(".json");

	m_tDesc = LoadStateFile(path, iLevelIndex);

	if (FAILED(Bind_State(m_tDesc.setStates)))
		return E_FAIL;

	for (auto& stateDesc : m_tDesc.vecMonsterStateDesc)
	{
		_uint stateIdx = (*m_umapState.find(stateDesc.strName)).second;

		// StateDesc만 작성후 넘기기 추후에 외부에서 Desc Get해와서 AddState
		if (stateDesc.bIsCustom == true)
			continue;

		if (FAILED(Add_State(stateIdx, CState_Monster::Create(this, stateIdx, &stateDesc))))
			return E_FAIL;
	}

	return S_OK;
}

void CMonsterActionState::Update_CooldownTime(_float fTimeDelta)
{
	for (auto& state : m_vecStates)
	{
		if (state)
			static_cast<CStateBase_Monster*>(state)->Update_CooldownTime(fTimeDelta, false);
	}
}

_int CMonsterActionState::Get_StateIndex(const string& strStateName)
{
	auto itr = m_umapState.find(strStateName);
	if (itr == m_umapState.end())
		return -1;

	return itr->second;
}

_bool CMonsterActionState::IsStateReady(_uint iIdx)
{
	return static_cast<CStateBase_Monster*>(m_vecStates[iIdx])->IsCooldownTimeSatisfy();
}

CMonsterActionState* CMonsterActionState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonsterActionState* pInsatnce = new CMonsterActionState(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CMonsterActionState::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CComponent* CMonsterActionState::Clone(void* pArg)
{
	CMonsterActionState* pClone = new CMonsterActionState(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CMonsterActionState::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CMonsterActionState::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Super::Free();
}
