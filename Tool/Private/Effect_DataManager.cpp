#include "pch.h"
#include "Effect_DataManager.h"

////////////////
//   Object   //
////////////////
#include "GameObject.h"

////////////////
//   Builder  //
////////////////
#include "BuilderSystem.h"

// Effect
#include "Builder_Effect.h"
#include "DataDocument_Effect.h"
#include "DataStruct_Effect.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffect_DataManager)

CEffect_DataManager::CEffect_DataManager()
{
}



HRESULT CEffect_DataManager::Initialize_EffectDataManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	m_pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);

	if (FAILED(Ready_Builder()))
		return E_FAIL;

	if (FAILED(Ready_BuildFiles()))
		return E_FAIL;


	return S_OK;
}

void CEffect_DataManager::Update(const _float& fTimeDelta)
{

}

void CEffect_DataManager::Render()
{
}

void CEffect_DataManager::Push_EffectTag(const string& Tag)
{
	// 테토남 답게 Tag 중복 검사 패스.
	m_vecEffectTags.push_back(Tag);
}

CGameObject* CEffect_DataManager::Make_EffectPrototype(EEFFECT_DATATYPE DataType, const string& EffectTag)
{
	_uint iHashTag = Engine_Utils::ToHash(EffectTag.c_str());
	auto& pData = m_EffectDescData[iHashTag];

	CGameObject* pBase = nullptr;

	switch (DataType)
	{
	case EEFFECT_DATATYPE::NORMAL:
	{
		pBase =
			static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
				m_pGameInstance->Get_CurrentLevelIndex(),
				L"Prototype_GameObject_Effect",
				&pData));
		break;
	}

	case EEFFECT_DATATYPE::ENVIRONMENT:
	{
		pBase =
			static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
				m_pGameInstance->Get_CurrentLevelIndex(),
				L"Prototype_GameObject_Effect_Env",
				&pData));
		break;
	}
	}

	if (pBase == nullptr)
		return nullptr;

	return pBase;
}

void CEffect_DataManager::Push_ToolEffectData(_uint iHashTag, void* Desc)
{
	Effect::EFFECT_CONTAINERDESC* pDesc = static_cast<Effect::EFFECT_CONTAINERDESC*>(Desc);
	if (pDesc == nullptr)
	{
		MSG_BOX("Desc이 Null입니다 : CEffect_DataManager");
		return;
	}

	auto result = m_EffectDescData.emplace(iHashTag, *pDesc);
}

void* CEffect_DataManager::Find_ToolEffectData(_uint iHashTag)
{
	auto iter = m_EffectDescData.find(iHashTag);

	if (iter == m_EffectDescData.end())
	{
		static Effect::EFFECT_CONTAINERDESC tEmptyDesc = {};
		MSG_BOX("Hash Tag에 맞는 EffectData가 없습니다. : CEffect_DataManager");
		return nullptr;
	}

	return &(iter->second);
}
HRESULT CEffect_DataManager::Ready_Builder()
{
	m_pBuilderSystem = CBuilderSystem::Create();
	if (m_pBuilderSystem == nullptr)  return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::EFFECT, CBuilder_Effect::Create(m_pDevice, m_pDeviceContext, m_pGameInstance->Get_CurrentLevelIndex()))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_DataManager::Ready_BuildFiles()
{
#pragma region EFFECT
	DTO::ECategory eCategory = DTO::ECategory::EFFECT;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>(m_pGameInstance->Get_CurrentLevelIndex(), eCategory)))
		return E_FAIL;

	std::filesystem::path strEffectFolderPath = L"../../Resources/Data/EffectData/VFX_Env/";

	if (std::filesystem::exists(strEffectFolderPath))
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(strEffectFolderPath))
		{
			if (std::filesystem::is_regular_file(entry.path()))
			{
				// 확장자가 .json인 것만 골라내기
				if (entry.path().extension() == ".json")
				{
					if (FAILED(m_pGameInstance->Load_File_Json(m_pGameInstance->Get_CurrentLevelIndex(), eCategory, entry.path())))
						return E_FAIL;

					if (FAILED(m_pBuilderSystem->Build_File(m_pGameInstance->Get_CurrentLevelIndex(), eCategory, entry.path().stem().string())))
						return E_FAIL;

					Push_EffectTag(entry.path().stem().string());
				}
			}
		}
	}
#pragma endregion

	return S_OK;
}

void CEffect_DataManager::Free()
{
	m_vecEffectTags.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);

	m_EffectDescData.clear();


	Super::Free();
}
