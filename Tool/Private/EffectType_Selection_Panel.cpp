#include "pch.h"
#include "EffectType_Selection_Panel.h"
#include "CEffectObject.h"
#include "Transform.h"
#include "Effect.h"
#include "CameraMan.h"
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
		auto EffectList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::EFFECT), L"Effect_Layer");

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
	ImGui::Begin("##EFfect type Selection");
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

	EditEffect();
	TransformEffect(pGo);
	ImGui::End();
	
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
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;
		ToolDesc.pTransform_Desc = &transformDesc;

		ToolDesc.wstrLayerTag = L"Effect_Layer";

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
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;

		pEffectDesc.pMatParent = &(pEffectContainer->Get_Component<CTransform>()->Get_WorldMatrix());
		pEffectDesc.pTransform_Desc = &transformDesc;
		pEffectDesc.iPartsID = E_PartsObjectID::Effect_Particle;

		pEffectDesc.wstrLayerTag = L"Effect_Part_Particles";
		pEffectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::EFFECT);

		// ========     셰이더 선택   =========
		pEffectDesc.Data._Effect_Shader_Tag = L"Shader_VtxEffectParticle";
		pEffectDesc.Data._Effect_ShaderPass = 0;

		// ========     이펙트 타입   =========
		pEffectDesc.Data.eEffectSystemType = (_uint)DTO::E_EffectSystemType::Particle;
		pEffectDesc.Data.eEffectType = (_uint)DTO::E_EFFECTTYPE::Particle;
		pEffectDesc.Data.eEffectSystemType = (_uint)DTO::E_EffectSystemType::Particle;
		pEffectDesc.Data._Effect_EmissionType = (_uint)DTO::E_EMISSION_TYPE::BOX;
		pEffectDesc.Data._Effect_ShapeType = (_uint)DTO::E_SHAPETYPE::SPREAD;

		// =========   이펙트 Color Value   ===============
		pEffectDesc.Data._Effect_Color = Vec4{ 0.f, 0.f, 0.f, 1.f };

		// =========   이펙트 SRT   ============
		pEffectDesc.Data._Effect_StartScale = { 1.f, 1.f, 1.f };
		pEffectDesc.Data._Effect_EndScale = { 1.f, 1.f, 1.f };
		
		pEffectDesc.Data._Effect_StartRotation = { 0.f, 0.f, 0.f };
		pEffectDesc.Data._Effect_TargetRotation = { 0.f, 0.f, 0.f };

		// =========   버퍼 사이즈   ======================
		pEffectDesc.Data._Effect_MaxParticle = { 30 };
		pEffectDesc.Data._Effect_Looping = { true };
		pEffectDesc.Data._Effect_LifeTime = { 5.f };
		pEffectDesc.Data._Effect_Range = { 1.f, 1.f, 1.f };
		pEffectDesc.Data._Effect_StartSpeed = { 1.f };
		pEffectDesc.Data._Effect_ParticleSize = { 0.05f, 0.15f };
		pEffectDesc.Data.iRandomFlags = {};
		pEffectDesc.Data._Effect_StartDelay = 0.f;
		pEffectDesc.Data._Effect_PlayBackSpeed = 1.f;

		// ========  이펙트 Material 설정   ===========
		pEffectDesc.Data._Effect_Model_Tag = {};
		pEffectDesc.Data._Effect_DiffuseTexture_Tag = {};
		pEffectDesc.Data._Effect_NoiseTexture_Tag = {};
		pEffectDesc.Data._Effect_DiffuseTexture_Tag = {};

		// =======   이펙트 스크롤 Value   ===========
		pEffectDesc.Data._Effect_ScrollSpeed = { 0.f, 0.f };
		pEffectDesc.Data._Effect_DiscardValue = { 0.05f };

		// ========   이펙트 왜곡 Scale Value   ==========
		pEffectDesc.Data._Effect_DistortionScale = { 0.f, 0.f };

		// ==========   이펙트 Sacle Value   ==============
		pEffectDesc.Data._Effect_StartScale = { 1.f, 1.f, 1.f };
		pEffectDesc.Data._Effect_EndScale = { 1.f, 1.f, 1.f };

		static_cast<Effect*>(pEffectContainer)->Add_Part(0, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect_Part_Particle", &pEffectDesc);
		CEffectObject* pEFfectOBject = static_cast<Effect*>(pEffectContainer)->Get_Part<CEffectObject>(0);
		pEFfectOBject->Set_Name("DEFAULT_EFFECT");
		pEFfectOBject->Awake(ENUM_TO_UINT(ELevelType::EFFECT));
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

void CEffectType_Selection_Panel::EditEffect()
{
	if(ImGui::TreeNode("Edit##Container_EffectList"))
	{
		if (ImGui::TreeNode("Name Change##Container_EffectList"))
		{
			static char nameBuf[128] = {};
			string effectName = static_cast<Effect*>(*m_ppTargetSlot)->Get_Name();

			if (nameBuf[0] == '\0' && !effectName.empty())
			{
				std::string tmp = effectName;
				strncpy_s(nameBuf, sizeof(nameBuf), tmp.c_str(), _TRUNCATE);
			}
			// ImGui에서 입력 받기
			ImGui::InputText(" ##Container_EffectList", nameBuf, IM_ARRAYSIZE(nameBuf)); ImGui::SameLine();

			if (ImGui::Button("SAVE"))
			{
				effectName = nameBuf;
				static_cast<Effect*>(*m_ppTargetSlot)->Set_Name(effectName);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Delete##Container_EffectList"))
		{
			if (ImGui::Button("Delete##Container_EffectList"))
			{
				static_cast<Effect*>(*m_ppTargetSlot)->Set_Dead(L"Effect_Layer");
			}

			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void CEffectType_Selection_Panel::TransformEffect(CToolObject* pGo)
{
	if (ImGui::TreeNode("Transform##Position"))
	{
		CTransform* pTransform = pGo->Get_Component<CTransform>();
		Vec3 Position = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 Scale = pTransform->Get_Scaled();

		if (ImGui::Button("Test"))
		{
			Position = Vec3(20.f, 17.f, 0.f);
			pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, Position);

			m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, Position);
		}

		ImGui::SameLine(); 

		if (ImGui::Button("RollBack"))
		{
			Position = Vec3(0.f, 0.f, 0.f);
			pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, Position);

			m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, Position);
		}
		// -----------------------
		if (ImGui::DragFloat3(" ##Transform_ParticleSystem", &Position.x, 0.1f))
		{
			pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, Position);
		}

		ImGui::Text("ObjectPos : (%g, %g, %g)", Position.x, Position.y, Position.z);
		ImGui::Text("ObjectScale : (%g, %g, %g)", Scale.x, Scale.y, Scale.z);

		ImGui::TreePop();
	}
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