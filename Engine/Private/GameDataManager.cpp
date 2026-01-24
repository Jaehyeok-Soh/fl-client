#include "Engine_pch.h"
#include "GameDataManager.h"
#include "GameInstance.h"

CGameDataManager::CGameDataManager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameDataManager::Initialize()
{
    return S_OK;
}

HRESULT CGameDataManager::Add_Presets(const wstring& wstrFilePath)
{
    if (m_umapPresets.size() > 0)
    {
        MSG_BOX("CGameDataManager::Add_Presets, Already Loaded");
        return S_OK;
    }

    if (wstrFilePath.empty())
        return E_FAIL;

    MAPFILE_DATA mapfileData = {};
    if (FAILED(m_pGameInstance->Load_MapData(wstrFilePath, mapfileData)))
        return E_FAIL;

    const vector<MESHEEFFECT_PRESET_SAVEDATA>& vecPresets = mapfileData.meshEffectPresets;
    const size_t iPresetCount = vecPresets.size();
    if (iPresetCount <= 0)
    {
        MSG_BOX("CGameDataManager::Add_Presets, Loaded data is empty!");
        return E_FAIL;
    }
    
    for (size_t i = 0; i < iPresetCount; ++i)
    {
        if (FAILED(Regist_PresetID(vecPresets[i].strName, vecPresets[i].iPresetID)))
            return E_FAIL;

        Regist_Preset(vecPresets[i].iPresetID, vecPresets[i].snapShot);
    }

    return S_OK;
}

HRESULT CGameDataManager::Add_Previews(const wstring& wstrFilePath)
{
    if (wstrFilePath.empty())
        return E_FAIL;

    MAPFILE_DATA mapfileData = {};
    if (FAILED(m_pGameInstance->Load_MapData(wstrFilePath, mapfileData)))
        return E_FAIL;

    const vector<MAPOBJECT_SAVEDATA>& vecPreviews = mapfileData.objects;
    size_t iPreviewCount = vecPreviews.size();
    if (iPreviewCount <= 0)
        return E_FAIL;

    std::filesystem::path filePath{ wstrFilePath };
    wstring wstrFileName{ L"" };
    wstrFileName = filePath.filename().lexically_normal().stem();
    if (wstrFileName.empty())
        return E_FAIL;

    auto itr = m_umapPreviews.find(wstrFileName);
    if (itr != m_umapPreviews.end())
        return E_FAIL;

    m_umapPreviews.insert(unordered_map<wstring, vector<MAPOBJECT_SAVEDATA>>::value_type(wstrFileName, vecPreviews));
    return S_OK;
}

const EFFECT_PRESET_SNAPSHOT& CGameDataManager::Get_PresetSnapShot(_uint iPresetID)
{
    auto itr = m_umapPresets.find(iPresetID);
    if (itr == m_umapPresets.end())
        return m_umapPresets[0];

    return itr->second;
}

const EFFECT_PRESET_SNAPSHOT &CGameDataManager::Get_PresetSnapShot(const string& strTag)
{
    _int iID{ -1 };
    iID = Get_PresetID(strTag);
    if (iID < 0)
        return m_umapPresets[0];

    auto itr = m_umapPresets.find(iID);
    if (itr == m_umapPresets.end())
        return m_umapPresets[0];

    return itr->second;
}


const MAPOBJECT_SAVEDATA* CGameDataManager::Get_Preview(const wstring& wstrGroupTag, const string& strNameTag)
{
    const vector<MAPOBJECT_SAVEDATA>* pFind = Get_Previews(wstrGroupTag);
    if (pFind == nullptr)
        return nullptr;

    for (const auto& obj : (*pFind))
    {
        if (obj.name == strNameTag)
            return &obj;
    }

    return nullptr;
}

const vector<MAPOBJECT_SAVEDATA> *CGameDataManager::Get_Previews(const wstring& wstrTag)
{
    auto itr = m_umapPreviews.find(wstrTag);
    if (itr == m_umapPreviews.end())
        return nullptr;

    return &itr->second;
}

HRESULT CGameDataManager::Regist_PresetID(const string& strTag, _uint iID)
{
    auto itr = m_umapPresetNameToID.find(strTag);
    if (itr != m_umapPresetNameToID.end())
        return E_FAIL;

    m_umapPresetNameToID.insert(unordered_map<string, _uint>::value_type(strTag, iID));
    return S_OK;
}

void CGameDataManager::Regist_Preset(_uint iID, const EFFECT_PRESET_SNAPSHOT& data)
{
    auto itr = m_umapPresets.find(iID);
    if (itr != m_umapPresets.end())
        return;

    m_umapPresets.insert(unordered_map<_uint, EFFECT_PRESET_SNAPSHOT>::value_type(iID, data));
}

_int CGameDataManager::Get_PresetID(const string& strTag)
{
    auto itr = m_umapPresetNameToID.find(strTag);
    if (itr == m_umapPresetNameToID.end())
        return -1;

    return itr->second;
}

void CGameDataManager::Preset_Clear()
{
    m_umapPresetNameToID.clear();
    m_umapPresets.clear();
}

void CGameDataManager::Preview_Clear()
{
    m_umapPreviews.clear();
}

void CGameDataManager::ModelPartsData_Clear()
{
    m_tModelPartsData = {};
}

CGameDataManager* CGameDataManager::Create()
{
    CGameDataManager* pInstance = new CGameDataManager;
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CGameDataManager::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CGameDataManager::Free()
{
    ModelPartsData_Clear();
    Preset_Clear();
    Preview_Clear();
    Safe_Release(m_pGameInstance);
    Super::Free();
}
