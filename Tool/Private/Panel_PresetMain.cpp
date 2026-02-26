#include "pch.h"
#include "Panel_PresetMain.h"
#include "AttackPreset_DataManager.h"
#include "Level_AttackPreset.h"
#include "GameInstance.h"

CPanel_PresetMain::CPanel_PresetMain(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super("PresetMain##AttackPreset", pOwner, pDevice, pDeviceContext)
	, m_pPresetManager(CAttackPreset_DataManager::GetInstance())
{
	Safe_AddRef(m_pPresetManager);
}

HRESULT CPanel_PresetMain::Initialize()
{
	return S_OK;
}

void CPanel_PresetMain::Update(const _float fTimeDelta)
{
}

HRESULT CPanel_PresetMain::Render(CToolObject* pGo)
{
    static constexpr const _char* s_ColliderTypeItmes[] =
    {
        "Overlap", "PhysicsCollider"
    };
    static constexpr const _char* s_HitTypeItems[] =
    {
        "Additive", "Light", "Heavy", "Launch", "Knockdown", "None"
    };
    static constexpr const _char* s_DamageTypeItems[] =
    {
        "Physical", "Fire", "Electric"
    };
    static constexpr const _char* s_HitPolicyItems[] =
    {
        "Once", "Interval"
    };

    ImGui::Begin("AttackPreset");
    CLevel_AttackPreset* pLevel = static_cast<CLevel_AttackPreset*>(m_pOwnerLevel);

    _uint iKey = pLevel->Get_SelectedKey();
    if (iKey == 0)
    {
        ImGui::TextDisabled("Select a preset from the list.");
        ImGui::End();
        return S_OK;
    }

    auto* pData = m_pPresetManager->Find(iKey);
    if (pData == nullptr)
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid selection.");
        ImGui::End();
        return S_OK;
    }

    // Tag edit (중복 불가)
    static _char tagBuf[MAX_NAME]{};
    static _uint iLastKey = 0;
    if (iLastKey != iKey)
    {
        strncpy_s(tagBuf, pData->strTag.c_str(), sizeof(tagBuf));
        tagBuf[sizeof(tagBuf) - 1] = '\0';
        iLastKey = iKey;
    }

    ImGui::Text("PresetKey: 0x%08X", pData->iPresetKey);

    ImGui::Text("Category: %d", (_int)pData->eCategory);
    ImGui::Text("LocalId : %u", (_uint)pData->iID);

    ImGui::InputText("Tag", tagBuf, IM_ARRAYSIZE(tagBuf));
    _bool bTagOk = m_pPresetManager->Is_TagUnique(tagBuf, iKey);
    if (bTagOk == false)
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Tag duplicated!");

    if (ImGui::Button("Apply Tag") && bTagOk && tagBuf[0] != '\0')
    {
        pData->strTag = tagBuf;
        m_pPresetManager->Upsert_Data(*pData);
    }

    ImGui::SeparatorText("ColliderType");
    {
        _int iColliderType = static_cast<_int>(pData->eColliderType);
        if (ImGui::Combo("ColliderType##Combo", &iColliderType, s_ColliderTypeItmes, static_cast<_int>(DTO::g_AttackPresetColliderTypeCount)))
            pData->eColliderType = static_cast<DTO::EAttackPresetColliderType>(iColliderType);
    }

    ImGui::SeparatorText("Combat");

    {
        _int iHitTypeIndex = static_cast<_int>(pData->tCombat.eHitType);
        if (ImGui::Combo("HitType", &iHitTypeIndex, s_HitTypeItems, static_cast<_int>(DTO::g_HitTypeCount)))
            pData->tCombat.eHitType = static_cast<DTO::EHitType>(iHitTypeIndex);

        _int iDamageTypeIndex = static_cast<_int>(pData->tCombat.eDamageType);
        if (ImGui::Combo("DamageType", &iDamageTypeIndex, s_DamageTypeItems, static_cast<_int>(DTO::g_DamageTypeCount)))
            pData->tCombat.eDamageType = static_cast<DTO::EDamageType>(iDamageTypeIndex);
    }

    ImGui::DragFloat("BaseDamage", &pData->tCombat.fBaseDamage, 0.1f, 0.f, 999999.f);
    ImGui::DragFloat("HitStunSec", &pData->tCombat.fHitStunSec, 0.01f, 0.f, 10.f);
    ImGui::DragFloat("HitStopSec", &pData->tCombat.fHitStopSec, 0.01f, 0.f, 1.f);
    ImGui::DragFloat("Impulse", &pData->tCombat.fImpulse, 1.0f, 0.0f, 2000.0f, "%.1f");
    ImGui::DragFloat("PushPower", &pData->tCombat.fPushPower, 0.1f, 0.0f, 200.0f, "%.2f");

    ImGui::SeparatorText("HitPolicy");

    {
        _int iHitPolicyTypeIndex = static_cast<_int>(pData->tPolicy.ePolicyType);
        if (ImGui::Combo("HitPolicyType", &iHitPolicyTypeIndex, s_HitPolicyItems, static_cast<_int>(DTO::g_HitPolicyTypeCount)))
            pData->tPolicy.ePolicyType = static_cast<DTO::EHitPolicyType>(iHitPolicyTypeIndex);
    }

    if (pData->tPolicy.ePolicyType == DTO::EHitPolicyType::Interval)
        ImGui::DragFloat("IntervalSec", &pData->tPolicy.fIntervalSec, 0.01f, 0.f, 10.f);
    else
        ImGui::TextDisabled("IntervalSec (Only for Interval)");

    ImGui::End();
	return S_OK;
}

CPanel_PresetMain* CPanel_PresetMain::Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_PresetMain* pInstance = new CPanel_PresetMain(pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CPanel_PresetMain::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPanel_PresetMain::Free()
{
	Safe_Release(m_pPresetManager);
	Super::Free();
}