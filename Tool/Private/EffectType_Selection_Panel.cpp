#include "pch.h"
#include "EffectType_Selection_Panel.h"
#include "CEffectObject.h"
#include "Effect.h"
#include "GameInstance.h"

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
	// =============  Effect Particle System, Force Field, Line, Trail 선택창  ==================

	if (ImGui::Button("Particle System")) { m_eSelectedEffectType = E_EffectSystemType::Particle; }
	ImGui::SameLine();
	if (ImGui::Button("Force Field")) { m_eSelectedEffectType = E_EffectSystemType::ForceField; }
	ImGui::SameLine();
	if (ImGui::Button("Line")) { m_eSelectedEffectType = E_EffectSystemType::Line; }
	ImGui::SameLine();
	if (ImGui::Button("Trail")) { m_eSelectedEffectType = E_EffectSystemType::Trail; }
	ImGui::SameLine();
		

}

void CEffectType_Selection_Panel::DrawEffectList()
{
	// =============  Effect Layer & Effect List  =====================
	ImGui::SeparatorText("Container_EffectList");

	if (ImGui::TreeNode("Container_EffectList##Effect_List"))
	{
		m_pEffectList.clear();
		auto EffectList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::EFFECT), L"Effect");

		if (EffectList == nullptr)
		{
			ImGui::TreePop();
			return;
		}


		for (auto*& pEffect : *EffectList)
		{
			if (dynamic_cast<CToolObject*>(pEffect))
				m_pEffectList.push_back(make_pair(dynamic_cast<CToolObject*>(pEffect)->Get_Name(), static_cast<CToolObject*>(pEffect)));
		}

		if (m_pEffectList.size() != 0)
		{
			std::vector<const char*> iTems;
			iTems.reserve(static_cast<int>(m_pEffectList.size()));

			for (auto& str : m_pEffectList)
				iTems.push_back(str.first.c_str());

			static int EffectNumber = {};

			ImGui::ListBox("EffectList", &EffectNumber, iTems.data(), static_cast<int>(m_pEffectList.size()),6);

			if ((iTems.size() - 1) < EffectNumber)
			{
				ImGui::TreePop();
				return;
			}

			// 여기서 선택한 Effect를 Target에 넣어준다.
			if (m_pEffectList.size() > EffectNumber && m_pEffectList[EffectNumber].second != nullptr)
				*m_ppTargetSlot = m_pEffectList[EffectNumber].second;
		}

		ImGui::TreePop();
	}
}

HRESULT CEffectType_Selection_Panel::Render(CToolObject* pGo)
{
	if (ImGui::Begin("##EFfect type Selection"))
	{
		DrawEmbedded();
		DrawEffectList();

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

		ImGui::End();
	}
	return S_OK;
}

void CEffectType_Selection_Panel::CreateParticleEffect()
{
	CToolObject* pEffectContainer = { nullptr };

	// =============== CREATE	EFFECT ==================
	{
		Effect::TOOLOBJECT_DESC ToolDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		// =========    트랜스폼   ============
		transformDesc.vPosition = { 0.f, 0.0f, 0.f };
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;
		ToolDesc.pTransform_Desc = &transformDesc;

		ToolDesc.wstrLayerTag = L"Effect";

		if (!(pEffectContainer = static_cast<CToolObject*>(m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::EFFECT),
			L"Prototype_GameObject_Effect",
			ENUM_TO_UINT(ELevelType::EFFECT),
			ToolDesc.wstrLayerTag, &ToolDesc))))
		{
			MSG_BOX("생성 실패 : Effect Object - Particle System");
			return;
		}

		pEffectContainer->Set_Name(TypeToString(m_eSelectedEffectType));
	}

	// ===============  DEFAULT Parts Object 1개는 넣어준다  ==================
	{
		CGameObject* pResult = { nullptr };

		CEffectObject::Effect_Desc pEffectDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		// =========    트랜스폼   ============
		transformDesc.vPosition = { 0.f, 0.0f, 0.f };
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;

		pEffectDesc.pMatParent = &(pEffectContainer->Get_Component<CTransform>()->Get_WorldMatrix());
		pEffectDesc.pTransform_Desc = &transformDesc;

		pEffectDesc.wstrLayerTag = L"Effect_Parts";
		pEffectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::EFFECT);
		// ========     이펙트 타입   =========
		pEffectDesc.eEffectSystemType = E_EffectSystemType::Particle;
		pEffectDesc.eEffectType = E_EFFECTTYPE::NONE;

		// =========   이펙트 Color Value   ===============
		pEffectDesc._Effect_Color = Vec4{ 0.f, 0.f, 0.f, 1.f };


		// ========  이펙트 Material 설정   ===========
		pEffectDesc._Effect_Model_Tag = {};
		pEffectDesc._Effect_Shader_Tag = {};
		pEffectDesc._Effect_DiffuseTexture_Tag = {};
		pEffectDesc._Effect_Mesh_NoiseTexture_Tag = {};
		pEffectDesc._Effect_DiffuseTexture_Tag = {};
		pEffectDesc._Effect_ShaderPass = {};

		pEffectDesc._Effect_bUseAtlas = false;
		pEffectDesc._Effect_TileCount = CEffectObject::_uint2{0, 0};

		// =======   이펙트 스크롤 Value   ===========
		pEffectDesc._Effect_ScrollSpeed = { 0.f, 0.f };

		// ========   이펙트 왜곡 Scale Value   ==========
		pEffectDesc._Effect_DistortionScale = { 0.f, 0.f };

		// ==========   이펙트 Sacle Value   ==============
		pEffectDesc._Effect_StartScale = { 1.f, 1.f, 1.f };
		pEffectDesc._Effect_EndScale = { 1.f, 1.f, 1.f };

		static_cast<Effect*>(pEffectContainer)->Add_Part(0, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect_Parts", &pEffectDesc);
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

wstring CEffectType_Selection_Panel::TypeToString(E_EffectSystemType eType)
{
	switch (eType)
	{
	case Tool::E_EffectSystemType::Particle:
		return L"ParticleSystem";
	case Tool::E_EffectSystemType::ForceField:
		return L"ForceField";
	case Tool::E_EffectSystemType::Line:
		return L"Line";
	case Tool::E_EffectSystemType::Trail:
		return L"Trail";
	}

	return L"NONE";
}

HRESULT CEffectType_Selection_Panel::EffectPanel_Initialize(CToolObject** pTarget)
{
	m_ppTargetSlot = pTarget;

	return S_OK;
}

CEffectType_Selection_Panel* CEffectType_Selection_Panel::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CToolObject** pTarget)
{
	CEffectType_Selection_Panel* pInstance = new CEffectType_Selection_Panel(pLabel, pOwner, pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : EffectType_Selection_Panel");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->EffectPanel_Initialize(pTarget)))
		return nullptr;

	return pInstance;
}


void CEffectType_Selection_Panel::Free()
{
    __super::Free();

	Safe_Release(m_pGameInstance);
}