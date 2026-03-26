#include "pch.h"
#include "CParticle_System_Panel.h"
#include "Transform.h"
#include "Effect.h"
#include "CEffectObject.h"
#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Gravity_Force.h"
#include "Tool_PartObject.h"

CParticle_System_Panel::CParticle_System_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CParticle_System_Panel::Create_CanvasEffect()
{
	//// =========	Create	  ==============
	//CGameObject* pResult = { nullptr };

	//CEffectObject::Effect_Desc pEffectDesc = {};
	//CTransform::TRANSFORM_DESC transformDesc = {};
	//// =========    트랜스폼   ============
	//transformDesc.vPosition = { 0.f, 0.0f, 0.f };
	//transformDesc.fRotatePerSec = 1.f;
	//transformDesc.fMovePerSec = 1.f;

	//pEffectDesc.pMatParent = nullptr;
	//pEffectDesc.pTransform_Desc = &transformDesc;

	//pEffectDesc.wstrLayerTag = L"Effect_Parts";
	//pEffectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::EFFECT);
	//// ========     이펙트 타입   =========
	//pEffectDesc.eEffectSystemType = E_EffectSystemType::Particle;
	//pEffectDesc.eEffectType = E_EFFECTTYPE::Particle;
	//pEffectDesc.eEffectParticleType = E_PARTICLETYPE::TEXTURE;

	//// =========   이펙트 Color Value   ===============
	//pEffectDesc._Effect_Color = Vec4{ 0.f, 0.f, 0.f, 1.f };

	//// ========  이펙트 Material 설정   ===========
	//pEffectDesc._Effect_Model_Tag = {};
	//pEffectDesc._Effect_Shader_Tag = {};
	//pEffectDesc._Effect_DiffuseTexture_Tag = {};
	//pEffectDesc._Effect_NoiseTexture_Tag = {};
	//pEffectDesc._Effect_DiffuseTexture_Tag = {};
	//pEffectDesc._Effect_ShaderPass = {};

	//pEffectDesc._Effect_TileCount = CEffectObject::_uint2{ 0, 0 };

	//// =======   이펙트 스크롤 Value   ===========
	//pEffectDesc._Effect_ScrollSpeed = { 0.f, 0.f };

	//// ========   이펙트 왜곡 Scale Value   ==========
	//pEffectDesc._Effect_DistortionScale = { 0.f, 0.f };

	//// ==========   이펙트 Sacle Value   ==============
	//pEffectDesc._Effect_StartScale = { 1.f, 1.f, 1.f };
	//pEffectDesc._Effect_EndScale = { 1.f, 1.f, 1.f };

	//CBase* pClone = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect_Parts", &pEffectDesc);
	//	
	//if(pClone)
	//	m_pCanvasEffectObject = static_cast<CEffectObject*>(pClone);

	//else
	//	return E_FAIL;

	//
	//

	return S_OK;
}

void CParticle_System_Panel::Update(const _float fDT)
{
	if (m_bTimeSetting)
	{
	// === 시간 계산 ===ww
		Time_Calculator(fDT * m_tCurrentDesc.Data._Effect_PlayBackSpeed);
	}
}

HRESULT CParticle_System_Panel::Render(CToolObject* pGo)
{
	if (pGo == nullptr)
	{
		m_bTimeSetting = false;
		return E_FAIL;
	}
	m_bModified = false;

	Draw_Parts(pGo);

	// Effect Particle을 선택했을 때만 뜨게 한다.
	if (m_iSelectPartsType == ENUM_TO_UINT(E_PartsObjectID::Effect_Particle))
	{
		Draw_ParticleSystem(pGo);
		Draw_Timer(pGo);
		Draw_EffectColor(pGo);
		Draw_Drop_Texture(pGo);

		if (m_bModified)
			Binding_EffectDesc(pGo);
	}

	else if (m_iSelectPartsType == ENUM_TO_UINT(E_PartsObjectID::Effect_ForceField))
	{
		Draw_ForceField_Window(pGo);
	}


	return S_OK;
}

void CParticle_System_Panel::Binding_EffectDesc(CToolObject* pGo)
{
	if (pGo == nullptr) return;

	// 일단은 임시로.
	CEffectObject* EffectParts = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(m_iSelectPartsIndex);
	EffectParts->Set_EffectDesc(m_tCurrentDesc);
}

void CParticle_System_Panel::Time_Calculator(const float fDT)
{
	m_fTimeAccumulation += fDT;
}

void CParticle_System_Panel::Draw_Timer(CToolObject* pGo)
{
	ImGui::Begin("Particles");
	
	ImGui::Spacing();

	// 1. 제어 버튼 (Pause, Restart, Stop)
	ImGui::Separator();

	// ===================   Play Restart Stop   ===================

	if (ImGui::Button("Play"))
	{
		m_bTimeSetting = true;

		auto EffectPartList = static_cast<Effect*>(pGo)->Get_PartList();
		for (auto Part : EffectPartList)
		{
			static_cast<CEffectObject*>(Part)->TimeFlagRequest(0);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Pause"))
	{
		m_bTimeSetting = false;

		auto EffectPartList = static_cast<Effect*>(pGo)->Get_PartList();
		for (auto Part : EffectPartList)
		{
			static_cast<CEffectObject*>(Part)->TimeFlagRequest(1);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Reset"))
	{
		m_fTimeAccumulation = 0.f;

		// 일단은 임시로.
		auto EffectPartList = static_cast<Effect*>(pGo)->Get_PartList();
		for (auto Part : EffectPartList)
		{
			Part->Get_Component<CVIBuffer_Particle_Point>()->Reset_Simulation();
			static_cast<CEffectObject*>(Part)->TimeFlagRequest(2);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		m_bTimeSetting = false;
		m_fTimeAccumulation = 0.f;

		// 일단은 임시로.
		auto EffectPartList = static_cast<Effect*>(pGo)->Get_PartList();
		for (auto Part : EffectPartList)
		{
			Part->Get_Component<CVIBuffer_Particle_Point>()->Reset_Simulation();
			static_cast<CEffectObject*>(Part)->TimeFlagRequest(3);
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	//  ==================   Playback Speed   ====================== 

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Playback Speed");
	ImGui::Spacing();
	m_bModified |= ImGui::DragFloat("##PlaybackSpeed", &m_tCurrentDesc.Data._Effect_PlayBackSpeed, 0.1f, 0.f, 100.f);
	ImGui::Spacing();

	// ==================   Playback Time   =======================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Playback Time");
	ImGui::Spacing();
	m_bModified |= ImGui::InputFloat("##PlayBackTime", &m_fTimeAccumulation, 0.f);
	ImGui::Spacing();

	// Particles 
	// Speed Range
	// S
	ImGui::End();
}

void CParticle_System_Panel::Draw_ParticleSystem(CToolObject* pGo)
{
	ImGui::Begin("Particle System");

	ImGui::Separator();
	// 트랜스폼 - 파티클 시스템 위치 옮기기 (부모 위치 옮기기)

	// ==================//
   //  Particle System  //
  // ==================//

	if (ImGui::CollapsingHeader("Particle System"))
	{
		// 1. 시간 관련 설정 
		// ====================   Duration - 시스템이 한번 실행되는 총 시간   =====================
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Duration");
		ImGui::SameLine();
		ImGui::Text("Duration"); ImGui::SameLine();
		if (ImGui::InputFloat("##Duration1", &m_tCurrentDesc.Data._Effect_Duration, 0.1f))
		{
			// 0.01보다 작아지지 않게 방어
			if (m_tCurrentDesc.Data._Effect_Duration < 0.01f)
				m_tCurrentDesc.Data._Effect_Duration = 0.01f;
			m_bModified = true;
		}
		ImGui::Spacing();

		// ==================  Basic Setting  ==================
		if (ImGui::TreeNode("Particle Basic Setting##ParticleSystem"))
		{
			// ===================   Looping - 재생이 끝나면 반복할지 결정한다		=====================
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Looping");
			ImGui::SameLine();
			m_bModified |= ImGui::Checkbox("##Looping1", &m_tCurrentDesc.Data._Effect_Looping);
			ImGui::Spacing();

			// ===================   Burst   ===================
			m_bModified |= ImGui::Checkbox("Effect Particle Burst", &m_tCurrentDesc.Data._Use_Effect_Particle_Burst);

			// ===================   Continue   ===================
			m_bModified |= ImGui::Checkbox("Effect Particle Continue", &m_tCurrentDesc.Data._Use_Effect_Continue);

			// ===================   시간에 따른 LifeDissolve를 줄 것인가   ===================
			if (ImGui::Checkbox("UseLifeDissolve##LifeDissolve", &m_tCurrentDesc.Data._Effect_Tool_UseLifeDissolve))
			{
				if (m_tCurrentDesc.Data._Effect_Tool_UseLifeDissolve)
					Engine_Utils::Add_Flag(m_tCurrentDesc.Data._Effect_RenderFlag, 1 << 13); // SCROLL
				else
					Engine_Utils::RemoveHard_Flag(m_tCurrentDesc.Data._Effect_RenderFlag, 1 << 13);

				m_bModified |= true;
			}

			// ===================  UV Power (Tiling) 설정 추가  =====================
			ImGui::AlignTextToFramePadding();
			ImGui::Text("UV Power (Tiling)");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.f);

			// _uint 타입이므로 DragInt나 InputInt를 사용합니다.
			// 0이면 기능 꺼짐, 1이면 기본, 2 이상부터 반복
			if (ImGui::DragScalar("##UVXPower", ImGuiDataType_U32, &m_tCurrentDesc.Data._Effect_UVXPower, 0.1f))
			{
				m_bModified |= true;
			}

			if (ImGui::DragScalar("##UVYPower", ImGuiDataType_U32, &m_tCurrentDesc.Data._Effect_UVYPower, 0.1f))
			{
				m_bModified |= true;
			}

			ImGui::Spacing();

			ImGui::TreePop();
		}

		// =================   Start Delay - 재생 버튼을 누르고 실제 입자가 나오기까지 걸리는 시간   =========================
		ImGui::AlignTextToFramePadding();
		if(ImGui::TreeNode("Particle Time Setting##ParticleSystem"))
		{
			ImGui::Text("StartDelay"); ImGui::SameLine();
			m_bModified |= ImGui::InputFloat("##StartDelay1", &m_tCurrentDesc.Data._Effect_StartDelay, 0.f);
			ImGui::Text("StartLifeTime"); ImGui::SameLine();
			if (ImGui::InputFloat("##StartLifeTime1", &m_tCurrentDesc.Data._Effect_LifeTime, 0.1f))
			{
				// 0.01보다 작아지지 않게 방어
				if (m_tCurrentDesc.Data._Effect_LifeTime < 0.01f)
					m_tCurrentDesc.Data._Effect_LifeTime = 0.01f;
				m_bModified = true;
			}
			ImGui::Text("StartSpeed"); ImGui::SameLine();
			m_bModified |= ImGui::InputFloat("##StartSpeed1", &m_tCurrentDesc.Data._Effect_StartSpeed, 0.f);

			ImGui::TreePop();
		}

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Timeline Setting"))
		{
			ImGui::Text("Appear Time Ratio");
			ImGui::SameLine();
			// 0.2f로 설정하면 전체 0.5초 중 0.1초 동안 나타나게 됨
			if (ImGui::SliderFloat("##AppearRatio", &m_tCurrentDesc.Data._Effect_ApearRatio, 0.0f, 1.0f, "%.2f"))
			{
				m_bModified = true;
			}

			// 이해를 돕기 위한 텍스트 출력
			float actualTime = m_tCurrentDesc.Data._Effect_LifeTime * m_tCurrentDesc.Data._Effect_ApearRatio;
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Actual Appear Time: %.3f sec", actualTime);

			ImGui::TreePop();
		}

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Random Seed Settings##ParticleSystem"))
		{
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Property Random Flags");

			// 2. 비트 플래그 개별 제어 (정우님이 만든 E_RANDOM_FLAG 연동)
			// 현재 플래그 상태 확인
			bool bRandPos = (m_tCurrentDesc.Data.iRandomFlags & DTO::RAND_POS) != 0;
			bool bRandLife = (m_tCurrentDesc.Data.iRandomFlags & DTO::RAND_LIFE) != 0;
			bool bRandSize = (m_tCurrentDesc.Data.iRandomFlags & DTO::RAND_SIZE) != 0;
			bool bRandSpeed = (m_tCurrentDesc.Data.iRandomFlags & DTO::RAND_SPEED) != 0;

			// 위치 랜덤 체크박스
			if (ImGui::Checkbox("Random Position", &bRandPos))
			{
				if (bRandPos) m_tCurrentDesc.Data.iRandomFlags |= DTO::RAND_POS;
				else m_tCurrentDesc.Data.iRandomFlags &= ~DTO::RAND_POS;
				m_bModified = true;
			}

			// 수명 랜덤 체크박스
			if (ImGui::Checkbox("Random LifeTime", &bRandLife))
			{
				if (bRandLife) m_tCurrentDesc.Data.iRandomFlags |= DTO::RAND_LIFE;
				else m_tCurrentDesc.Data.iRandomFlags &= ~DTO::RAND_LIFE;
				m_bModified = true;
			}

			// 크기 랜덤 체크박스
			if (ImGui::Checkbox("Random Size", &bRandSize))
			{
				if (bRandSize) m_tCurrentDesc.Data.iRandomFlags |= DTO::RAND_SIZE;
				else m_tCurrentDesc.Data.iRandomFlags &= ~DTO::RAND_SIZE;
				m_bModified = true;
			}

			if (ImGui::Checkbox("Random Speed", &bRandSpeed))
			{
				if (bRandSpeed) m_tCurrentDesc.Data.iRandomFlags |= DTO::RAND_SPEED;
				else m_tCurrentDesc.Data.iRandomFlags &= ~DTO::RAND_SPEED;
				m_bModified = true;
			}

			ImGui::Spacing();
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Current Flag Value: %d", m_tCurrentDesc.Data.iRandomFlags);

			ImGui::TreePop();
		}

		// ==============   3D Start Rotation - 입자의 축마다의 회전값    =========================

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("3D Start Rotataion"))
		{
			m_bModified |= ImGui::Checkbox("##StartRotation_3D", &m_tCurrentDesc.Data._bUseStartRotation); ImGui::Spacing();

			if (m_tCurrentDesc.Data._bUseStartRotation)
			{
				static Vec3 StartRotation = { 0.f, 0.f, 0.f };

				ImGui::SeparatorText("Rotation 3D Convert To Radian - X Y Z - ");
				m_bModified |= ImGui::DragFloat3("##StartRotationX_3D", &m_tCurrentDesc.Data._Effect_StartRotation.x, 0.1f, -360.f, 360.f); ImGui::Spacing();

				CEffectObject* pInstance = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(m_iSelectPartsIndex);
				CTransform* pTransform = pInstance->Get_Component<CTransform>();
			}
			ImGui::TreePop();
		}

		// ===========   Position & Rotation & Scale =============
		ImGui::AlignTextToFramePadding();

		if (ImGui::TreeNode("Transform##Position"))
		{
			// 어차피 gizmo 연동이 되어있음.
			CTransform* pTranform = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(m_iSelectPartsIndex)->Get_Component<CTransform>();
			Vec3 Position = pTranform->Get_Info(TRANSFORM_INFO_STATE::POS);
			Vec3 Scale = pTranform->Get_Scaled();

			ImGui::SeparatorText("Position Setting##Transform");
			ImGui::DragFloat3(" ##Transform_ParticleSystem", &Position.x, 0.1f, -100.f, 100.f);
			pTranform->Set_Info(TRANSFORM_INFO_STATE::POS, Position);
			ImGui::NewLine();

			// ==============  Start Size - 입자의 초기 사이즈   =========================================
			ImGui::SeparatorText("Start Size##Size Setting");
			m_bModified |= ImGui::DragFloat3("##Start Size Setting", &m_tCurrentDesc.Data._Effect_StartScale.x, 0.1f, 0.1f, 100.f);

			ImGui::SeparatorText("End Size##Size Setting");
			m_bModified |= ImGui::DragFloat3("##End Size Setting", &m_tCurrentDesc.Data._Effect_EndScale.x, 0.1f, 0.1f, 100.f);

			if (m_tCurrentDesc.Data.eEffectParticleType != (_uint)DTO::E_PARTICLETYPE::MESH)
			{
				if (m_tCurrentDesc.Data._Effect_StartScale.x < 0.01f)
					m_tCurrentDesc.Data._Effect_StartScale.x = 0.01f;

				if (m_tCurrentDesc.Data._Effect_EndScale.x < 0.01f)
					m_tCurrentDesc.Data._Effect_EndScale.x = 0.01f;
			}

			ImGui::NewLine();
			ImGui::Text("ObjectPos : (%g, %g, %g)", Position.x, Position.y, Position.z);
			ImGui::Text("ObjectScale : (%g, %g, %g)", Scale.x, Scale.y, Scale.z);

			ImGui::TreePop();
		}
	
			// Gravity Source - 중력이 어디서 오는지 결정한다. 물리 상수 (9.8ms^2)
			// Gravity Modifier - 중력의 영향을 얼마나 강하게 받을지 정하는 배수. 0이면 무중력, 1이면 설정된 중력만큼 아래로 떨어진다.


			// Simulation Space - 파티클의 좌표계를 결정한다.
				// 1. Local - 부모가 움직이면 이미 생성된 입자도 부모를 따라 움직인다.
				// 2. World - 생성된 후에는 부모를 떠나 독립된 월드 좌표에서 움직입니다.

		ImGui::AlignTextToFramePadding(); if (ImGui::TreeNode("Simulation Space"))
		{
			bool isLocal = (static_cast<Effect*>(pGo)->Get_SimulationSpace() == DTO::E_SIMULATION_SPACE::LOCAL);
			bool isWorld = (static_cast<Effect*>(pGo)->Get_SimulationSpace() == DTO::E_SIMULATION_SPACE::WORLD);

			ImGui::Text("Local"); ImGui::SameLine(0, 45.f);
			ImGui::Text("World");

			// ==== Local 체크박스 ====
			if (ImGui::Checkbox("##LocalMode", &isLocal))
			{
				// 클릭 시 무조건 LOCAL로 변경
				static_cast<Effect*>(pGo)->Set_SimulationSpace(DTO::E_SIMULATION_SPACE::LOCAL);
				m_bModified = true;
			}

			ImGui::SameLine(0, 40.f);

			// ==== World 체크박스 ====
			if (ImGui::Checkbox("##WorldMode", &isWorld))
			{
				// 클릭 시 무조건 WORLD로 변경
				static_cast<Effect*>(pGo)->Set_SimulationSpace(DTO::E_SIMULATION_SPACE::WORLD);
				m_bModified = true;
			}

			// ====== 이쁘게 디자인하기 =======
			ImGui::Spacing();
			const char* modeText = (static_cast<Effect*>(pGo)->Get_SimulationSpace() == DTO::E_SIMULATION_SPACE::LOCAL) ? "LOCAL" : "WORLD";
			ImGui::Text("Active Mode: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", modeText);

			ImGui::TreePop();
		}

		// ===========  파티클 갯수  ===================

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Max Particles##Particle System"))
		{
			m_bModified |= ImGui::InputInt("##MaxParticles", &m_tCurrentDesc.Data._Effect_MaxParticle, 0);
			ImGui::TreePop();
		}

		// Simulation Speed - 전체 이펙트의 재생 속도 배수입니다. 2라면 2배속으로 빠르게 움직입니다.
		// Delta Time - 시간 계산 방식을 정합니다. 게임 속도에 맞춰진 Scaled 시간인가?
		// Scaling Mode - 부모 오브젝트의 크기가 변할 때 파티클이 어떻게 반응할지 결정
// 
		// Play On Awake* - 파티클 시스템이 씬에 생성되자마자 즉시 재생될지 여부를 결정한다.
		// Emitter Velocity Mode - 파티클이 방출될 때, 본체의 속도를 어떻게 계산할지 정합니다.
		// Max Particles = 동시에 존재할 수 있는 최대 입자 개수. 이 값만큼 iNSTANCE bUFFER의 크기를 잡아두면 성능상 좋음

		// Auto Random Seed - 난수 Seed를 다르게 생성해서 입자 모양이 랜덤하게 변한다.
		// Stop Action - 이펙트가 완전히 끝났을 때 해당 오브젝트를 어떻게 처리할지 정한다.
		// Culling Mode - 카메라 화면 밖에 있을 때 연산을 중단할지 여부를 결정해서 성능을 최적화합니다.
		// Ring Buffer Mode - 최대 입자 개수가 찼을 때, 가장 오래된 입자를 지우고 새 입자를 만들지 결정한다.
	}

	// ===============================//
   //		Physics / Gravity	     //
  // ===============================//
  
	// ===========  파티클 중력 계산  ==============

	if (ImGui::CollapsingHeader("Physics / Gravity"))
	{
		ImGui::Spacing();

		// 중력 기본 설정
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Gravity Modifier");
		ImGui::SameLine(140);
		ImGui::SetNextItemWidth(-1.0f);
		if (ImGui::DragFloat("##GravityModifier", &m_tCurrentDesc.Data._Effect_GravityModifier, 0.05f, -10.f, 10.f, "%.2f x g"))
		{
			m_tCurrentDesc.Data._Effect_Gravity_Value = m_tCurrentDesc.Data._Effect_GravityModifier * 9.81f;
			m_bModified = true;
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Gravity Direction");
		ImGui::SameLine(140);
		ImGui::SetNextItemWidth(-1.0f);
		m_bModified |= ImGui::DragFloat3("##GravityDir", &m_tCurrentDesc.Data._Effect_GravityDir.x, 0.1f, -1.f, 1.f);

		ImGui::Separator();

		// 중력 커브 설정 로직
		if (ImGui::TreeNode("Gravity Strength Curve"))
		{
			m_bModified |= ImGui::Checkbox("Use Gravity Curve", &m_tCurrentDesc.Data._bUseGlobalGravityCurve);

			if (m_tCurrentDesc.Data._bUseGlobalGravityCurve)
			{
				ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 150.0f);
				ImGui::InvisibleButton("GravityCurveCanvas", canvas_size);
				ImVec2 canvas_p0 = ImGui::GetItemRectMin();
				ImVec2 mouse_pos = ImGui::GetMousePos();

				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRectFilled(canvas_p0, ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y), IM_COL32(30, 30, 30, 255));

				auto& pCurveVec = m_tCurrentDesc.Data._vecGlobalGravityCurve; // DTO 벡터 연결
				ImU32 lineColor = IM_COL32(100, 200, 255, 255); // 중력은 시원한 하늘색으로

				for (size_t i = 0; i < pCurveVec.size(); ++i)
				{
					ImVec2 point_pos = ImVec2(
						canvas_p0.x + pCurveVec[i].fTimeKey * canvas_size.x,
						canvas_p0.y + (1.0f - pCurveVec[i].fValue) * canvas_size.y
					);

					// 삭제 (Ctrl + 좌클릭)
					float dist = sqrtf(powf(mouse_pos.x - point_pos.x, 2) + powf(mouse_pos.y - point_pos.y, 2));
					if (dist < 7.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::GetIO().KeyCtrl)
					{
						pCurveVec.erase(pCurveVec.begin() + i);
						m_bModified = true;
						break;
					}

					// 선 그리기
					if (i < pCurveVec.size() - 1)
					{
						ImVec2 next_point = ImVec2(
							canvas_p0.x + pCurveVec[i + 1].fTimeKey * canvas_size.x,
							canvas_p0.y + (1.0f - pCurveVec[i + 1].fValue) * canvas_size.y
						);
						draw_list->AddLine(point_pos, next_point, lineColor, 2.0f);
					}
					draw_list->AddCircleFilled(point_pos, 3.0f, IM_COL32(255, 255, 255, 255));
				}

				// 점 추가 (우클릭)
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					DTO::Gravity_CurveKey newKey;
					newKey.fTimeKey = (mouse_pos.x - canvas_p0.x) / canvas_size.x;
					newKey.fValue = (1.0f - (mouse_pos.y - canvas_p0.y) / canvas_size.y);
					pCurveVec.push_back(newKey);
					std::sort(pCurveVec.begin(), pCurveVec.end(), [](auto& a, auto& b) { return a.fTimeKey < b.fTimeKey; });
					m_bModified = true;
				}
			}
			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.f), "Final Base Gravity: %.2f m/s^2", m_tCurrentDesc.Data._Effect_Gravity_Value);
		ImGui::Separator();
	}

	// ===============================//
	//        Scale Curve Setting     //
	// ===============================//

	if (ImGui::CollapsingHeader("Scale Curve Settings"))
	{
		ImGui::Spacing();

		// 1. 커브 사용 여부 및 축 분리 여부
		m_bModified |= ImGui::Checkbox("Use Scale Curve", &m_tCurrentDesc.Data._bUseScaleCurve);
		ImGui::SameLine();
		m_bModified |= ImGui::Checkbox("Separate Axes", &m_tCurrentDesc.Data._bSeparateScaleAxes);

		if (m_tCurrentDesc.Data._bUseScaleCurve)
		{
			// 2. 가시성을 위한 최대 스케일 범위 설정 (캔버스 세로축 기준)
			static float fMaxScaleView = 50.0f;
			ImGui::SetNextItemWidth(100.f);
			ImGui::DragFloat("Max View Range", &fMaxScaleView, 1.0f, 1.0f, 1000.0f, "Max: %.f");
			ImGui::SameLine();
			ImGui::TextDisabled("(?) Adjust vertical height of canvas");

			// 축별로 루프 (SeparateAxes가 꺼져있으면 X만 표시)
			int iLoopCount = m_tCurrentDesc.Data._bSeparateScaleAxes ? 3 : 1;
			const char* pAxisLabels[] = { "Scale X (All)", "Scale Y", "Scale Z" };
			vector<DTO::Rotation_CurveKey>* pCurves[] = {
				&m_tCurrentDesc.Data._vecScaleCurveX,
				&m_tCurrentDesc.Data._vecScaleCurveY,
				&m_tCurrentDesc.Data._vecScaleCurveZ
			};
			ImU32 axisColors[] = { IM_COL32(255, 100, 100, 255), IM_COL32(100, 255, 100, 255), IM_COL32(100, 100, 255, 255) };

			for (int i = 0; i < iLoopCount; ++i)
			{
				if (ImGui::TreeNode(pAxisLabels[i]))
				{
					ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 200.0f); // 스케일은 좀 더 크게
					ImGui::InvisibleButton("ScaleCurveCanvas", canvas_size);
					ImVec2 canvas_p0 = ImGui::GetItemRectMin();
					ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y);
					ImVec2 mouse_pos = ImGui::GetMousePos();

					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(35, 35, 35, 255));

					// 가이드 라인 (0, 1.0 지점 표시)
					float y_1_0 = canvas_p0.y + (1.0f - (1.0f / fMaxScaleView)) * canvas_size.y;
					draw_list->AddLine(ImVec2(canvas_p0.x, y_1_0), ImVec2(canvas_p1.x, y_1_0), IM_COL32(100, 100, 100, 150));
					draw_list->AddText(ImVec2(canvas_p0.x + 5, y_1_0 - 15), IM_COL32(150, 150, 150, 255), "Value: 1.0");

					auto& pVec = *pCurves[i];

					for (size_t j = 0; j < pVec.size(); ++j)
					{
						// 좌표 계산: Value가 fMaxScaleView일 때 캔버스 상단(0.0)에 위치
						float fNormalizedVal = pVec[j].fValue / fMaxScaleView;
						ImVec2 point_pos = ImVec2(
							canvas_p0.x + pVec[j].fTimeKey * canvas_size.x,
							canvas_p0.y + (1.0f - fNormalizedVal) * canvas_size.y
						);

						// 삭제 (Ctrl + 좌클릭)
						float dist = sqrtf(powf(mouse_pos.x - point_pos.x, 2) + powf(mouse_pos.y - point_pos.y, 2));
						if (dist < 8.0f && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
						{
							pVec.erase(pVec.begin() + j);
							m_bModified = true; break;
						}

						// 선 그리기
						if (j < pVec.size() - 1)
						{
							float fNextNorm = pVec[j + 1].fValue / fMaxScaleView;
							ImVec2 next_point = ImVec2(
								canvas_p0.x + pVec[j + 1].fTimeKey * canvas_size.x,
								canvas_p0.y + (1.0f - fNextNorm) * canvas_size.y
							);
							draw_list->AddLine(point_pos, next_point, axisColors[i], 2.5f);
						}
						draw_list->AddCircleFilled(point_pos, 4.0f, IM_COL32(255, 255, 255, 255));

						// 값 표시 (현재 점 위에 수치 나타내기)
						char buf[16]; sprintf_s(buf, "%.1f", pVec[j].fValue);
						draw_list->AddText(ImVec2(point_pos.x + 5, point_pos.y - 15), IM_COL32(255, 255, 255, 200), buf);
					}

					// 점 추가 (우클릭) - 실제 스케일 값으로 변환하여 저장
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
					{
						DTO::Rotation_CurveKey newKey;
						newKey.fTimeKey = (mouse_pos.x - canvas_p0.x) / canvas_size.x;
						// 마우스 y 좌표를 fMaxScaleView 범위 내의 실제 수치로 변환
						float fRatio = 1.0f - (mouse_pos.y - canvas_p0.y) / canvas_size.y;
						newKey.fValue = fRatio * fMaxScaleView;

						pVec.push_back(newKey);
						std::sort(pVec.begin(), pVec.end(), [](auto& a, auto& b) { return a.fTimeKey < b.fTimeKey; });
						m_bModified = true;
					}
					ImGui::TreePop();
				}
			}

			// SeparateAxes가 꺼져있을 때 X 데이터를 Y, Z에 동기화
			if (!m_tCurrentDesc.Data._bSeparateScaleAxes) {
				m_tCurrentDesc.Data._vecScaleCurveY = m_tCurrentDesc.Data._vecScaleCurveX;
				m_tCurrentDesc.Data._vecScaleCurveZ = m_tCurrentDesc.Data._vecScaleCurveX;
			}
		}
	}

	// ===============================//
	//      Glow Power Curve Setting  //
	// ===============================//

	if (ImGui::CollapsingHeader("Glow Power Curve Settings"))
	{
		ImGui::Spacing();

		// 커브 사용 여부
		m_bModified |= ImGui::Checkbox("Use Glow Power Curve", &m_tCurrentDesc.Data._bUseGlowPowerCurve);

		if (m_tCurrentDesc.Data._bUseGlowPowerCurve)
		{
			static float fMaxGlowView = 20.0f;
			ImGui::SetNextItemWidth(100.f);
			ImGui::DragFloat("Glow Max View", &fMaxGlowView, 1.0f, 1.0f, 500.0f, "Max: %.f");
			ImGui::SameLine();
			ImGui::TextDisabled("(?) Adjust vertical height for Glow Intensity");

			ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 150.0f);
			ImGui::InvisibleButton("GlowCurveCanvas", canvas_size);
			ImVec2 canvas_p0 = ImGui::GetItemRectMin();
			ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y);
			ImVec2 mouse_pos = ImGui::GetMousePos();

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(30, 30, 40, 255)); // 글로우 느낌의 푸르스름한 배경

			float y_base = canvas_p0.y + (1.0f - (m_tCurrentDesc.Data._Effect_GlowPower / fMaxGlowView)) * canvas_size.y;
			draw_list->AddLine(ImVec2(canvas_p0.x, y_base), ImVec2(canvas_p1.x, y_base), IM_COL32(200, 200, 100, 100));
			draw_list->AddText(ImVec2(canvas_p0.x + 5, y_base - 15), IM_COL32(200, 200, 100, 200), "Base Power");

			auto& pVec = m_tCurrentDesc.Data._vecGlowPowerCurve;
			ImU32 glowColor = IM_COL32(255, 255, 150, 255); // 황금색/빛나는 색

			for (size_t j = 0; j < pVec.size(); ++j)
			{
				float fNormalizedVal = pVec[j].fValue / fMaxGlowView;
				ImVec2 point_pos = ImVec2(
					canvas_p0.x + pVec[j].fTimeKey * canvas_size.x,
					canvas_p0.y + (1.0f - fNormalizedVal) * canvas_size.y
				);

				float dist = sqrtf(powf(mouse_pos.x - point_pos.x, 2) + powf(mouse_pos.y - point_pos.y, 2));
				if (dist < 8.0f && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
				{
					pVec.erase(pVec.begin() + j);
					m_bModified = true; break;
				}

				if (j < pVec.size() - 1)
				{
					float fNextNorm = pVec[j + 1].fValue / fMaxGlowView;
					ImVec2 next_point = ImVec2(
						canvas_p0.x + pVec[j + 1].fTimeKey * canvas_size.x,
						canvas_p0.y + (1.0f - fNextNorm) * canvas_size.y
					);
					draw_list->AddLine(point_pos, next_point, glowColor, 2.0f);
				}
				draw_list->AddCircleFilled(point_pos, 4.0f, IM_COL32(255, 255, 255, 255));

				char buf[16]; sprintf_s(buf, "%.1f", pVec[j].fValue);
				draw_list->AddText(ImVec2(point_pos.x + 5, point_pos.y - 15), IM_COL32(255, 255, 255, 200), buf);
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
			{
				DTO::Rotation_CurveKey newKey;
				newKey.fTimeKey = (mouse_pos.x - canvas_p0.x) / canvas_size.x;
				float fRatio = 1.0f - (mouse_pos.y - canvas_p0.y) / canvas_size.y;
				newKey.fValue = fRatio * fMaxGlowView;

				pVec.push_back(newKey);
				std::sort(pVec.begin(), pVec.end(), [](auto& a, auto& b) { return a.fTimeKey < b.fTimeKey; });
				m_bModified = true;
			}
		}
	}



	// ==================//
   //     Emission      //
  // ==================//
	if (ImGui::CollapsingHeader("Emission"))
	{
		// - Emission
		// Rate Over Time - 1초당 생성할 파티클 개수. AccumulatedTime >= 1.0f / Rate일 때 입자를 생성하는 로직을 짠다.
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Rate Over Time"))
		{
			m_bModified |= ImGui::InputFloat("##RateOverTime", &m_tCurrentDesc.Data._Effect_RateOverTime, 0.f);
			ImGui::TreePop();
		}
		// Rate Over Distance - 본체가 이동한 거리에 비례해서 입자를 뿜습니다. 
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Rate Over Distance"))
		{
			m_bModified |= ImGui::InputFloat("##RateOverDistance", &m_tCurrentDesc.Data._Effect_RateOverTime, 0.f);
			ImGui::TreePop();
		}
		// Bursts - 특정 시간에 파티클을 한꺼번에 '확' 터뜨리는 기능입니다.
			// Time - 시스템 시작 후 몇 초 뒤에 터뜨릴지 정합니다. 
			// Count - 그 순간 한꺼번에 생성할 입자의 개수입니다.
			// Cycles - 이 작업을 몇 번 반복할지 정합니다.
			// Interval : 반복할 때 ㅅ이의 시간 간격입니다.
			// Probability - 해당 폭발이 일어날 확률(0~1)
	}

	// ===============================//
   //  Rotataion over LifeTime       //
  // ===============================//
	if (ImGui::CollapsingHeader("Rotation Curve Graph"))
	{
		// 체크 박스 활성화
		if (ImGui::TreeNode("Rotation Settings"))
		{
			m_bModified |= ImGui::Checkbox("Use Rotation Curve", &m_tCurrentDesc.Data._bUseRotationCurve);
			if (m_tCurrentDesc.Data._bUseRotationCurve)
			{
				// 최종 목표 회전량 설정하기
				ImGui::SeparatorText("Target Rotation (Total over Duration)");
				m_bModified |= ImGui::DragFloat3("##TargetRot", &m_tCurrentDesc.Data._Effect_TargetRotation.x, 1.f);

				ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.f, 1.f), "Info : 360deg = 1 Full Loop");
			}
			ImGui::TreePop();
		}

		if (m_tCurrentDesc.Data._bUseRotationCurve)
		{
			ImGui::Text("Select Axis: "); ImGui::SameLine();
			ImGui::RadioButton("X", &m_iSelectedRotationAxis, 0); ImGui::SameLine();
			ImGui::RadioButton("Y", &m_iSelectedRotationAxis, 1); ImGui::SameLine();
			ImGui::RadioButton("Z", &m_iSelectedRotationAxis, 2);

			if (ImGui::TreeNode("Curve Editor##Rotation Curve Graph"))
			{
				ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 150.0f);
				ImGui::InvisibleButton("CurveCanvas", canvas_size);
				ImVec2 canvas_p0 = ImGui::GetItemRectMin();
				ImVec2 mouse_pos = ImGui::GetMousePos(); 

				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRectFilled(canvas_p0, ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y), IM_COL32(30, 30, 30, 255));

				vector<DTO::Rotation_CurveKey>* pCurveVec = nullptr;
				ImU32 lineColor = IM_COL32(255, 255, 0, 255);

				if (m_iSelectedRotationAxis == 0) { pCurveVec = &m_tCurrentDesc.Data._vecRotationCurveX; lineColor = IM_COL32(255, 100, 100, 255); }
				else if (m_iSelectedRotationAxis == 1) { pCurveVec = &m_tCurrentDesc.Data._vecRotationCurveY; lineColor = IM_COL32(100, 255, 100, 255); }
				else { pCurveVec = &m_tCurrentDesc.Data._vecRotationCurveZ; lineColor = IM_COL32(100, 100, 255, 255); }

				if (pCurveVec)
				{
					for (size_t i = 0; i < pCurveVec->size(); ++i)
					{
						//  현재 점의 화면 좌표 계산
						ImVec2 point_pos = ImVec2(
							canvas_p0.x + (*pCurveVec)[i].fTimeKey * canvas_size.x,
							canvas_p0.y + (1.0f - ((*pCurveVec)[i].fValue)) * canvas_size.y
						);

						//  삭제 로직: Ctrl + 좌클릭 시 해당 점 삭제 (유니티 스타일)
						float dist = sqrtf(powf(mouse_pos.x - point_pos.x, 2) + powf(mouse_pos.y - point_pos.y, 2));
						if (dist < 7.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::GetIO().KeyCtrl)
						{
							pCurveVec->erase(pCurveVec->begin() + i);
							m_bModified = true;
							break;
						}

						// 선 그리기
						if (i < pCurveVec->size() - 1)
						{
							ImVec2 next_point = ImVec2(
								canvas_p0.x + (*pCurveVec)[i + 1].fTimeKey * canvas_size.x,
								canvas_p0.y + (1.0f - ((*pCurveVec)[i + 1].fValue)) * canvas_size.y
							);
							draw_list->AddLine(point_pos, next_point, lineColor, 2.0f);
						}
						draw_list->AddCircleFilled(point_pos, 3.0f, IM_COL32(255, 255, 255, 255));
					}
				}

				// 우클릭 추가 로직
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					DTO::Rotation_CurveKey newKey;
					newKey.fTimeKey = (mouse_pos.x - canvas_p0.x) / canvas_size.x;
					newKey.fValue = (1.0f - (mouse_pos.y - canvas_p0.y) / canvas_size.y);
					pCurveVec->push_back(newKey);
					std::sort(pCurveVec->begin(), pCurveVec->end(), [](auto& a, auto& b) { return a.fTimeKey < b.fTimeKey; });
					m_bModified = true;
				}
				ImGui::TreePop();
			}
		}
	}


   // ===============================//
  //  UV Scroll Curve (X/Y Separate)//
 // ===============================//

	if (ImGui::CollapsingHeader("UV Scroll Curve (X/Y Separate)##UV Scroll Curve"))
	{
		m_bModified |= ImGui::Checkbox("Use OverScroll Curve", &m_tCurrentDesc.Data._Use_Effect_UV_OverScroll); ImGui::NewLine();
		// 1. 사용 여부 체크박스
		m_bModified |= ImGui::Checkbox("Use Scroll Curve", &m_tCurrentDesc.Data._bUseUVScrollCurve);

		if (m_tCurrentDesc.Data._bUseUVScrollCurve)
		{
			// 2. 축 선택 라디오 버튼
			ImGui::Text("Select Axis: ##UV Scroll Curve"); ImGui::SameLine();
			ImGui::RadioButton("X##UV Scroll Curve", &m_iSelectedScrollAxis, 0); ImGui::SameLine();
			ImGui::RadioButton("Y##UV Scroll Curve", &m_iSelectedScrollAxis, 1);

			if (ImGui::TreeNode("Curve Editor##UV Scroll Curve"))
			{
				// 3. 캔버스 설정 (네 코드 규격 그대로)
				ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 150.0f);
				ImGui::InvisibleButton("ScrollCurveCanvas##UV Scroll Curve", canvas_size);
				ImVec2 canvas_p0 = ImGui::GetItemRectMin();
				ImVec2 mouse_pos = ImGui::GetMousePos();

				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRectFilled(canvas_p0, ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y), IM_COL32(30, 30, 30, 255));

				// 4. 선택된 축에 따른 벡터 포인터 할당
				vector<DTO::Rotation_CurveKey>* pCurveVec = nullptr;
				ImU32 lineColor = IM_COL32(255, 255, 0, 255);

				if (m_iSelectedScrollAxis == 0) { pCurveVec = &m_tCurrentDesc.Data._vecUVScrollCurveX; lineColor = IM_COL32(255, 100, 100, 255); }
				else { pCurveVec = &m_tCurrentDesc.Data._vecUVScrollCurveY; lineColor = IM_COL32(100, 255, 100, 255); }

				if (pCurveVec)
				{
					for (size_t i = 0; i < pCurveVec->size(); ++i)
					{
						// 점 좌표 계산
						ImVec2 point_pos = ImVec2(
							canvas_p0.x + (*pCurveVec)[i].fTimeKey * canvas_size.x,
							canvas_p0.y + (1.0f - ((*pCurveVec)[i].fValue)) * canvas_size.y
						);

						// 삭제 로직 (Ctrl + 좌클릭)
						float dist = sqrtf(powf(mouse_pos.x - point_pos.x, 2) + powf(mouse_pos.y - point_pos.y, 2));
						if (dist < 7.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::GetIO().KeyCtrl)
						{
							pCurveVec->erase(pCurveVec->begin() + i);
							m_bModified = true;
							break;
						}

						// 선 그리기
						if (i < pCurveVec->size() - 1)
						{
							ImVec2 next_point = ImVec2(
								canvas_p0.x + (*pCurveVec)[i + 1].fTimeKey * canvas_size.x,
								canvas_p0.y + (1.0f - ((*pCurveVec)[i + 1].fValue)) * canvas_size.y
							);
							draw_list->AddLine(point_pos, next_point, lineColor, 2.0f);
						}
						draw_list->AddCircleFilled(point_pos, 3.0f, IM_COL32(255, 255, 255, 255));
					}
				}

				// 점 추가 로직 (우클릭)
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					DTO::Rotation_CurveKey newKey;
					newKey.fTimeKey = (mouse_pos.x - canvas_p0.x) / canvas_size.x;
					newKey.fValue = (1.0f - (mouse_pos.y - canvas_p0.y) / canvas_size.y);
					pCurveVec->push_back(newKey);
					std::sort(pCurveVec->begin(), pCurveVec->end(), [](auto& a, auto& b) { return a.fTimeKey < b.fTimeKey; });
					m_bModified = true;
				}
				ImGui::TreePop();
			}
		}
	}
	// ==================//
   //       Shape       //
  // ==================//

	if (ImGui::CollapsingHeader("Shape"))
	{
		// E_EMISSION_TYPE 설정하기
		if (ImGui::TreeNode("EmissionType Setting##Effect_List"))
		{
			vector<string> m_pEmissionList;
			m_pEmissionList.clear();

			m_pEmissionList.push_back("BOX");
			m_pEmissionList.push_back("CIRCLE");
			m_pEmissionList.push_back("SPHERE");
			m_pEmissionList.push_back("CONE");
			m_pEmissionList.push_back("CIRCLE_EDGE");

			std::vector<const char*> iTems;
			iTems.reserve(static_cast<int>(m_pEmissionList.size()));

			for (auto& str : m_pEmissionList)
				iTems.push_back(str.c_str());

			if (ImGui::ListBox("", &m_iSelectedEmissionIdx, iTems.data(), static_cast<int>(m_pEmissionList.size()), 6))
			{
				m_bModified |= true;
				m_tCurrentDesc.Data._Effect_EmissionType = m_iSelectedEmissionIdx;
			}

			if ((iTems.size() - 1) < m_iSelectedEmissionIdx)
			{
				ImGui::TreePop();
				return;
			}

			const char* EmissionNames[] = { "BOX", "CIRCLE", "SPHERE", "CONE", "CIRCLE_EDGE"};
			int currentIndex = (int)m_tCurrentDesc.Data._Effect_EmissionType;

			ImGui::Spacing();
			ImGui::Text("Selected EmissionType: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", EmissionNames[currentIndex]);
			ImGui::Separator();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Shape_EffectList##Effect_List"))
		{
			vector<string> m_pShapeList = {
				"NONE", "DROP", "RISE", "SPREAD", "STOP", 
				"SPIRAL", "DNA", "GATHER", "FOUNTAIN", "LEAF", 
				"CIRCLE_TRAIL", "SEMI_CIRCLETRAIL", "WIND_LEAF" , "STRONGWIND_LEAF",
				"IRREGULAR_SPREAD", "IRREGULAR_FOUNTAIN",
			};

			std::vector<const char*> iTems;
			iTems.reserve(static_cast<int>(m_pShapeList.size()));

			for (auto& str : m_pShapeList)
				iTems.push_back(str.c_str());

			if (ImGui::ListBox("", &m_iSelectedShapeIdx, iTems.data(), static_cast<int>(m_pShapeList.size()), 6))
			{
				m_bModified |= true;

				switch (m_iSelectedShapeIdx) {
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::NONE): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::NONE); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::DROP): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::DROP); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::RISE): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::RISE); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::SPREAD): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::SPREAD); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::STRAIGHT): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::STRAIGHT); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::SPIRAL): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::SPIRAL); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::DNA): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::DNA); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::GATHER): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::GATHER); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::FOUNTAIN): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::FOUNTAIN); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::LEAF): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::LEAF); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::CIRCLETRAIL): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::CIRCLETRAIL); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::SEMI_CIRCLETRAIL): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::SEMI_CIRCLETRAIL); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::WIND_LEAF): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::WIND_LEAF); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::IRREGULAR_SPREAD): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::IRREGULAR_SPREAD); break;
				case ENUM_TO_UINT(DTO::E_SHAPETYPE::IRREGULAR_FOUNTAIN): m_tCurrentDesc.Data._Effect_ShapeType = ENUM_TO_UINT(DTO::E_SHAPETYPE::IRREGULAR_FOUNTAIN); break;

				}
			}

			if ((iTems.size() - 1) < m_iSelectedShapeIdx)
			{
				ImGui::TreePop();
				return;
			}

			const char* shapeNames[] = { 
				"NONE", "DROP", "RISE", "SPREAD", "STOP", 
				"SPIRAL", "DNA", "GATHER", "FOUNTAIN", "LEAF", 
				"CIRCLE_TRAIL", "SEMI_CIRCLETRAIL", "WIND_LEAF", "STRONGWIND_LEAF", 
				"IRREGULAR_SPREAD", "IRREGULAR_FOUNTAIN"};
			int currentIndex = (int)m_tCurrentDesc.Data._Effect_ShapeType;

			ImGui::Spacing();
			ImGui::Text("Selected Shape: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", shapeNames[currentIndex]);
			ImGui::Separator();

			ImGui::TreePop();
		}

		// ============   Distortion   Scale 설정하기   ===========
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Distortion Scale##Shape"))
		{
			m_bModified |= ImGui::DragFloat2("##Distortion Scale", &m_tCurrentDesc.Data._Effect_DistortionScale.x, 0.1f, 0.1f, 100.f);
			ImGui::TreePop();
		}

		// ============ Scroll 값 설정하기 ============
		ImGui::AlignTextToFramePadding();

		if (ImGui::TreeNode("Scroll Settings##Shape"))
		{
			// 공용 베이스 스크롤 속도
			m_bModified |= ImGui::DragFloat2("Base Speed", &m_tCurrentDesc.Data._Effect_ScrollSpeed.x, 0.1f);

			ImGui::SeparatorText("Individual Texture Scroll Weight");

			// 각 텍스처별 상세 설정 헬퍼 람다
			auto DrawTextureScrollUI = [&](const char* label, bool& toolFlag, uint32_t bit, Vec2& weight) {
				if (ImGui::Checkbox(label, &toolFlag)) {
					if (toolFlag) m_tCurrentDesc.Data._Effect_RenderFlag |= bit;
					else m_tCurrentDesc.Data._Effect_RenderFlag &= ~bit;
					m_bModified = true;
				}
				if (toolFlag) {
					ImGui::SameLine(120);
					ImGui::SetNextItemWidth(150);
					string dragLabel = "##Weight" + string(label);
					m_bModified |= ImGui::DragFloat2(dragLabel.c_str(), &weight.x, 0.01f, -2.0f, 2.0f, "W:%.2f");
				}
				};

			// 1. Diffuse (1 << 6)
			DrawTextureScrollUI("Diffuse", m_tCurrentDesc.Data._Effect_Tool_UseScroll_Diffuse, 1 << 6, m_tCurrentDesc.Data._Effect_DiffuseTexture_ScrollWeight);
			// 2. Noise (1 << 7)
			DrawTextureScrollUI("Noise", m_tCurrentDesc.Data._Effect_Tool_UseScroll_Noise, 1 << 7, m_tCurrentDesc.Data._Effect_NoiseTexture_ScrollWeight);
			// 3. Masking (1 << 8)
			DrawTextureScrollUI("Masking", m_tCurrentDesc.Data._Effect_Tool_UseScroll_Masking, 1 << 8, m_tCurrentDesc.Data._Effect_MaskingTexture_ScrollWeight);
			// 4. Gradation (1 << 9)
			DrawTextureScrollUI("Gradation", m_tCurrentDesc.Data._Effect_Tool_UseScroll_Gradation, 1 << 9, m_tCurrentDesc.Data._Effect_GradationTexture_ScrollWeight);
			// 5. DissolveTexture (1 << 10)
			DrawTextureScrollUI("Dissolve", m_tCurrentDesc.Data._Effect_Tool_UseScroll_Dissolve, 1 << 10, m_tCurrentDesc.Data._Effect_DissolveTexture_ScrollWeight);
			// 6. GlowTexture(1 << 11)
			DrawTextureScrollUI("Glow", m_tCurrentDesc.Data._Effect_Tool_UseScroll_Glow, 1 << 11, m_tCurrentDesc.Data._Effect_GlowTexture_ScrollWeight);
			// 7. CurveTexture(1 << 12)
			DrawTextureScrollUI("Curve", m_tCurrentDesc.Data._Effect_Tool_UseScroll_Curve, 1 << 12, m_tCurrentDesc.Data._Effect_CurveTexture_ScrollWeight);
			// 8. SubMaskTexture(1 << 14)
			DrawTextureScrollUI("SubMask", m_tCurrentDesc.Data._Effect_Tool_UseSCroll_SubMask, 1 << 14, m_tCurrentDesc.Data._Effect_SubMaskTexture_ScrollWeight);
			
			ImGui::TreePop();
		}

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("UV Offset"))
		{
			m_bModified |= ImGui::DragFloat2("##UV Offset", &m_tCurrentDesc.Data._Effect_UV_Offset.x, 0.01f, -1.f, 1.f);
			ImGui::TreePop();
		}

		// ============  Discard Value 설정하기   ==========
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Discard Value##Shape"))
		{
			m_bModified |= ImGui::DragFloat("##Discard Value", &m_tCurrentDesc.Data._Effect_DiscardValue, 0.01f, 0.f, 1.f);
			ImGui::TreePop();
		}

		// ============   Radius   ============= 
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Range"))
		{
			m_bModified |= ImGui::DragFloat3("##Range", &m_tCurrentDesc.Data._Effect_Range.x, 0.1f, 0.1f, 100.f);
			ImGui::TreePop();
		}

		// ===========  size 변경 
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Particle Size ##Shape"))
		{
			m_bModified |= ImGui::DragFloat2("Particle Size", &m_tCurrentDesc.Data._Effect_ParticleSize.x, 0.01f, 0.0f, 100.f, "%.2f");
			if (m_tCurrentDesc.Data._Effect_ParticleSize.x >= m_tCurrentDesc.Data._Effect_ParticleSize.y)
			{
				m_tCurrentDesc.Data._Effect_ParticleSize.y = m_tCurrentDesc.Data._Effect_ParticleSize.x + 0.01f;
			}
			ImGui::TreePop();
		}

		// 1. Diffuse Texture
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Diffuse Texture"); ImGui::SameLine(130);
		if (ImGui::Button("Select##Diffuse")) ImGui::OpenPopup("TextureSelector##Diffuse");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##Diffuse", &m_tCurrentDesc.Data._Effect_Tool_DiffuseTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_DiffuseTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 0);
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 0);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##Diffuse", m_tCurrentDesc.Data._Effect_DiffuseTexture_Tag);

		ImGui::Spacing();

		// 2. Noise Texture
		ImGui::Text("Noise Texture"); ImGui::SameLine(130);
		if (ImGui::Button("Select##Noise")) ImGui::OpenPopup("TextureSelector##Noise");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##Noise", &m_tCurrentDesc.Data._Effect_Tool_NoiseTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_NoiseTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 1);
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 1);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##Noise", m_tCurrentDesc.Data._Effect_NoiseTexture_Tag);

		ImGui::Spacing();

		// 3. Masking Texture
		ImGui::Text("Masking Texture"); ImGui::SameLine(130);
		if (ImGui::Button("Select##Masking")) ImGui::OpenPopup("TextureSelector##Masking");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##Masking", &m_tCurrentDesc.Data._Effect_Tool_MaskingTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_MaskingTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 2);
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 2);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##Masking", m_tCurrentDesc.Data._Effect_MaskingTexture_Tag);

		ImGui::Spacing();

		// 3. Gradation Texture (Color Atlas)
		ImGui::Text("Gradation Tex"); ImGui::SameLine(130);
		if (ImGui::Button("Select##Gradation")) ImGui::OpenPopup("TextureSelector##Gradation");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##Gradation", &m_tCurrentDesc.Data._Effect_Tool_GradationTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_GradationTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 3);
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 3);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##Gradation", m_tCurrentDesc.Data._Effect_GradationTexture_Tag);

		ImGui::Spacing();

		// 4. Curve Texture 
		ImGui::Text("Curve Texture"); ImGui::SameLine(130);
		if (ImGui::Button("Select##Curve")) ImGui::OpenPopup("TextureSelector##Curve");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##Curve", &m_tCurrentDesc.Data._Effect_Tool_CurveTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_CurveTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 4); // DISSOLVETEXTURE 7
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 4);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##Curve", m_tCurrentDesc.Data._Effect_CurveTexture_Tag);

		// 5. Dissolve Texture
		ImGui::Text("Dissolve Texture"); ImGui::SameLine(130);
		if (ImGui::Button("Select##Dissolve")) ImGui::OpenPopup("TextureSelector##Dissolve");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##Dissolve", &m_tCurrentDesc.Data._Effect_Tool_DissolveTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_DissolveTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 7); // DISSOLVETEXTURE 7
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 7);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##Dissolve", m_tCurrentDesc.Data._Effect_DissolveTexture_Tag);


		// 5. Glow Texture 
		ImGui::Text("Glow Texture"); ImGui::SameLine(130);
		if (ImGui::Button("Select##Glow")) ImGui::OpenPopup("TextureSelector##Glow");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##Glow", &m_tCurrentDesc.Data._Effect_Tool_GlowTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_GlowTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 6); // DISSOLVETEXTURE 7
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 6);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##Glow", m_tCurrentDesc.Data._Effect_GlowTexture_Tag);


		// 6. SubMasking Texture 
		ImGui::Text("SubMask Texture"); ImGui::SameLine(130);
		if (ImGui::Button("Select##SubMask")) ImGui::OpenPopup("TextureSelector##SubMask");
		ImGui::SameLine();
		if (ImGui::Checkbox("Use##SubMask", &m_tCurrentDesc.Data._Effect_Tool_SubMaskTexture)) {
			if (m_tCurrentDesc.Data._Effect_Tool_SubMaskTexture) m_tCurrentDesc.Data._Effect_TextureFlag |= (1 << 6); // SubMaskTexture 8
			else m_tCurrentDesc.Data._Effect_TextureFlag &= ~(1 << 8);
			m_bModified = true;
		}
		Draw_TextureSelectorPopup("TextureSelector##SubMask", m_tCurrentDesc.Data._Effect_SubMaskTexture_Tag);


		// ===========  Texture Rotation  ================
	// 
	// Texture Setting 버튼
		if (ImGui::Button("Texture Setting"))
		{
			m_bShowRotationModal = true;
		}

		if (m_bShowRotationModal)
		{
			Draw_Rotation_Texture(pGo);
		}

	}
#pragma endregion
	// ==========  Position & Rotation & Scale ===========

		// Type 
			// Mode 
		// Mesh
		// Single Material
		// Use Mesh Colors
		// Normal Offsset
		// Texture


	// Align To Direction
	// Randomize Direction
	// Spherize Direction
	// Randomize Position


	// ==================//
   //       RENDERER    //
  // ==================//

	// 공용으로 사용할 람다식
	auto PackSamplerFlag = [](uint32_t& totalFlag, int selectedIdx, int shift) {
		// 해당 위치의 3비트를 0으로 밀어버림
		totalFlag &= ~(0x7 << shift);
		// 새로운 인덱스를 shift만큼 밀어서 꽂아넣음 
		totalFlag |= (selectedIdx << shift);
		};


	if (ImGui::CollapsingHeader("Renderer"))
	{
		// ============ RenderState check box =============

		if (ImGui::TreeNode("RenderState Setting##Renderer"))
		{
			// =============  [USE BILLBOARD]  ==============
			ImGui::Text("Bilboard"); ImGui::SameLine(0, 200.f);
			if (ImGui::Checkbox("Use##BillBoardRenderer", &m_tCurrentDesc.Data._Effect_Tool_UseBillboard))
			{
				if (m_tCurrentDesc.Data._Effect_Tool_UseBillboard)
					Engine_Utils::Add_Flag(m_tCurrentDesc.Data._Effect_RenderFlag, 1);

				else
					Engine_Utils::RemoveHard_Flag(m_tCurrentDesc.Data._Effect_RenderFlag, 1);

				m_bModified |= true;
			}
			// =============  [CHOOSE SAMPLERSTATE]	  ============

			// Diffuse Texture##SamplerState
			if (ImGui::TreeNode("Diffuse Texture##SamplerState"))
			{
				vector<string> m_PParticleTypeList;
				m_PParticleTypeList.clear();
				m_PParticleTypeList.push_back("LinearSampler");
				m_PParticleTypeList.push_back("LinearClampSampler");
				m_PParticleTypeList.push_back("LinearBorderSampler");
				m_PParticleTypeList.push_back("LinearMirrorSampler");
				m_PParticleTypeList.push_back("PointSampler");

				std::vector<const char*> iTems1;
				iTems1.reserve(static_cast<int>(m_PParticleTypeList.size()));

				for (auto& str : m_PParticleTypeList)
					iTems1.push_back(str.c_str());

				if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc.Data._Effect_Tool_DiffuseSamplerState_Flag, [](void* data, int idx, const char** out_text)
					{
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
					},
					(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
				{
					PackSamplerFlag(m_tCurrentDesc.Data._Effect_SamplerStateFlag, m_tCurrentDesc.Data._Effect_Tool_DiffuseSamplerState_Flag, 0);
					m_bModified |= true;
				}

		
				ImGui::TreePop();
				ImGui::Spacing();
			}
			// Noise Texture##SamplerState
			if (ImGui::TreeNode("Noise Texture##SamplerState"))
			{
				vector<string> m_PParticleTypeList;
				m_PParticleTypeList.clear();

				m_PParticleTypeList.push_back("LinearSampler");
				m_PParticleTypeList.push_back("LinearClampSampler");
				m_PParticleTypeList.push_back("LinearBorderSampler");
				m_PParticleTypeList.push_back("LinearMirrorSampler");
				m_PParticleTypeList.push_back("PointSampler");

				std::vector<const char*> iTems1;
				iTems1.reserve(static_cast<int>(m_PParticleTypeList.size()));

				for (auto& str : m_PParticleTypeList)
					iTems1.push_back(str.c_str());

				if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc.Data._Effect_Tool_NoiseSamplerState_Flag, [](void* data, int idx, const char** out_text)
					{
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
					},
					(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
				{
					PackSamplerFlag(m_tCurrentDesc.Data._Effect_SamplerStateFlag, m_tCurrentDesc.Data._Effect_Tool_NoiseSamplerState_Flag, 3);
					m_bModified |= true;
				}

				ImGui::TreePop();
				ImGui::Spacing();
			}
			// Masking Texture##SamplerState
			if (ImGui::TreeNode("Masking Texture##SamplerState"))
			{
				vector<string> m_PParticleTypeList;
				m_PParticleTypeList.clear();

				m_PParticleTypeList.push_back("LinearSampler");
				m_PParticleTypeList.push_back("LinearClampSampler");
				m_PParticleTypeList.push_back("LinearBorderSampler");
				m_PParticleTypeList.push_back("LinearMirrorSampler");
				m_PParticleTypeList.push_back("PointSampler");

				std::vector<const char*> iTems1;
				iTems1.reserve(static_cast<int>(m_PParticleTypeList.size()));

				for (auto& str : m_PParticleTypeList)
					iTems1.push_back(str.c_str());

				if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc.Data._Effect_Tool_MaskingSamplerState_Flag, [](void* data, int idx, const char** out_text)
					{
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
					},
					(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
				{
					PackSamplerFlag(m_tCurrentDesc.Data._Effect_SamplerStateFlag, m_tCurrentDesc.Data._Effect_Tool_MaskingSamplerState_Flag, 6);
					m_bModified |= true;
				}

				ImGui::TreePop();
				ImGui::Spacing();
			}
			// Gradation Texture##SamplerState
			if (ImGui::TreeNode("Gradation Texture##SamplerState"))
			{
				vector<string> m_PParticleTypeList;
				m_PParticleTypeList.clear();

				m_PParticleTypeList.push_back("LinearSampler");
				m_PParticleTypeList.push_back("LinearClampSampler");
				m_PParticleTypeList.push_back("LinearBorderSampler");
				m_PParticleTypeList.push_back("LinearMirrorSampler");
				m_PParticleTypeList.push_back("PointSampler");

				std::vector<const char*> iTems1;
				iTems1.reserve(static_cast<int>(m_PParticleTypeList.size()));

				for (auto& str : m_PParticleTypeList)
					iTems1.push_back(str.c_str());

				if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc.Data._Effect_Tool_GradationSamplerState_Flag, [](void* data, int idx, const char** out_text)
					{
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
					},
					(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
				{
					PackSamplerFlag(m_tCurrentDesc.Data._Effect_SamplerStateFlag, m_tCurrentDesc.Data._Effect_Tool_GradationSamplerState_Flag, 9);
					m_bModified |= true;
				}

				ImGui::TreePop();
				ImGui::Spacing();
			}

			ImGui::TreePop();
		}

		// ===========   ParticleType Select   ===========
				// Render Mode
		if (ImGui::TreeNode("Particle Type"))
		{
			vector<string> m_PParticleTypeList;
			m_PParticleTypeList.clear();
			m_PParticleTypeList.push_back("NONE");
			m_PParticleTypeList.push_back("PARTICLE");
			m_PParticleTypeList.push_back("TEXTURE");
			m_PParticleTypeList.push_back("MESH");

			std::vector<const char*> iTems1;
			iTems1.reserve(static_cast<int>(m_PParticleTypeList.size()));

			for (auto& str : m_PParticleTypeList)
				iTems1.push_back(str.c_str());

			if (ImGui::ListBox("##ParticleType Select", &m_iSelcetedParticleTypeIdx, [](void* data, int idx, const char** out_text)
				{
					auto& vector = *static_cast<std::vector<std::string>*>(data);
					*out_text = vector[idx].c_str();
					return true;
				},
				(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
			{
				m_bModified |= true;
			}

			switch (m_iSelcetedParticleTypeIdx)
			{
			case (_uint)DTO::E_PARTICLETYPE::PARTICLE:
				m_tCurrentDesc.Data.eEffectParticleType = (_uint)DTO::E_PARTICLETYPE::PARTICLE; break;
			case (_uint)DTO::E_PARTICLETYPE::TEXTURE:
				m_tCurrentDesc.Data.eEffectParticleType = (_uint)DTO::E_PARTICLETYPE::TEXTURE; break;
			case (_uint)DTO::E_PARTICLETYPE::MESH:
				m_tCurrentDesc.Data.eEffectParticleType = (_uint)DTO::E_PARTICLETYPE::MESH;
				Make_MeshSelectButton();
				break;

			default:
				m_tCurrentDesc.Data.eEffectParticleType = (_uint)DTO::E_PARTICLETYPE::NONE;
				break;
			}

			// =============   어떤 ParticleType 선택중이니   ==============
			ImGui::Text("Current ParticleType: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.3f, 0.3f, 1.0f, 1.0f), "%s", m_PParticleTypeList[m_iSelcetedParticleTypeIdx].c_str());
			ImGui::Spacing();

			ImGui::Text("Current Model: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.7f, 1.0f), "%ls", m_tCurrentDesc.Data._Effect_Model_Tag.c_str());

			ImGui::TreePop();
			ImGui::Spacing();
		}

		// ===========   Shader List   ============
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Shader List"))
		{

			if (ImGui::ListBox("##Shader_List", &m_iSelectedShaderIdx, [](void* data, int idx, const char** out_text)
				{
					auto& vector = *static_cast<std::vector<std::pair<std::string, std::string>>*>(data);
					*out_text = vector[idx].second.c_str();
					return true;
				},
				(void*)&m_ShaderFileNames, (int)m_ShaderFileNames.size(), 5))
			{
				if (m_iSelectedShaderIdx < m_ShaderFileNames.size())
				{
					m_tCurrentDesc.Data._Effect_Shader_Path = Engine_Utils::ToWString(m_ShaderFileNames[m_iSelectedShaderIdx].first);
					m_tCurrentDesc.Data._Effect_Shader_Tag = Engine_Utils::ToWString(m_ShaderFileNames[m_iSelectedShaderIdx].second);
				}

				m_bModified |= true;
			}

			// =============   어떤 Shader 선택중이니   ==============
			ImGui::Text("Current Shader: "); ImGui::SameLine();

			if (!m_tCurrentDesc.Data._Effect_Shader_Tag.empty())
			{
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "%ls", m_tCurrentDesc.Data._Effect_Shader_Tag.c_str());
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "NONE");
			}
			ImGui::Spacing();

			if (ImGui::TreeNode("SHADER PASS LIST##RENDERER"))
			{
				vector<string> m_PParticleTypeList;
				m_PParticleTypeList.clear();

				switch (m_tCurrentDesc.Data.eEffectParticleType)
				{
				case (_uint)DTO::E_PARTICLETYPE::NONE:
					m_PParticleTypeList.push_back("NONE");
					break;
				case (_uint)DTO::E_PARTICLETYPE::PARTICLE:
					m_PParticleTypeList.push_back("DEFAULT_PARTICLE");
					m_PParticleTypeList.push_back("NONDEPTH_DEFAULT");
					break;
				case (_uint)DTO::E_PARTICLETYPE::TEXTURE:
					m_PParticleTypeList.push_back("DEFAULT_TEXTURE");
					m_PParticleTypeList.push_back("BLEND_TEXTURE");
					m_PParticleTypeList.push_back("BloomHard");
					m_PParticleTypeList.push_back("Blend_BloomHard");
					m_PParticleTypeList.push_back("NONDEPTH_DEFAULT");
					m_PParticleTypeList.push_back("Glow_Texture");
					m_PParticleTypeList.push_back("NONEDEPTH_Glow_Texture");
					break;
				case (_uint)DTO::E_PARTICLETYPE::MESH:
					m_PParticleTypeList.push_back("DEFAULT_MESH");
					m_PParticleTypeList.push_back("GLOW_MESH");
					m_PParticleTypeList.push_back("BloomHard");
					m_PParticleTypeList.push_back("DISTOTION");
					m_PParticleTypeList.push_back("SwordEffect");
					m_PParticleTypeList.push_back("NONDEPTH_DEFAULT");
					m_PParticleTypeList.push_back("CHANIN_MESH");
					break;
				}

				if (m_PParticleTypeList.size() <= m_iSelectedShaderPassIdx)
					m_iSelectedShaderPassIdx = 0/*static_cast<int>((m_PParticleTypeList.size() - 1))*/;

				std::vector<const char*> iTems1;
				iTems1.reserve(static_cast<int>(m_PParticleTypeList.size()));

				for (auto& str : m_PParticleTypeList)
					iTems1.push_back(str.c_str());

				if (ImGui::ListBox("##ParticleType Select", &m_iSelectedShaderPassIdx, [](void* data, int idx, const char** out_text)
					{
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
					},
					(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
				{
					m_bModified |= true;
					m_tCurrentDesc.Data._Effect_ShaderPass = (m_iSelectedShaderPassIdx);
				}

				// =============   어떤 Shader Pass 선택중이니   ==============
				ImGui::Text("Current Type: ##ShaderPassList"); ImGui::SameLine();
				int iTextIdx = (int)m_tCurrentDesc.Data.eEffectParticleType;

				if (iTextIdx > 0)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%s", m_PParticleTypeList[m_iSelectedShaderPassIdx].c_str());
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "NONE");
				}

				ImGui::TreePop();
				ImGui::Spacing();
			}
			ImGui::TreePop();
		}

	}

	ImGui::End();
}

void CParticle_System_Panel::Make_MeshSelectButton()
{
	if (ImGui::Button(" Load Mesh Data "))
	{
		OPENFILENAMEW ofn{};
		_tchar szFile[MAX_PATH] = { 0 };

		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"Mesh Files (*.mesh)\0*.mesh\0All Files (*.*)\0*.*\0\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (::GetOpenFileNameW(&ofn) == TRUE)
		{
			wstring result = szFile;
			// ParsingData
			if (result.find(L".mesh") != std::wstring::npos)
			{
				std::filesystem::path filePath(result);

				string MeshName = filePath.stem().string();
				m_tCurrentDesc.Data._Effect_Model_Tag = Engine_Utils::ToWString(MeshName);

				m_bModified |= true;
			}
		}
	}

}

void CParticle_System_Panel::Draw_Drop_Texture(CToolObject* pGo)
{

}

void CParticle_System_Panel::Draw_Rotation_Texture(CToolObject* pGo)
{
	ImGui::Begin("Texture_Setting##DropTexture");

	if (ImGui::TreeNodeEx("ROTATION SETTING", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("RotationTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			const char* textureNames[] = { "Diffuse", "Noise", "Masking", "Gradation", "Curve", "Normal", "Glow", "SubMasking"};
			const char* rotationLabels[] = { "0", "90", "180", "270" };

			for (int i = 0; i < 8; i++)
			{
				ImGui::TableNextColumn();
				ImGui::Text("%s", textureNames[i]);

				// 각 텍스처별로 콤보박스 배치
				std::string label = "##Rot" + std::to_string(i);
				if (ImGui::Combo(label.c_str(), &m_iRotIndices[i], rotationLabels, IM_ARRAYSIZE(rotationLabels)))
				{
					UpdateRotationFlags();
					m_bModified |= true;
				}
			}
			ImGui::EndTable();

			ImGui::Separator();

			// 하단에 Close 버튼 추가
			if (ImGui::Button("Close", ImVec2(120, 0)))
			{
				m_bShowRotationModal = false;
			}
		}
		ImGui::TreePop();
	}

	// Setting 하나 더
	Draw_Sprite_Texture(pGo);

	ImGui::End();
}

void CParticle_System_Panel::Draw_Sprite_Texture(CToolObject* pGo)
{
	ImGui::Spacing();
	ImGui::SeparatorText("TEXTURE SPRITE SETTINGS (Individual)");
	ImGui::Spacing();

	// -------------------------------------------------------------------------
	// pData.x : 0.0f이면 미사용, 1.0f이면 사용
	// pData.y : Column Count
	// pData.z : Row Count
	// pData.w : Playback Speed
	// -------------------------------------------------------------------------

	auto DrawSpriteSlotUI = [&](const char* label, Vec4& pData)
		{
			ImGui::PushID(label);

			if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 1. Sprite Mode 결정
				const char* items[] = { "None", "Fixed Index", "Animation" };
				int iMode = (int)pData.x;

				ImGui::Text("Sprite Mode"); ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::Combo("##SpriteMode", &iMode, items, IM_ARRAYSIZE(items)))
				{
					pData.x = (float)iMode;
					m_bModified = true;
				}

				if (iMode > 0)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
					float childHeight = (iMode == 2) ? 120.0f : 90.0f; // InputInt 사용 시 높이를 약간 더 확보
					ImGui::BeginChild("Settings", ImVec2(0, childHeight), true);

					int iCol = (int)pData.y;
					int iRow = (int)pData.z;

					// --- Grid Columns (DragInt -> InputInt로 교체) ---
					ImGui::Columns(2, nullptr, false);
					ImGui::Text("Grid Columns"); ImGui::NextColumn();
					ImGui::SetNextItemWidth(-1);
					// InputInt는 +, - 버튼을 생성하며 직접 입력도 가능합니다.
					if (ImGui::InputInt("##Cols", &iCol, 1)) // 1씩 증가/감소 스텝 설정
					{
						if (iCol < 1) iCol = 1; if (iCol > 100) iCol = 100; // 범위 제한
						pData.y = (float)iCol;
						m_bModified = true;
					}
					ImGui::NextColumn();

					// --- Grid Rows (DragInt -> InputInt로 교체) ---
					ImGui::Text("Grid Rows"); ImGui::NextColumn();
					ImGui::SetNextItemWidth(-1);
					if (ImGui::InputInt("##Rows", &iRow, 1))
					{
						if (iRow < 1) iRow = 1; if (iRow > 100) iRow = 100;
						pData.z = (float)iRow;
						m_bModified = true;
					}
					ImGui::Columns(1);

					ImGui::Separator();

					// 3. 상황별 설정 (w)
					if (iMode == 1) // Fixed Index 모드
					{
						int iFixedIdx = (int)pData.w;
						int iMaxIdx = (iCol * iRow) - 1;
						ImGui::Text("Target Index"); ImGui::SameLine();
						ImGui::SetNextItemWidth(-1);
						// Slider 대신 InputInt 사용 (인덱스 조절용)
						if (ImGui::InputInt("##Index", &iFixedIdx, 1))
						{
							if (iFixedIdx < 0) iFixedIdx = 0;
							if (iFixedIdx > iMaxIdx) iFixedIdx = max(0, iMaxIdx);
							pData.w = (float)iFixedIdx;
							m_bModified = true;
						}
					}
					else if (iMode == 2) // Animation 모드
					{
						ImGui::Text("Anim Speed"); ImGui::SameLine();
						ImGui::SetNextItemWidth(-1);
						// 속도의 경우 소수점 단위 조절이 필요하므로 InputFloat 사용
						if (ImGui::InputFloat("##Speed", &pData.w, 0.05f, 0.5f, "%.2f x"))
						{
							if (pData.w < 0.f) pData.w = 0.f;
							m_bModified = true;
						}
					}

					ImGui::EndChild();
					ImGui::PopStyleVar();
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
			ImGui::Spacing();
		};

	// Diffuse
	DrawSpriteSlotUI("Diffuse Texture Sprite", m_tCurrentDesc.Data._Effect_DiffuseTexture_SpriteInfo);

	// Noise
	DrawSpriteSlotUI("Noise Texture Sprite", m_tCurrentDesc.Data._Effect_NoiseTexture_SpriteInfo);

	// Gradation
	DrawSpriteSlotUI("Gradation Texture Sprite", m_tCurrentDesc.Data._Effect_GradationTexture_SpriteInfo);

	// Dissolve
	DrawSpriteSlotUI("Dissolve Texture Sprite", m_tCurrentDesc.Data._Effect_DissolveTexture_SpriteInfo);

	// Glow
	DrawSpriteSlotUI("Glow Texture Sprite", m_tCurrentDesc.Data._Effect_GlowTexture_SpriteInfo);

	// Mask
	DrawSpriteSlotUI("Mask Texture Sprite", m_tCurrentDesc.Data._Effect_MaskTexture_SpriteInfo);

	// Curve
	DrawSpriteSlotUI("Curve Texture Sprite", m_tCurrentDesc.Data._Effect_CurveTexture_SpriteInfo);

	// SubMask
	DrawSpriteSlotUI("SubMask Texture Sprite", m_tCurrentDesc.Data._Effect_SubMaskTexture_SpriteInfo);

	// 닫기 버튼
	if (ImGui::Button("Close Modal", ImVec2(ImGui::GetContentRegionAvail().x, 40)))
	{
		m_bShowRotationModal = false;
	}
}

void CParticle_System_Panel::UpdateRotationFlags()
{
	uint32_t packed = 0;
	for (int i = 0; i < 6; i++)
	{
		packed |= (static_cast<uint32_t>(m_iRotIndices[i]) << (i * 4));
	}
	m_tCurrentDesc.Data._Effect_TextureRotationFlag = packed;
}

void CParticle_System_Panel::Draw_Preview_Texture(CToolObject* pGo)
{
	ImGui::Separator();
	ImGui::Text("Currently Bound Textures");

	if (ImGui::BeginChild("TexturePreview", ImVec2(0, 100), true, ImGuiWindowFlags_HorizontalScrollbar))
	{
		const char* SlotName[] = { "Diffuse", "Noise", "Mask", "Gradation", "Curve", "Normal" };

		for (int i = 0; i < 6; i++)
		{
			ImGui::BeginGroup();

			ImGui::Text("%s", SlotName[i]);

			// 텍스처 SRV 바인딩
			CToolObject* pInstance = static_cast<Effect*>(pGo)->Get_Part<CToolObject>(m_iSelectPartsIndex);
			CTextureBase* pTextureCom = m_pGameInstance->Get_Resource<CTextureBase>(L"");
				

			if (pTextureCom)
			{
				ID3D11ShaderResourceView* pSRV = pTextureCom->Get_SRV();
			}
		}
	}
}

void CParticle_System_Panel::Draw_EffectColor(CToolObject* pGo)
{
	ImGui::Begin("Effect Settings");

	ImGui::Text("Particle Color Settings");

	m_bModified |= ImGui::ColorPicker4("##MyEffectPicker", (float*)&m_tCurrentDesc.Data._Effect_Color,
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_AlphaBar |       // 투명도 조절 바 표시
		ImGuiColorEditFlags_DisplayRGB);

	// HDR 선택도 할 수 있는 창을 만들자.

	ImGui::End();
}

void CParticle_System_Panel::Draw_Parts(CToolObject* pGo)
{
	ImGui::Begin("Effect Parts Setting");

	Effect* pInstance = static_cast<Effect*>(pGo);
	if (pInstance == nullptr) { ImGui::End(); return; }

	auto& PartsList = pInstance->Get_PartList();

	// 삭제 등으로 인덱스가 범위를 벗어났을 때만 보정
	if (m_iSelectPartsIndex >= (int)PartsList.size())
		m_iSelectPartsIndex = max(0, (int)PartsList.size() - 1);

	int iCurrentIdx = m_iSelectPartsIndex;

	if (ImGui::TreeNode("Effect_PartsList##Effect_PartsList"))
	{
		std::vector<const char*> iTems;
		iTems.reserve(PartsList.size());

		for (auto& parts : PartsList)
		{
			if (parts)
				iTems.push_back(parts->Get_Name().c_str());
		}

		if (ImGui::ListBox("##PartsSelector", &iCurrentIdx, iTems.data(), (int)iTems.size(), 6))
		{
			m_iSelectPartsIndex = iCurrentIdx;
			m_bModified = true;

			CEffectObject* pSelectdPart = nullptr;
			
			// ===========================  타입 검사  =================================

			if (dynamic_cast<CEffectObject*>(pInstance->Get_Part<CEffectObject>(m_iSelectPartsIndex)))
			{
				m_iSelectPartsType = ENUM_TO_UINT(E_PartsObjectID::Effect_Particle);
			}

			else if (dynamic_cast<CGravity_Force*>(pInstance->Get_Part<CGravity_Force>(m_iSelectPartsIndex)))
			{
				m_iSelectPartsType = ENUM_TO_UINT(E_PartsObjectID::Effect_ForceField);
			}
				
			
			if (pInstance->Get_Part<CEffectObject>(m_iSelectPartsIndex))
			{
				pSelectdPart =  pInstance->Get_Part<CEffectObject>(m_iSelectPartsIndex);
				// 구조체 복사하기
				m_tCurrentDesc.Data = pSelectdPart->Get_EffectDesc();

				// 툴에서 사용하는 인덱스 변수들을 구조체 값에 맞춰서 복사하기.
				m_iSelectedEmissionIdx = (int)m_tCurrentDesc.Data._Effect_EmissionType;
				m_iSelectedShaderIdx = (int)m_tCurrentDesc.Data._Effect_ShapeType;
				m_iSelcetedParticleTypeIdx = (int)m_tCurrentDesc.Data.eEffectParticleType;
				m_iSelectedShaderPassIdx = m_tCurrentDesc.Data._Effect_ShaderPass;
			}
		}

		if (!iTems.empty())
		{
			ImGui::Text("Current Part: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "%s", iTems[m_iSelectPartsIndex]);
		}
		ImGui::Spacing();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Edit##Part_Setting"))
	{
		if (ImGui::TreeNode("Create##Part_Setting"))
		{
			static int iSelectedPartType = 0; // 0: NONE, 1: Particle, 2: ForceField
			const char* pPartTypeNames[] = { "NONE (Select Type!)", "Effect_Particle", "Effect_ForceField" };

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Select Part Type");
			ImGui::Combo("##PartTypeCombo", &iSelectedPartType, pPartTypeNames, IM_ARRAYSIZE(pPartTypeNames));
			ImGui::Spacing();

			static char nameBuf[128] = {};
			if (nameBuf[0] == '\0') strncpy_s(nameBuf, "NEW_PARTS", _TRUNCATE);

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Set Part Name");
			ImGui::InputText("##CreateName_PartSetting", nameBuf, IM_ARRAYSIZE(nameBuf));
			ImGui::Spacing();

			if (iSelectedPartType == 0)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
				ImGui::Button("Select Type First");
				ImGui::PopStyleColor();
				ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Warning: Part type must not be NONE.");
			}
			else
			{
				if (ImGui::Button("Create##Part_ObjectSetting", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				{
					CEffectObject::Effect_Desc pEffectDesc = {};
					CTransform::TRANSFORM_DESC transformDesc = {};
					transformDesc.fRotatePerSec = 1.f;
					transformDesc.fMovePerSec = 1.f;

					pEffectDesc.pMatParent = &(pGo->Get_Component<CTransform>()->Get_WorldMatrix());
					pEffectDesc.pTransform_Desc = &transformDesc;
					pEffectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::EFFECT);

					*(E_PartsObjectID*)&pEffectDesc.iPartsID = (E_PartsObjectID)iSelectedPartType;

					// 타입에 따른 기본 설정 분기
					if ((E_PartsObjectID)iSelectedPartType == E_PartsObjectID::Effect_Particle)
					{
						pEffectDesc.wstrLayerTag = L"Effect_Part_Particles";
						pEffectDesc.iPartsID = E_PartsObjectID::Effect_Particle;
					}

					else if ((E_PartsObjectID)iSelectedPartType == E_PartsObjectID::Effect_ForceField)
					{
						pEffectDesc.wstrLayerTag = L"Effect_Part_ForceField";
						pEffectDesc.iPartsID = E_PartsObjectID::Effect_ForceField;
					}

					else
						pEffectDesc.Data.eEffectType = (_uint)DTO::E_EFFECTTYPE::NONE;

					pEffectDesc.Data._Effect_Color = Vec4{ 0.f, 0.f, 0.f, 1.f };
					pEffectDesc.Data._Effect_StartScale = { 1.f, 1.f, 1.f };
					pEffectDesc.Data._Effect_EndScale = { 1.f, 1.f, 1.f };

					// ========     셰이더 선택   =========
					pEffectDesc.Data._Effect_Shader_Tag = L"Shader_VtxEffectParticle";
					pEffectDesc.Data._Effect_ShaderPass = 0;

					// ========     이펙트 타입   =========
					pEffectDesc.Data.eEffectSystemType = (_uint)DTO::E_EffectSystemType::Particle;
					pEffectDesc.Data.eEffectType = (_uint)DTO::E_EFFECTTYPE::Particle;
					pEffectDesc.Data.eEffectSystemType = (_uint)DTO::E_EffectSystemType::Particle;
					pEffectDesc.Data._Effect_EmissionType = (_uint)DTO::E_EMISSION_TYPE::BOX;
					pEffectDesc.Data._Effect_ShapeType = (_uint)DTO::E_SHAPETYPE::SPREAD;


					// 파츠 추가
					if((E_PartsObjectID)iSelectedPartType == E_PartsObjectID::Effect_Particle)
						pInstance->Add_Part(static_cast<_uint>(PartsList.size()), ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect_Part_Particle", &pEffectDesc);

					else if((E_PartsObjectID)iSelectedPartType == E_PartsObjectID::Effect_ForceField)
						pInstance->Add_Part(static_cast<_uint>(PartsList.size()), ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect_Part_ForceField", &pEffectDesc);

					Tool_PartObject* pNewPart = nullptr;

					if (auto* pEffectParticle = pInstance->Get_Part<CEffectObject>((_uint)PartsList.size() - 1))
					{
						pNewPart = pInstance->Get_Part<CEffectObject>((_uint)PartsList.size() - 1);
					}

					else if (auto* pGravity = pInstance->Get_Part<CGravity_Force>((_uint)PartsList.size() - 1))
					{
						pNewPart = pInstance->Get_Part<CGravity_Force>((_uint)PartsList.size() - 1);
					}
	
					if (pNewPart)
					{
						pNewPart->Set_Name(nameBuf);
						pNewPart->Awake(ENUM_TO_UINT(ELevelType::EFFECT));
						m_iSelectPartsIndex = (int)PartsList.size() - 1; 
						m_iSelectPartsType = iSelectedPartType;
					}
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Name Change##Part_Setting"))
		{
			static char nameBuf[128] = {};
			string effectName = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(m_iSelectPartsIndex)->Get_Name();

			if (nameBuf[0] == '\0' && !effectName.empty())
			{
				std::string tmp = effectName;
				strncpy_s(nameBuf, sizeof(nameBuf), tmp.c_str(), _TRUNCATE);
			}
			// ImGui에서 입력 받기
			ImGui::InputText("##ChangeName_PartSetting", nameBuf, IM_ARRAYSIZE(nameBuf)); ImGui::SameLine();

			if (ImGui::Button("SAVE"))
			{
				effectName = nameBuf;
				static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(m_iSelectPartsIndex)->Set_Name(effectName);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Delete##Part_Setting"))
		{
			if (ImGui::Button("Delete##Part_SettingButton"))
			{
				static_cast<Effect*>(pGo)->Remove_Part(m_iSelectPartsIndex);

				m_iSelectPartsIndex--;
				if (m_iSelectPartsIndex <= 0)
					m_iSelectPartsIndex = 0;
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	
	ImGui::End();
	
}

void CParticle_System_Panel::Draw_TextureSelectorPopup(const char* popupID, wstring& outTag)
{
	static char filterBuf[128] = ""; // 검색용 버퍼
	ImGui::SetNextWindowSize(ImVec2(700, 550), ImGuiCond_Appearing);

	if (ImGui::BeginPopupModal(popupID, NULL))
	{
		// --- 상단 검색바 ---
		ImGui::Text("Search:"); ImGui::SameLine();
		ImGui::InputText("##TextureSearch", filterBuf, IM_ARRAYSIZE(filterBuf));
		ImGui::Separator();

		// --- 좌측 폴더 리스트 ---
		ImGui::BeginChild("FolderList", ImVec2(180, 0), true);
		for (auto& folderName : m_TextureFolderNames)
		{
			if (ImGui::Selectable(folderName.c_str(), m_strSelectedFolder == folderName))
				m_strSelectedFolder = folderName;
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// --- 우측 텍스처 그리드 ---
		ImGui::BeginChild("TextureGrid", ImVec2(0, 0), true);
		if (!m_strSelectedFolder.empty() && m_TextureMap.count(m_strSelectedFolder))
		{
			auto& fileList = m_TextureMap[m_strSelectedFolder];
			string searchFilter = filterBuf;
			int columns = 5;
			int drawIdx = 0;

			for (auto& filePair : fileList)
			{
				string fileName = filePair.second;

				// [이름 검색 로직] 필터어가 포함된 경우만 출력
				if (!searchFilter.empty() && fileName.find(searchFilter) == string::npos)
					continue;

				wstring textureTag = L"Texture_" + Engine_Utils::ToWString(fileName);
				CTextureBase* pTexture = m_pGameInstance->Get_Resource<CTextureBase>(textureTag);
				ID3D11ShaderResourceView* pSRV = (pTexture) ? pTexture->Get_SRV() : nullptr;

				ImGui::PushID(drawIdx++);
				ImGui::BeginGroup();
				if (pSRV)
				{
					if (ImGui::ImageButton("##texBtn", (ImTextureID)pSRV, ImVec2(64, 64)))
					{
						outTag = Engine_Utils::ToWString(fileName);
						m_bModified = true;
						filterBuf[0] = '\0'; // 선택 시 검색어 초기화
						ImGui::CloseCurrentPopup();
					}
				}
				else { ImGui::Button("No Res", ImVec2(64, 64)); }

				string display = (fileName.length() > 10) ? fileName.substr(0, 8) + ".." : fileName;
				ImGui::Text(display.c_str());
				ImGui::EndGroup();
				ImGui::PopID();

				if ((drawIdx) % columns != 0) ImGui::SameLine(0, 10.f);
				Safe_Release(pTexture);
			}
		}
		ImGui::EndChild();

		if (ImGui::Button("Close", ImVec2(120, 0))) { filterBuf[0] = '\0'; ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void CParticle_System_Panel::Draw_ForceField_Window(CToolObject* pGo)
{
	ImGui::Begin("Gravity Force");

	ImGui::Separator();
	// 트랜스폼 - 파티클 시스템 위치 옮기기 (부모 위치 옮기기)

	// ==================//
   //    Gravity Force  //
  // ==================//

	if (ImGui::CollapsingHeader("Gravity Force##GravityForce"))
	{
		if (ImGui::TreeNode("Transform##Gravity Force"))
		{
			// 어차피 gizmo 연동이 되어있음.
			CTransform* pTranform = static_cast<Effect*>(pGo)->Get_Part<CGravity_Force>(m_iSelectPartsIndex)->Get_Component<CTransform>();
			Vec3 Position = pTranform->Get_Info(TRANSFORM_INFO_STATE::POS);
			Vec3 Scale = pTranform->Get_Scaled();

			// ======== 위젯 정렬 함수 =========
			float fStartPos = 80.f;

			// Position 
			ImGui::Text("Pos : "); ImGui::SameLine(fStartPos); ImGui::SetNextItemWidth(-1.f);

			if(ImGui::DragFloat3(" ##Transform_Gravity Force_Position", &Position.x, 0.1f))
				pTranform->Set_Info(TRANSFORM_INFO_STATE::POS, Position);

			// Scale
			ImGui::Text("Scale : "); ImGui::SameLine(fStartPos); ImGui::SetNextItemWidth(-1.f);

			if (ImGui::DragFloat3(" ##Transform_Gravity Force_Scale", &Scale.x, 0.1f, 0.0001f, 1000.f))
			{
				if (Scale.x < 0.001f) Scale.x = 0.001f;
				if (Scale.y < 0.001f) Scale.y = 0.001f;
				if (Scale.z < 0.001f) Scale.z = 0.001f;
				pTranform->Set_Scale(Scale);
			}

	
			// 출력
			ImGui::Separator();
			ImGui::Text("ObjectPos : (%g, %g, %g)", Position.x, Position.y, Position.z);
			ImGui::Text("ObjectScale : (%g, %g, %g)", Scale.x, Scale.y, Scale.z);

			ImGui::TreePop();
		}
	}

	ImGui::End();
}


HRESULT CParticle_System_Panel::EffectPanel_Initialize()
{
	if (FAILED(EffectFileResource_Setting()))
		return E_FAIL;

	if (FAILED(Create_CanvasEffect()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_System_Panel::EffectFileResource_Setting()
{
	ResourceFolderSearch(E_EFFECT_RESOURCETYPE::TEXTURE, m_sTextureFolderPath);
	ResourceFileSearch(E_EFFECT_RESOURCETYPE::TEXTURE, m_sTextureFolderPath);
	ResourceFileSearch(E_EFFECT_RESOURCETYPE::SHADER, m_sShaderFolderPath);
	return S_OK;
}

HRESULT CParticle_System_Panel::ResourceFolderSearch(E_EFFECT_RESOURCETYPE eType, const string& RootPath)
{
	// 기존 데이터 초기화 (필요시)
	m_TextureMap.clear();
	m_TextureFolderNames.clear();

	namespace fs = std::filesystem;

	if (!fs::exists(RootPath)) return E_FAIL;

	for (auto& iter : fs::recursive_directory_iterator(RootPath))
	{
		// 파일인 경우에만 처리
		if (iter.is_regular_file())
		{
			auto fullPath = iter.path();
			string strFullPath = Engine_Utils::ToString(fullPath);
			string fileName = fullPath.filename().string();
			string pureFileName = Engine_Utils::GetFileNameWithoutExtension(strFullPath);

			// 파일이 속한 바로 위 폴더 이름 추출
			string folderName = fullPath.parent_path().filename().string();

			string ext = fullPath.extension().string();
			for (auto& c : ext) c = tolower(c);

			if (ext == ".png" || ext == ".dds" || ext == ".tga" || ext == ".jpg")
			{
				m_TextureMap[folderName].push_back(make_pair(strFullPath, pureFileName));

				if (find(m_TextureFolderNames.begin(), m_TextureFolderNames.end(), folderName) == m_TextureFolderNames.end())
				{
					m_TextureFolderNames.push_back(folderName);
				}
			}
		}
	}

	return S_OK;
}

HRESULT CParticle_System_Panel::ResourceFileSearch(E_EFFECT_RESOURCETYPE eType, const string& Path)
{
	for (auto iter = std::filesystem::recursive_directory_iterator(Path);
		iter != std::filesystem::recursive_directory_iterator();
		++iter)
	{
		// 아 여기에 화살표 그으면 되는구나.

		int depth = iter.depth();
		auto fullpath = iter->path();
		auto FolderName = iter->path().filename();

		if (iter->is_regular_file())
		{
			string m_sFileName = Engine_Utils::GetFileNameWithoutExtension(Engine_Utils::ToString(fullpath));

			switch (eType)
			{
				case E_EFFECT_RESOURCETYPE::TEXTURE:
					m_TextureFileNames.push_back(make_pair(Engine_Utils::ToString(fullpath), m_sFileName)); break;

				case E_EFFECT_RESOURCETYPE::SHADER:
					m_ShaderFileNames.push_back(make_pair(Engine_Utils::ToString(fullpath), m_sFileName)); break;
			}
		}
	}

	return S_OK;
}

CParticle_System_Panel* CParticle_System_Panel::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CParticle_System_Panel* pInstance = new CParticle_System_Panel(pLabel, pOwner, pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : Particle_System_Panel");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->EffectPanel_Initialize()))
	{
		MSG_BOX("Initialize Fail : Particle_System Panel");
		return nullptr;
	}

	return pInstance;
}


void CParticle_System_Panel::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	//Safe_Release(m_pCanvasEffectObject);

	//// Preview Texture용 이펙트 객체 삭제
	//Safe_Release(m_pPreviewRTV);
	//Safe_Release(m_pPreviewSRV);
}
