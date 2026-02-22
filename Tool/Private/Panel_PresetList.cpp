#include "pch.h"
#include "Panel_PresetList.h"
#include "AttackPreset_DataManager.h"
#include "Level_AttackPreset.h"
#include "GameInstance.h"

CPanel_PresetList::CPanel_PresetList(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super("PresetList##AttackPreset", pOwner, pDevice, pDeviceContext)
    , m_pPresetManager(CAttackPreset_DataManager::GetInstance())
{
    Safe_AddRef(m_pPresetManager);
}

HRESULT CPanel_PresetList::Initialize()
{
	return S_OK;
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

void CPanel_PresetList::Update(const _float fTimeDelta)
{
}

HRESULT CPanel_PresetList::Render(CToolObject* pGo)
{
    ImGui::Begin("AttackPreset List");
    CLevel_AttackPreset* pLevel = static_cast<CLevel_AttackPreset*>(m_pOwnerLevel);

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
    _uint iSelected = pLevel->Get_SelectedKey();

    ImGui::BeginChild("PresetListChild", ImVec2(0, 0), true);
    for (const auto& [key, data] : m_pPresetManager->Get_AllDatas())
    {
        // filter
        if (s_iCategoryFilter > 0)
        {
            // TODO: category mapping
            // if(data.eCategory != selectedCat) continue;
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
    Safe_Release(m_pPresetManager);
	Super::Free();
}
