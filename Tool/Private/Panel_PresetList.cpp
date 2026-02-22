#include "pch.h"
#include "Panel_PresetList.h"
#include "Builder_AttackPreset.h"
#include "AttackPreset_DataManager.h"
#include "Level_AttackPreset.h"
#include "DataDocument_AttackPreset.h"
#include "GameInstance.h"

CPanel_PresetList::CPanel_PresetList(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super("PresetList##AttackPreset", pOwner, pDevice, pDeviceContext)
    , m_pPresetManager(CAttackPreset_DataManager::GetInstance())
{
    Safe_AddRef(m_pPresetManager);
}

HRESULT CPanel_PresetList::Initialize()
{
    ELevelType eLevelType = ELevelType::ATTACK_PRESET;
    DTO::ECategory eCategory = DTO::ECategory::ATTACK_PRESET;
    _uint iLevelID = ENUM_TO_UINT(eLevelType);

    m_pBuilder = CBuilder_AttackPreset::Create(m_pDevice, m_pDeviceContext, iLevelID);
    if (m_pBuilder == nullptr)
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->Regist_Document<CDataDocument_AttackPreset>(iLevelID, eCategory)))
        return E_FAIL;

	return S_OK;
}

const _char* CPanel_PresetList::To_CategoryFolder(DTO::EAttackPresetCategory eCategory)
{
    switch (eCategory)
    {
        case DTO::EAttackPresetCategory::PlayerBasic:           return "PlayerBasic";
        case DTO::EAttackPresetCategory::PlayerSkill:           return "PlayerSkill";
        case DTO::EAttackPresetCategory::PlayerProjectile:      return "PlayerProjectile";
        case DTO::EAttackPresetCategory::MonsterBasic:          return "MonsterBasic";
        case DTO::EAttackPresetCategory::MonsterSkill:          return "MonsterSkill";
        case DTO::EAttackPresetCategory::MonsterPorjectile:     return "MonsterPorjectile";
        case DTO::EAttackPresetCategory::BossBasic:             return "BossBasic";
        case DTO::EAttackPresetCategory::BossSkill:             return "BossSkill";
        case DTO::EAttackPresetCategory::BossProjectile:        return "BossProjectile";
        default:                                                return "Unknown";
    }
}

DTO::EAttackPresetCategory CPanel_PresetList::Mapping_Category(_int iValue)
{
    switch (iValue)
    {
    case 1:
        return DTO::EAttackPresetCategory::PlayerBasic;
    case 2:
        return DTO::EAttackPresetCategory::PlayerSkill;
    case 3:
        return DTO::EAttackPresetCategory::PlayerProjectile;
    case 4:
        return DTO::EAttackPresetCategory::MonsterBasic;
    case 5:
        return DTO::EAttackPresetCategory::MonsterSkill;
    case 6:
        return DTO::EAttackPresetCategory::MonsterPorjectile;
    case 7:
        return DTO::EAttackPresetCategory::BossBasic;
    case 8:
        return DTO::EAttackPresetCategory::BossSkill;
    case 9:
        return DTO::EAttackPresetCategory::BossProjectile;
    default:
        return DTO::EAttackPresetCategory::END;
    }
}

std::string CPanel_PresetList::Make_FileName(_ushort iID, const string& strTag)
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << iID << "__" << strTag << ".json";
    return oss.str();
}

HRESULT CPanel_PresetList::All_Load()
{
    ELevelType eLevelType = ELevelType::ATTACK_PRESET;
    DTO::ECategory eCategory = DTO::ECategory::ATTACK_PRESET;
    _uint iLevelID = ENUM_TO_UINT(eLevelType);

    if (m_pBuilder == nullptr)
        return E_FAIL;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (pGameInstance == nullptr)
        return E_FAIL;

    std::filesystem::path rootFolder{ g_wszAttackPresetDataPath };

    if (std::filesystem::exists(rootFolder) == false)
    {
        MSG_BOX("CPanel_PresetList::All_Load, RootFolder exist failed");
        return E_FAIL;
    }

    for (const auto& dirEntry : std::filesystem::directory_iterator(rootFolder))
    {
        if (dirEntry.is_directory() == false)
            continue;

        const std::filesystem::path categoryFolder = dirEntry.path();
        for (const auto& fileEntry : std::filesystem::directory_iterator(categoryFolder))
        {
            if (fileEntry.is_regular_file() == false)
                continue;

            const std::filesystem::path filePath = fileEntry.path();
            if (filePath.extension() != ".json")
                continue;

            string strFileKey = filePath.filename().stem().string();
            if (FAILED(pGameInstance->Load_File_Json(iLevelID, eCategory, filePath)))
            {
                MSG_BOX("CPanel_PresetList::All_Load, Load_File_Json failed");
                return E_FAIL;
            }

            const CDataDocumentBase* pBaseDoc = pGameInstance->Get_Document(iLevelID, eCategory, strFileKey);
            if (pBaseDoc == nullptr)
            {
                MSG_BOX("CPanel_PresetList::All_Load, BaseDocument was nullptr");
                return E_FAIL;
            }

            if (FAILED((m_pBuilder->Build(*pBaseDoc))))
            {
                MSG_BOX("CPanel_PresetList::All_Load, Build failed");
                return E_FAIL;
            }
        }
    }
    return S_OK;
}

HRESULT CPanel_PresetList::Save_One(const DTO::TAttackPreset_Data& inData)
{
    ELevelType eLevelType = ELevelType::ATTACK_PRESET;
    DTO::ECategory eCategory = DTO::ECategory::ATTACK_PRESET;
    _uint iLevelID = ENUM_TO_UINT(eLevelType);
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (pGameInstance == nullptr)
        return E_FAIL;

    std::filesystem::path rootFolder{ g_wszAttackPresetDataPath };

    DTO::TAttackPreset_Data data = inData;
    data.Make_Key();

    if (data.strTag.empty() == true)
    {
        MSG_BOX("CPanel_PresetList::Save_One, tag is empty");
        return E_FAIL;
    }

    const std::filesystem::path categoryFolder = rootFolder / To_CategoryFolder(data.eCategory);
    if (std::filesystem::exists(categoryFolder) == false)
        std::filesystem::create_directories(categoryFolder);

    const std::filesystem::path filePath = categoryFolder / Make_FileName(data.iID, data.strTag);

    CDataDocument_AttackPreset* pDocument = static_cast<CDataDocument_AttackPreset*>(pGameInstance->Ensure_Document(iLevelID, eCategory, filePath));
    if (pDocument == nullptr)
        return E_FAIL;

    if (FAILED(pDocument->Try_Add(data)))
        return E_FAIL;

    if (FAILED(pGameInstance->Save_File_Json(iLevelID, eCategory, filePath)))
    {
        MSG_BOX("CPanel_PresetList::Save_One, save_file_json failed");
        return E_FAIL;
    }

    return S_OK;
}

void CPanel_PresetList::Update(const _float fTimeDelta)
{
}

HRESULT CPanel_PresetList::Render(CToolObject* pGo)
{
    ImGui::Begin("AttackPreset List");
    CLevel_AttackPreset* pLevel = static_cast<CLevel_AttackPreset*>(m_pOwnerLevel);
    _uint iSelected = pLevel->Get_SelectedKey();
    static _char s_Search[MAX_NAME]{};
    // All
    static _int  s_iCategoryFilter = -1;

    ImGui::InputText("Search (Tag)", s_Search, IM_ARRAYSIZE(s_Search));

    const _char* categories[] =
    {
        "All",
        "PlayerBasic", "PlayerSkill", "PlayerProjectile",
        "MonsterBasic", "MonsterSkill", "MonsterProjectile",
        "BossBasic", "BossSkill", "BossProjectile"
    };

    ImGui::SeparatorText("File");

    ///////////////////
    /// Load Button ///
    ///////////////////
    {
        if (ImGui::Button("All load"))
        {
            m_pPresetManager->Clear();

            All_Load();
        }
    }

    ImGui::SameLine();

    ///////////////////
    /// Save Button ///
    ///////////////////
    {
        const _bool bCanSave = (iSelected != 0) && (m_pPresetManager->Find(iSelected) != nullptr);
        if (bCanSave == false)
            ImGui::BeginDisabled();
        if (ImGui::Button("Selected Save"))
        {
            const auto* pData = m_pPresetManager->Find(iSelected);
            Save_One(*pData);
        }
        if (bCanSave == false)
            ImGui::EndDisabled();
    }

    ImGui::Combo("Category", &s_iCategoryFilter, categories, IM_ARRAYSIZE(categories));

    ImGui::Separator();

    static _char s_NewTag[MAX_NAME]{};
    // 위 categories에서 1부터가 실제 category라고 가정
    static _int  s_iNewCategory = 0;

    if (ImGui::Button("New"))
        ImGui::OpenPopup("NewPresetPopup");

    if (ImGui::BeginPopupModal("NewPresetPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Tag (Unique)", s_NewTag, IM_ARRAYSIZE(s_NewTag));
        ImGui::Combo("Category##new", &s_iNewCategory, categories, IM_ARRAYSIZE(categories));

        _bool bTagOk = m_pPresetManager->Is_TagUnique(s_NewTag);
        if (bTagOk == false)
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Tag is duplicated!");

        if (ImGui::Button("Create") && bTagOk && s_iNewCategory > 0)
        {
            // DTO::EAttackPresetCategory 매핑
            DTO::EAttackPresetCategory eCategory = Mapping_Category(s_iNewCategory);

            _uint iNewKey = 0;
            if (SUCCEEDED(m_pPresetManager->Create_Data(eCategory, s_NewTag, iNewKey)))
            {
                pLevel->Set_SelectedKey(iNewKey);
                s_NewTag[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            s_NewTag[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();

    //////////
    ///List///
    //////////
    ImGui::BeginChild("PresetListChild", ImVec2(0, 0), true);
    for (const auto& [key, data] : m_pPresetManager->Get_AllDatas())
    {
        if (s_iCategoryFilter > 0)
        {
            const DTO::EAttackPresetCategory eSelectedCategory = Mapping_Category(s_iCategoryFilter);
            if (data.eCategory != eSelectedCategory)
                continue;
        }
        if (s_Search[0] != '\0')
        {
            if (data.strTag.find(s_Search) == std::string::npos)
                continue;
        }

        const bool isSelected = (key == iSelected);
        if (ImGui::Selectable(data.strTag.c_str(), isSelected))
            pLevel->Set_SelectedKey(key);

        // 우측에 key 표시(디버그)
        ImGui::SameLine(260);
        ImGui::TextDisabled("0x%08X", key);
    }
    ImGui::EndChild();

    // Delete
    // 메모리만 제거
    if (iSelected != 0)
    {
        ImGui::Separator();
        if (ImGui::Button("Delete Selected"))
        {
            m_pPresetManager->Remove(iSelected);
            pLevel->Set_SelectedKey(0);
        }
    }

    ImGui::End();
    return S_OK;
}

CPanel_PresetList* CPanel_PresetList::Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_PresetList* pInstance = new CPanel_PresetList(pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CPanel_PresetList::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPanel_PresetList::Free()
{
    Safe_Release(m_pBuilder);
    Safe_Release(m_pPresetManager);
	Super::Free();
}
