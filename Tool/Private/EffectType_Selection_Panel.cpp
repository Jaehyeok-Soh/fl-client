#include "EffectType_Selection_Panel.h"
#include "GameInstance.h"
#include "CEffectObject.h"

CEffectType_Selection_Panel::CEffectType_Selection_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CEffectType_Selection_Panel::Update(const _float fDT)
{

}

void CEffectType_Selection_Panel::DrawEmbedded()
{
	// =============  Effect Particle System, Force Field, Line, Trail ¼±ÅÃÃ¢  ==================
	if (ImGui::Begin("##EFfect type Selection"))
	{
		if (ImGui::Button("Particle System")) { m_eSelectedEffectType = E_EffectSystemType::Particle; }
		ImGui::SameLine();
		if (ImGui::Button("Force Field")) { m_eSelectedEffectType = E_EffectSystemType::ForceField; }
		ImGui::SameLine();
		if (ImGui::Button("Line")) { m_eSelectedEffectType = E_EffectSystemType::Line; }
		ImGui::SameLine();
		if (ImGui::Button("Trail")) { m_eSelectedEffectType = E_EffectSystemType::Trail; }
		ImGui::SameLine();
		
		ImGui::End();
	}

}

HRESULT CEffectType_Selection_Panel::Render(CToolObject* pGo)
{
	DrawEmbedded();

	switch (m_eSelectedEffectType)
	{
	case E_EffectSystemType::Particle:
		CreateParticleEffect(); break;

	case E_EffectSystemType::ForceField:
		CreateForceFieldEffect(); break;

	case E_EffectSystemType::Line:
		CreateLineEffect(); break;

	case E_EffectSystemType::Trail:
		CreateTrailEffect(); break;
	}

	m_eSelectedEffectType = E_EffectSystemType::None;

    return S_OK;
}

void CEffectType_Selection_Panel::CreateParticleEffect()
{
	{
		CGameObject* pResult = { nullptr };

		CEffectObject::Effect_Desc pEffectDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		// =========    Æ®·£½ºÆû   ============
		transformDesc.vPosition = { 0.f, 0.0f, 0.f };
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;
		pEffectDesc.pTransform_Desc = &transformDesc;

		pEffectDesc.wstrLayerTag = L"Effect";
		pEffectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::EFFECT);
		// ========     ÀÌÆåÆ® Å¸ÀÔ   =========
		pEffectDesc.eEffectSystemType = E_EffectSystemType::Particle;
		pEffectDesc.eEffectType = E_EffectType::None;

		// =========   ÀÌÆåÆ® Color Value   ===============
		pEffectDesc._Effect_Color = _float4{ 0.f, 0.f, 0.f, 1.f };


		// ========  ÀÌÆåÆ® Material ¼³Á¤   ===========
		pEffectDesc._Effect_Model_Tag = {};
		pEffectDesc._Effect_Shader_Tag = {};
		pEffectDesc._Effect_DiffuseTexture_Tag = {};
		pEffectDesc._Effect_Mesh_NoiseTexture_Tag = {};
		pEffectDesc._Effect_DiffuseTexture_Tag = {};
		pEffectDesc._Effect_ShaderPass = {};

		pEffectDesc._Effect_bUseAtlas = false;
		pEffectDesc._Effect_TileCount = CEffectObject::_uint2{0, 0};

		// =======   ÀÌÆåÆ® ½ºÅ©·Ñ Value   ===========
		pEffectDesc._Effect_ScrollSpeed = { 0.f, 0.f };
		pEffectDesc._Effect_fameSpeed = {};

		// ========   ÀÌÆåÆ® ¿Ö°î Scale Value   ==========
		pEffectDesc._Effect_DistortionScale = { 0.f, 0.f };

		// ==========   ÀÌÆåÆ® Sacle Value   ==============
		pEffectDesc._Effect_StartScale = { 1.f, 1.f, 1.f };
		pEffectDesc._Effect_EndScale = { 1.f, 1.f, 1.f };


		if ((pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_Effect",
			ENUM_TO_UINT(ELevelType::EFFECT),
			pEffectDesc.wstrLayerTag, &pEffectDesc)))
		{
			MSG_BOX("»ý¼º ½ÇÆÐ : Effect Object - Particle System");
			return;
		}
	}

}

void CEffectType_Selection_Panel::CreateForceFieldEffect()
{

}

void CEffectType_Selection_Panel::CreateLineEffect()
{

}

void CEffectType_Selection_Panel::CreateTrailEffect()
{

}

HRESULT CEffectType_Selection_Panel::EffectPanel_Initialize()
{

	return S_OK;
}

CEffectType_Selection_Panel* CEffectType_Selection_Panel::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEffectType_Selection_Panel* pInstance = new CEffectType_Selection_Panel(pLabel, pOwner, pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : EffectType_Selection_Panel");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->EffectPanel_Initialize()))
		return nullptr;

	return pInstance;
}


void CEffectType_Selection_Panel::Free()
{
    __super::Free();

	Safe_Release(m_pGameInstance);
}