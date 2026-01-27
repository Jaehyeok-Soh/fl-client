#include "pch.h"
#include "CParticle_System_Panel.h"
#include "Engine_Utils.h"
#include "Transform.h"
#include "Effect.h"
#include "CEffectObject.h"
#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"

CParticle_System_Panel::CParticle_System_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CParticle_System_Panel::Update(const _float fDT)
{
	if (m_bTimeSetting)
	{
	// === 시간 계산 ===ww
		Time_Calculator(fDT * m_tCurrentDesc._Effect_PlayBackSpeed);
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
	Draw_ParticleSystem(pGo);
	Draw_Timer(pGo);
	Draw_EffectColor(pGo);

	if(m_bModified)
		Binding_EffectDesc(pGo);

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
			static_cast<CEffectObject*>(Part)->TimePause(false);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Pause"))
	{
		m_bTimeSetting = false;

		auto EffectPartList = static_cast<Effect*>(pGo)->Get_PartList();
		for (auto Part : EffectPartList)
		{
			static_cast<CEffectObject*>(Part)->TimePause(true);
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
			static_cast<CEffectObject*>(Part)->TimePause(false);
			static_cast<CEffectObject*>(Part)->TimeReset();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		m_bTimeSetting = false;
		m_tCurrentDesc._Effect_TimeStop = true;
		m_fTimeAccumulation = 0.f;

		// 일단은 임시로.
		auto EffectPartList = static_cast<Effect*>(pGo)->Get_PartList();
		for (auto Part : EffectPartList)
		{
			Part->Get_Component<CVIBuffer_Particle_Point>()->Reset_Simulation();
			static_cast<CEffectObject*>(Part)->TimePause(true);
			static_cast<CEffectObject*>(Part)->TimeReset();
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	//  ==================   Playback Speed   ====================== 

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Playback Speed");
	ImGui::Spacing();
	m_bModified |= ImGui::DragFloat("##PlaybackSpeed", &m_tCurrentDesc._Effect_PlayBackSpeed, 0.1f, 0.f, 100.f);
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
		m_bModified |= ImGui::InputFloat("##Duration1", &m_tCurrentDesc._Effect_Duration, 0.f);
		ImGui::Spacing();

		// ===================   Looping - 재생이 끝나면 반복할지 결정한다		=====================
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Looping");
		ImGui::SameLine();
		m_bModified |= ImGui::Checkbox("##Looping1", &m_tCurrentDesc._Effect_Looping);
		ImGui::Spacing();

		// ==================   Prewarm - 체크하면 게임 시작시 이미 시스템이 헌 서아쿨아 돌아간 것처럼 미리 입자가 퍼져있다.  =========================
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Prewarm");
		ImGui::SameLine();
		static bool Prewarm = false;
		m_bModified |= ImGui::Checkbox("##Prewarm1", &Prewarm);
		ImGui::Spacing();

		// =================   Start Delay - 재생 버튼을 누르고 실제 입자가 나오기까지 걸리는 시간   =========================
		ImGui::AlignTextToFramePadding();
		if(ImGui::TreeNode("Particle Time Setting##ParticleSystem"))
		{
			ImGui::Text("StartDelay"); ImGui::SameLine();
			m_bModified |= ImGui::InputFloat("##StartDelay1", &m_tCurrentDesc._Effect_StartDelay, 0.f);
			ImGui::Text("StartLifeTime"); ImGui::SameLine();
			m_bModified |= ImGui::InputFloat("##StartLifeTime1", &m_tCurrentDesc._Effect_LifeTime, 0.f);
			ImGui::Text("StartSpeed"); ImGui::SameLine();
			m_bModified |= ImGui::InputFloat("##StartSpeed1", &m_tCurrentDesc._Effect_StartSpeed, 0.f);

			ImGui::TreePop();
		}

		ImGui::AlignTextToFramePadding();
		if(ImGui::TreeNode("Auto Random Seed"))
		{
			m_bModified |= ImGui::Checkbox("Auto Random Seed##ParticleSystem", &m_tCurrentDesc._Effect_IsRandomSeed);

			ImGui::TreePop();
		}

		// ==============  Start Size - 입자의 초기 사이즈   =========================================
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Size Setting"))
		{
			ImGui::SeparatorText("Start Size##Size Setting");
			m_bModified |= ImGui::DragFloat3("##Start Size Setting", &m_tCurrentDesc._Effect_StartScale.x, 0.1f, 0.1f, 100.f);

			ImGui::SeparatorText("End Size##Size Setting");
			m_bModified |= ImGui::DragFloat3("##End Size Setting", &m_tCurrentDesc._Effect_EndScale.x, 0.1f, 0.1f, 100.f);

			ImGui::TreePop();
		}
		// ==============   3D Start Rotation - 입자의 축마다의 회전값    =========================

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("3D Start Rotataion"))
		{
			static bool StartRotation_3D = false;
			m_bModified |= ImGui::Checkbox("##StartRotation_3D", &StartRotation_3D); ImGui::Spacing();

			if (StartRotation_3D)
			{
				static Vec3 StartRotation = { 0.f, 0.f, 0.f };

				ImGui::SeparatorText("Rotation 3D Convert To Radian - X Y Z - ");
				m_bModified |= ImGui::DragFloat3("##StartRotationX_3D", &StartRotation.x, 0.1f, 0.f, 360.f); ImGui::Spacing();

				CEffectObject* pInstance = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(m_iSelectPartsIndex);
				CTransform* pTransform = pInstance->Get_Component<CTransform>();

				if (pTransform)
					pTransform->Rotation(
						DirectX::XMConvertToRadians(StartRotation.x),
						DirectX::XMConvertToRadians(StartRotation.y),
						DirectX::XMConvertToRadians(StartRotation.z));
			}
			ImGui::TreePop();
		}
			// Gravity Source - 중력이 어디서 오는지 결정한다. 물리 상수 (9.8ms^2)
			// Gravity Modifier - 중력의 영향을 얼마나 강하게 받을지 정하는 배수. 0이면 무중력, 1이면 설정된 중력만큼 아래로 떨어진다.


			// Simulation Space - 파티클의 좌표계를 결정한다.
				// 1. Local - 부모가 움직이면 이미 생성된 입자도 부모를 따라 움직인다.
				// 2. World - 생성된 후에는 부모를 떠나 독립된 월드 좌표에서 움직입니다.

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Simulation Space"))
		{
			ImGui::Text("Local"); ImGui::SameLine(0, 20.f);
			ImGui::Text("World"); ImGui::Spacing();
			static bool Local = false;
			static bool World = false;

			m_bModified |= ImGui::Checkbox("##Local1", &Local); ImGui::SameLine(0, 40.f);
			m_bModified |= ImGui::Checkbox("##World1", &World); ImGui::Spacing();

			if (Local) m_tCurrentDesc._Effect_SimulationType = E_SIMULATION_SPACE::LOCAL;
			else if (World) m_tCurrentDesc._Effect_SimulationType = E_SIMULATION_SPACE::WORLD;

			ImGui::TreePop();
		}
	
		// Simulation Speed - 전체 이펙트의 재생 속도 배수입니다. 2라면 2배속으로 빠르게 움직입니다.
		// Delta Time - 시간 계산 방식을 정합니다. 게임 속도에 맞춰진 Scaled 시간인가?
		// Scaling Mode - 부모 오브젝트의 크기가 변할 때 파티클이 어떻게 반응할지 결정
// 
		// Play On Awake* - 파티클 시스템이 씬에 생성되자마자 즉시 재생될지 여부를 결정한다.
		// Emitter Velocity Mode - 파티클이 방출될 때, 본체의 속도를 어떻게 계산할지 정합니다.
		// Max Particles = 동시에 존재할 수 있는 최대 입자 개수. 이 값만큼 iNSTANCE bUFFER의 크기를 잡아두면 성능상 좋음

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Max Particles##Particle System"))
		{
			m_bModified |= ImGui::InputInt("##MaxParticles", &m_tCurrentDesc._Effect_MaxParticle, 0);
			ImGui::TreePop();
		}

		// Auto Random Seed - 난수 Seed를 다르게 생성해서 입자 모양이 랜덤하게 변한다.
		// Stop Action - 이펙트가 완전히 끝났을 때 해당 오브젝트를 어떻게 처리할지 정한다.
		// Culling Mode - 카메라 화면 밖에 있을 때 연산을 중단할지 여부를 결정해서 성능을 최적화합니다.
		// Ring Buffer Mode - 최대 입자 개수가 찼을 때, 가장 오래된 입자를 지우고 새 입자를 만들지 결정한다.
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
			m_bModified |= ImGui::InputFloat("##RateOverTime", &m_tCurrentDesc._Effect_RateOverTime, 0.f);
			ImGui::TreePop();
		}
		// Rate Over Distance - 본체가 이동한 거리에 비례해서 입자를 뿜습니다. 
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Rate Over Distance"))
		{
			m_bModified |= ImGui::InputFloat("##RateOverDistance", &m_tCurrentDesc._Effect_RateOverTime, 0.f);
			ImGui::TreePop();
		}
		// Bursts - 특정 시간에 파티클을 한꺼번에 '확' 터뜨리는 기능입니다.
			// Time - 시스템 시작 후 몇 초 뒤에 터뜨릴지 정합니다. 
			// Count - 그 순간 한꺼번에 생성할 입자의 개수입니다.
			// Cycles - 이 작업을 몇 번 반복할지 정합니다.
			// Interval : 반복할 때 ㅅ이의 시간 간격입니다.
			// Probability - 해당 폭발이 일어날 확률(0~1)
	}

	// ==================//
   //       Shape       //
  // ==================//

	if (ImGui::CollapsingHeader("Shape"))
	{
		static int ShapeEffectNumber = {};
		if (ImGui::TreeNode("Shape_EffectList##Effect_List"))
		{
			vector<string> m_pShapeList;
			m_pShapeList.clear();

			m_pShapeList.push_back("NONE");
			m_pShapeList.push_back("SPREAD");
			m_pShapeList.push_back("DROP");
			m_pShapeList.push_back("RISE");
			m_pShapeList.push_back("MESH");
			m_pShapeList.push_back("STRAIGHT");

			std::vector<const char*> iTems;
			iTems.reserve(static_cast<int>(m_pShapeList.size()));

			for (auto& str : m_pShapeList)
				iTems.push_back(str.c_str());

			m_bModified |= ImGui::ListBox("", &ShapeEffectNumber, iTems.data(), static_cast<int>(m_pShapeList.size()), 6);

			if ((iTems.size() - 1) < ShapeEffectNumber)
			{
				ImGui::TreePop();
				return;
			}
			ImGui::TreePop();
		}

		switch (ShapeEffectNumber){
		case 0: m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::NONE; break;
		case 1: m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::SPREAD; break;
		case 2: m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::DROP; break;
		case 3: m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::RISE; break;
		case 4: m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::MESH; break;
		case 5: m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::STRAIGHT; break; }

		// ============   Distortion   Scale 설정하기   ===========
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Distortion Scale##Shape"))
		{
			m_bModified |= ImGui::DragFloat2("##Distortion Scale", &m_tCurrentDesc._Effect_DistortionScale.x, 0.1f, 0.1f, 100.f);
			ImGui::TreePop();
		}

		// ============   Scroll 값 설정하기   ============
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Scroll Speed##Shape"))
		{
			m_bModified |= ImGui::DragFloat2("##Scroll Speed", &m_tCurrentDesc._Effect_ScrollSpeed.x, 0.1f, -100.f, 100.f);
			ImGui::TreePop();
		}

		// ============  Discard Value 설정하기   ==========
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Discard Value##Shape"))
		{
			m_bModified |= ImGui::DragFloat("##Discard Value", &m_tCurrentDesc._Effect_DiscardValue, 0.01f, 0.f, 1.f);
			ImGui::TreePop();
		}

		// ============   Radius   ============= 
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Range"))
		{
			m_bModified |= ImGui::DragFloat3("##Range", &m_tCurrentDesc._Effect_Range.x, 0.1f, 0.1f, 100.f);
			ImGui::TreePop();
		}

		// ===========  size 변경 
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Particle Size ##Shape"))
		{
			m_bModified |= ImGui::DragFloat2("Particle Size", &m_tCurrentDesc._Effect_ParticleSize.x, 0.01f, 0.0f, 100.f, "%.2f");
			

			ImGui::TreePop();
		}

#pragma region DIFFUSE_TEXTURE
		// ===========   Diffse Texture  ============
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Diffuse Texture"); ImGui::SameLine();

		if (ImGui::Button("Open Texture Selector##Diffuse_Texture"))
			ImGui::OpenPopup("TextureSelector##Diffuse_Texture");

		ImGui::SameLine();
		// ============ Diffuse Use check box =============
		if (ImGui::Checkbox("Use##DefaultTexture", &m_tCurrentDesc._Effect_Tool_DiffuseTexture))
		{
			if (m_tCurrentDesc._Effect_Tool_DiffuseTexture)
				Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_TextureFlag, 1);

			else
				Engine_Utils::RemoveHard_Flag(m_tCurrentDesc._Effect_TextureFlag, 1);

			m_bModified |= true;
		}

		ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Appearing);

		if (ImGui::BeginPopupModal("TextureSelector##Diffuse_Texture", NULL))
		{
			ImGui::BeginChild("FolderList", ImVec2(180, 0), true);
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Category");
				ImGui::Separator();

				for (auto& folderName : m_TextureFolderNames)
				{
					// 현재 선택된 폴더면 하이라이트 효과
					if (ImGui::Selectable(folderName.c_str(), m_strSelectedFolder == folderName))
					{
						m_strSelectedFolder = folderName;
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			// --- 우측 : 선택된 폴더 내 텍스처 그리드 창 ---
			ImGui::BeginChild("TextureGrid", ImVec2(0, 0), true);
			{
				ImGui::Text("Folder: %s", m_strSelectedFolder.empty() ? "None" : m_strSelectedFolder.c_str());
				ImGui::Separator();

				if (!m_strSelectedFolder.empty() && m_TextureMap.count(m_strSelectedFolder))
				{
					auto& fileList = m_TextureMap[m_strSelectedFolder];
					int columns = 5; // 한 줄에 5장씩

					for (int i = 0; i < fileList.size(); ++i)
					{
						string fullPath = fileList[i].first;
						string fileName = fileList[i].second;

						// 리소스 매니저에서 텍스처 가져오기 (태그 명명 규칙 확인해봐!)
						wstring textureTag = L"Texture_" + Engine_Utils::ToWString(fileName);
						CTextureBase* pTexture = m_pGameInstance->Get_Resource<CTextureBase>(textureTag);
						ID3D11ShaderResourceView* pSRV = (pTexture) ? pTexture->Get_SRV() : nullptr;

						ImGui::PushID(i);
						ImGui::BeginGroup();

						if (pSRV)
						{
							// 이미지 버튼 크기를 64x64 정도로 키움
							if (ImGui::ImageButton("##texBtn", (ImTextureID)pSRV, ImVec2(64, 64)))
							{
								m_tCurrentDesc._Effect_DiffuseTexture_Tag = Engine_Utils::ToWString(fileName);
								m_bModified = true;
								ImGui::EndGroup(); ImGui::PopID();
								ImGui::CloseCurrentPopup();

								Safe_Release(pTexture);
								break;
							}
						}
						else
						{
							ImGui::Button("No Res", ImVec2(64, 64));
						}

						// 파일명이 너무 길면 잘라서 출력
						string display = (fileName.length() > 10) ? fileName.substr(0, 8) + ".." : fileName;
						ImGui::Text(display.c_str());

						ImGui::EndGroup();
						ImGui::PopID();

						// 가로로 5장 배치 로직
						if ((i + 1) % columns != 0) ImGui::SameLine(0, 10.f);

						Safe_Release(pTexture);
					}
				}
			}
			ImGui::EndChild();

			ImGui::Separator();
			if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

			ImGui::EndPopup();
		}

		ImGui::Spacing();

		// ===========   Noise Texture  ============
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Noise Texture"); ImGui::SameLine();

		if (ImGui::Button("Open Texture Selector##Noise Texture"))
			ImGui::OpenPopup("TextureSelector##Noise Texture");

		ImGui::SameLine();

#pragma endregion

#pragma region NOISE_TEXTURE
		// ============ NoiseTexture Use check box =============
		if (ImGui::Checkbox("Use##NoiseTexture", &m_tCurrentDesc._Effect_Tool_NoiseTexture))
		{
			if (m_tCurrentDesc._Effect_Tool_NoiseTexture)
				Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_TextureFlag, 2);

			else
				Engine_Utils::RemoveHard_Flag(m_tCurrentDesc._Effect_TextureFlag, 2);

			m_bModified |= true;
		}

		ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Appearing);

		if (ImGui::BeginPopupModal("TextureSelector##Noise Texture", NULL))
		{
			ImGui::BeginChild("FolderList##Noise Texture", ImVec2(180, 0), true);
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Category");
				ImGui::Separator();

				for (auto& folderName : m_TextureFolderNames)
				{
					// 현재 선택된 폴더면 하이라이트 효과
					if (ImGui::Selectable(folderName.c_str(), m_strSelectedFolder == folderName))
					{
						m_strSelectedFolder = folderName;
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			// --- 우측 : 선택된 폴더 내 텍스처 그리드 창 ---
			ImGui::BeginChild("TextureGrid##Noise Texture", ImVec2(0, 0), true);
			{
				ImGui::Text("Folder: %s", m_strSelectedFolder.empty() ? "None" : m_strSelectedFolder.c_str());
				ImGui::Separator();

				if (!m_strSelectedFolder.empty() && m_TextureMap.count(m_strSelectedFolder))
				{
					auto& fileList = m_TextureMap[m_strSelectedFolder];
					int columns = 5; // 한 줄에 5장씩

					for (int i = 0; i < fileList.size(); ++i)
					{
						string fullPath = fileList[i].first;
						string fileName = fileList[i].second;

						// 리소스 매니저에서 텍스처 가져오기 (태그 명명 규칙 확인해봐!)
						wstring textureTag = L"Texture_" + Engine_Utils::ToWString(fileName);
						CTextureBase* pTexture = m_pGameInstance->Get_Resource<CTextureBase>(textureTag);
						ID3D11ShaderResourceView* pSRV = (pTexture) ? pTexture->Get_SRV() : nullptr;

						ImGui::PushID(i);
						ImGui::BeginGroup();

						if (pSRV)
						{
							// 이미지 버튼 크기를 64x64 정도로 키움
							if (ImGui::ImageButton("##texBtnNoise Texture", (ImTextureID)pSRV, ImVec2(64, 64)))
							{
								m_tCurrentDesc._Effect_NoiseTexture_Tag = Engine_Utils::ToWString(fileName);
								m_bModified = true;
								ImGui::EndGroup(); ImGui::PopID();
								ImGui::CloseCurrentPopup();

								Safe_Release(pTexture);
								break;
							}
						}
						else
						{
							ImGui::Button("No Res##Noise Texture", ImVec2(64, 64));
						}

						// 파일명이 너무 길면 잘라서 출력
						string display = (fileName.length() > 10) ? fileName.substr(0, 8) + ".." : fileName;
						ImGui::Text(display.c_str());

						ImGui::EndGroup();
						ImGui::PopID();

						// 가로로 5장 배치 로직
						if ((i + 1) % columns != 0) ImGui::SameLine(0, 10.f);

						Safe_Release(pTexture);
					}
				}
			}
			ImGui::EndChild();

			ImGui::Separator();
			if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

			ImGui::EndPopup();
		}

		ImGui::Spacing();

#pragma endregion

#pragma region MASKING_TEXTURE
		// ===========   Masking Texture  ============
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Masking Texture"); ImGui::SameLine();

		if (ImGui::Button("Open Texture Selector##Masking Texture"))
			ImGui::OpenPopup("TextureSelector##Masking Texture");

		ImGui::SameLine();
		// ============ Masking Use check box =============
		if (ImGui::Checkbox("Use##MaskingTexture", &m_tCurrentDesc._Effect_Tool_MaskingTexture))
		{
			if (m_tCurrentDesc._Effect_Tool_MaskingTexture)
				Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_TextureFlag, 4);

			else
				Engine_Utils::RemoveHard_Flag(m_tCurrentDesc._Effect_TextureFlag, 4);

			m_bModified |= true;
		}

		ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Appearing);

		if (ImGui::BeginPopupModal("TextureSelector##Masking Texture", NULL))
		{
			ImGui::BeginChild("FolderList##Masking Texture", ImVec2(180, 0), true);
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Category");
				ImGui::Separator();

				for (auto& folderName : m_TextureFolderNames)
				{
					// 현재 선택된 폴더면 하이라이트 효과
					if (ImGui::Selectable(folderName.c_str(), m_strSelectedFolder == folderName))
					{
						m_strSelectedFolder = folderName;
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			// --- 우측 : 선택된 폴더 내 텍스처 그리드 창 ---
			ImGui::BeginChild("TextureGrid##Masking Texture", ImVec2(0, 0), true);
			{
				ImGui::Text("Folder: %s", m_strSelectedFolder.empty() ? "None" : m_strSelectedFolder.c_str());
				ImGui::Separator();

				if (!m_strSelectedFolder.empty() && m_TextureMap.count(m_strSelectedFolder))
				{
					auto& fileList = m_TextureMap[m_strSelectedFolder];
					int columns = 5; // 한 줄에 5장씩

					for (int i = 0; i < fileList.size(); ++i)
					{
						string fullPath = fileList[i].first;
						string fileName = fileList[i].second;

						// 리소스 매니저에서 텍스처 가져오기 (태그 명명 규칙 확인해봐!)
						wstring textureTag = L"Texture_" + Engine_Utils::ToWString(fileName);
						CTextureBase* pTexture = m_pGameInstance->Get_Resource<CTextureBase>(textureTag);
						ID3D11ShaderResourceView* pSRV = (pTexture) ? pTexture->Get_SRV() : nullptr;

						ImGui::PushID(i);
						ImGui::BeginGroup();

						if (pSRV)
						{
							// 이미지 버튼 크기를 64x64 정도로 키움
							if (ImGui::ImageButton("##texBtnMasking Texture", (ImTextureID)pSRV, ImVec2(64, 64)))
							{
								m_tCurrentDesc._Effect_MaskingTexture_Tag = Engine_Utils::ToWString(fileName);
								m_bModified = true;
								ImGui::EndGroup(); ImGui::PopID();
								ImGui::CloseCurrentPopup();

								Safe_Release(pTexture);
								break;
							}
						}
						else
						{
							ImGui::Button("No Res##Masking Texture", ImVec2(64, 64));
						}

						// 파일명이 너무 길면 잘라서 출력
						string display = (fileName.length() > 10) ? fileName.substr(0, 8) + ".." : fileName;
						ImGui::Text(display.c_str());

						ImGui::EndGroup();
						ImGui::PopID();

						// 가로로 5장 배치 로직
						if ((i + 1) % columns != 0) ImGui::SameLine(0, 10.f);

						Safe_Release(pTexture);
					}
				}
			}
			ImGui::EndChild();

			ImGui::Separator();
			if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

			ImGui::EndPopup();
		}

		ImGui::Spacing();

#pragma endregion

#pragma region GRADATION_TEXTURE
		// ===========   Gradation Texture  ============
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Gradation Texture"); ImGui::SameLine();

		if (ImGui::Button("Open Texture Selector##Gradation Texture"))
			ImGui::OpenPopup("TextureSelector##Gradation Texture");

		ImGui::SameLine();
		// ============ Gradation Use check box =============
		if (ImGui::Checkbox("Use##GradationTexture", &m_tCurrentDesc._Effect_Tool_GradationTexture))
		{
			if (m_tCurrentDesc._Effect_Tool_GradationTexture)
				Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_TextureFlag, 8);

			else
				Engine_Utils::RemoveHard_Flag(m_tCurrentDesc._Effect_TextureFlag, 8);

			m_bModified |= true;
		}

		ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Appearing);

		if (ImGui::BeginPopupModal("TextureSelector##Gradation Texture", NULL))
		{
			ImGui::BeginChild("FolderList##Gradation Texture", ImVec2(180, 0), true);
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Category");
				ImGui::Separator();

				for (auto& folderName : m_TextureFolderNames)
				{
					// 현재 선택된 폴더면 하이라이트 효과
					if (ImGui::Selectable(folderName.c_str(), m_strSelectedFolder == folderName))
					{
						m_strSelectedFolder = folderName;
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			// --- 우측 : 선택된 폴더 내 텍스처 그리드 창 ---
			ImGui::BeginChild("TextureGrid##Gradation Texture", ImVec2(0, 0), true);
			{
				ImGui::Text("Folder: %s", m_strSelectedFolder.empty() ? "None" : m_strSelectedFolder.c_str());
				ImGui::Separator();

				if (!m_strSelectedFolder.empty() && m_TextureMap.count(m_strSelectedFolder))
				{
					auto& fileList = m_TextureMap[m_strSelectedFolder];
					int columns = 5; // 한 줄에 5장씩

					for (int i = 0; i < fileList.size(); ++i)
					{
						string fullPath = fileList[i].first;
						string fileName = fileList[i].second;

						// 리소스 매니저에서 텍스처 가져오기 (태그 명명 규칙 확인해봐!)
						wstring textureTag = L"Texture_" + Engine_Utils::ToWString(fileName);
						CTextureBase* pTexture = m_pGameInstance->Get_Resource<CTextureBase>(textureTag);
						ID3D11ShaderResourceView* pSRV = (pTexture) ? pTexture->Get_SRV() : nullptr;

						ImGui::PushID(i);
						ImGui::BeginGroup();

						if (pSRV)
						{
							// 이미지 버튼 크기를 64x64 정도로 키움
							if (ImGui::ImageButton("##texBtnGradation Texture", (ImTextureID)pSRV, ImVec2(64, 64)))
							{
								m_tCurrentDesc._Effect_GradationTexture_Tag = Engine_Utils::ToWString(fileName);
								m_bModified = true;
								ImGui::EndGroup(); ImGui::PopID();
								ImGui::CloseCurrentPopup();

								Safe_Release(pTexture);
								break;
							}
						}
						else
						{
							ImGui::Button("No Res##Gradation Texture", ImVec2(64, 64));
						}

						// 파일명이 너무 길면 잘라서 출력
						string display = (fileName.length() > 10) ? fileName.substr(0, 8) + ".." : fileName;
						ImGui::Text(display.c_str());

						ImGui::EndGroup();
						ImGui::PopID();

						// 가로로 5장 배치 로직
						if ((i + 1) % columns != 0) ImGui::SameLine(0, 10.f);

						Safe_Release(pTexture);
					}
				}
			}
			ImGui::EndChild();

			ImGui::Separator();
			if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

			ImGui::EndPopup();
		}

		ImGui::Spacing();
#pragma endregion

		// ===========   Position & Rotation & Scale =============
		ImGui::AlignTextToFramePadding();

		if (ImGui::TreeNode("Transform##Position"))
		{
			// 어차피 gizmo 연동이 되어있음.
			CTransform* pTranform = pGo->Get_Component<CTransform>();
			Vec3 Position = pTranform->Get_Info(TRANSFORM_INFO_STATE::POS);
			Vec3 Scale = pTranform->Get_Scaled();

			ImGui::Text("ObjectPos : (%g, %g, %g)", Position.x, Position.y, Position.z);
			ImGui::Text("ObjectScale : (%g, %g, %g)", Scale.x, Scale.y, Scale.z);

			ImGui::TreePop();
		}
	}

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

	if (ImGui::CollapsingHeader("Renderer"))
	{
		// ============ RenderState check box =============

		if (ImGui::TreeNode("RenderState Setting##Renderer"))
		{
			// =============  [USE BILLBOARD]  ==============
			ImGui::Text("Bilboard"); ImGui::SameLine(0, 200.f);
			if (ImGui::Checkbox("Use##BillBoardRenderer", &m_tCurrentDesc._Effect_Tool_UseBillboard))
			{
				if (m_tCurrentDesc._Effect_Tool_UseBillboard)
					Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_RenderFlag, 1);

				else
					Engine_Utils::RemoveHard_Flag(m_tCurrentDesc._Effect_RenderFlag, 1);

				m_bModified |= true;
			}
			ImGui::Spacing();
			// =============  [CHOOSE SAMPLERSTATE]	  ============

			if (ImGui::TreeNode("SamplerState Setting##Renderer"))
			{
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

					if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag, [](void* data, int idx, const char** out_text)
						{
							auto& vector = *static_cast<std::vector<std::string>*>(data);
							*out_text = vector[idx].c_str();
							return true;
						},
						(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
					{
						m_bModified |= true;
					}

					switch (m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag)
					{
					case 0:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 1); break;
					case 1:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 2); break;
					case 2:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 4); break;
					case 3:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 8); break;
					case 4:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 16); break;
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

					if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag, [](void* data, int idx, const char** out_text)
						{
							auto& vector = *static_cast<std::vector<std::string>*>(data);
							*out_text = vector[idx].c_str();
							return true;
						},
						(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
					{
						m_bModified |= true;
					}

					switch (m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag)
					{
					case 0:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 1); break;
					case 1:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 2); break;
					case 2:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 4); break;
					case 3:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 8); break;
					case 4:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 16); break;
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

					if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag, [](void* data, int idx, const char** out_text)
						{
							auto& vector = *static_cast<std::vector<std::string>*>(data);
							*out_text = vector[idx].c_str();
							return true;
						},
						(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
					{
						m_bModified |= true;
					}

					switch (m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag)
					{
					case 0:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 1); break;
					case 1:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 2); break;
					case 2:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 4); break;
					case 3:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 8); break;
					case 4:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 16); break;
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

					if (ImGui::ListBox("##ParticleType Select", &m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag, [](void* data, int idx, const char** out_text)
						{
							auto& vector = *static_cast<std::vector<std::string>*>(data);
							*out_text = vector[idx].c_str();
							return true;
						},
						(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
					{
						m_bModified |= true;
					}

					switch (m_tCurrentDesc._Effect_Tool_DiffuseSamplerState_Flag)
					{
					case 0:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 1); break;
					case 1:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 2); break;
					case 2:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 4); break;
					case 3:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 8); break;
					case 4:
						Engine_Utils::Add_Flag(m_tCurrentDesc._Effect_SamplerStateFlag, 16); break;
					}

					ImGui::TreePop();
					ImGui::Spacing();
				}

				ImGui::TreePop();
			}



			ImGui::TreePop();
		}

		// ===========   ParticleType Select   ===========
				// Render Mode
		if (ImGui::TreeNode("Particle Type"))
		{
			static int ParticleNumber = {};

			vector<string> m_PParticleTypeList;
			m_PParticleTypeList.clear();

			m_PParticleTypeList.push_back("PARTICLE");
			m_PParticleTypeList.push_back("TEXTURE");
			m_PParticleTypeList.push_back("MESH");

			std::vector<const char*> iTems1;
			iTems1.reserve(static_cast<int>(m_PParticleTypeList.size()));

			for (auto& str : m_PParticleTypeList)
				iTems1.push_back(str.c_str());

			if (ImGui::ListBox("##ParticleType Select", &ParticleNumber, [](void* data, int idx, const char** out_text)
				{
					auto& vector = *static_cast<std::vector<std::string>*>(data);
					*out_text = vector[idx].c_str();
					return true;
				},
				(void*)&m_PParticleTypeList, (int)m_PParticleTypeList.size(), 2))
			{ 
				m_bModified |= true;
			}

			switch (ParticleNumber)
			{
			case 0:
				m_tCurrentDesc.eEffectParticleType = E_PARTICLETYPE::PARTICLE; break;
			case 1:
				m_tCurrentDesc.eEffectParticleType = E_PARTICLETYPE::TEXTURE; break;
			case 2:
				m_tCurrentDesc.eEffectParticleType = E_PARTICLETYPE::MESH; 
				Make_MeshSelectButton();
				break;
			default:
				m_tCurrentDesc.eEffectParticleType = E_PARTICLETYPE::NONE;
				break;
			}

			ImGui::TreePop();
			ImGui::Spacing();
		}

		// ===========   Shader List  ============
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Shader List"))
		{
			static int Shader_Number = 0;

			if (ImGui::ListBox("##Shader_List", &Shader_Number, [](void* data, int idx, const char** out_text)
				{
					auto& vector = *static_cast<std::vector<std::pair<std::string, std::string>>*>(data);
					*out_text = vector[idx].second.c_str();
					return true;
				},
				(void*)&m_ShaderFileNames, (int)m_ShaderFileNames.size(), 5))
			{
				if (Shader_Number < m_ShaderFileNames.size())
				{
					m_tCurrentDesc._Effect_Shader_Path = Engine_Utils::ToWString(m_ShaderFileNames[Shader_Number].first);
					m_tCurrentDesc._Effect_Shader_Tag = Engine_Utils::ToWString(m_ShaderFileNames[Shader_Number].second);
				}

				m_bModified |= true;
			}

			ImGui::Spacing();
			// Shader Pass 선택하기
			ImGui::Text("Shader Pass :"); ImGui::SameLine();
			m_bModified |= ImGui::InputInt("##Renderer_ShaderPass", &m_tCurrentDesc._Effect_ShaderPass); 
			ImGui::TreePop(); ImGui::Spacing();
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
				m_tCurrentDesc._Effect_Model_Tag = Engine_Utils::ToWString(MeshName);

				m_bModified |= true;
			}
		}
	}

}

void CParticle_System_Panel::Draw_EffectColor(CToolObject* pGo)
{
	ImGui::Begin("Effect Settings");

	ImGui::Text("Particle Color Settings");

	m_bModified |= ImGui::ColorPicker4("##MyEffectPicker", (float*)&m_tCurrentDesc._Effect_Color,
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_AlphaBar |       // 투명도 조절 바 표시
		ImGuiColorEditFlags_DisplayRGB);

	ImGui::End();
}

void CParticle_System_Panel::Draw_Parts(CToolObject* pGo)
{
	ImGui::Begin("Effect Parts Setting");

	// ContainerObject의 Parts List를 들고오고 size를 통해 Index 조작도 되게끔 한다.
	Effect* pInstance = static_cast<Effect*>(pGo);
	if (pInstance == nullptr)
	{
		ImGui::End();
		return;
	}

	static int PartIndex = {};
	auto PartsList = pInstance->Get_PartList();
	
	PartIndex = static_cast<int>(PartsList.size()) - 1;

	if (ImGui::TreeNode("Effect_PartsList##Effect_PartsList"))
	{
		std::vector<const char*> iTems;
		iTems.reserve(static_cast<int>(PartsList.size()));

		for (auto& parts : PartsList)
			iTems.push_back(parts->Get_Name().c_str());
		
		if (ImGui::ListBox("##PartsSelector", &PartIndex, iTems.data(), static_cast<int>(iTems.size()), 6))
		{
			m_iSelectPartsIndex = PartIndex;
			m_bModified |= true;

			if ((iTems.size() - 1) < PartIndex)
			{
				ImGui::TreePop();
				ImGui::End();
				return;
			}

			CEffectObject* pSelectdPart = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(m_iSelectPartsIndex);
			if (pSelectdPart)
				m_tCurrentDesc = pSelectdPart->Get_EffectDesc();
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Edit##Part_Setting"))
	{
		if (ImGui::TreeNode("Create##Part_Setting"))
		{
			// 이름 짓기
			static char nameBuf[128] = {};
			string effectName = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(PartIndex)->Get_Name();

			if (nameBuf[0] == '\0' && !effectName.empty())
			{
				std::string tmp = effectName;
				strncpy_s(nameBuf, sizeof(nameBuf), tmp.c_str(), _TRUNCATE);
			}
			// ImGui에서 입력 받기
			ImGui::InputText("##ChangeName_PartSetting", nameBuf, IM_ARRAYSIZE(nameBuf)); ImGui::SameLine();
			effectName = nameBuf;
			
			if (ImGui::Button("Create##Part_ObjectSetting"))
			{
				// =========	Create	  ==============
				CGameObject* pResult = { nullptr };

				CEffectObject::Effect_Desc pEffectDesc = {};
				CTransform::TRANSFORM_DESC transformDesc = {};
				// =========    트랜스폼   ============
				transformDesc.vPosition = { 0.f, 0.0f, 0.f };
				transformDesc.fRotatePerSec = 1.f;
				transformDesc.fMovePerSec = 1.f;

				pEffectDesc.pMatParent = &(pGo->Get_Component<CTransform>()->Get_WorldMatrix());
				pEffectDesc.pTransform_Desc = &transformDesc;

				pEffectDesc.wstrLayerTag = L"Effect_Parts";
				pEffectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::EFFECT);
				// ========     이펙트 타입   =========
				pEffectDesc.eEffectSystemType = E_EffectSystemType::Particle;
				pEffectDesc.eEffectType = E_EFFECTTYPE::Particle;

				// =========   이펙트 Color Value   ===============
				pEffectDesc._Effect_Color = Vec4{ 0.f, 0.f, 0.f, 1.f };

				// ========  이펙트 Material 설정   ===========
				pEffectDesc._Effect_Model_Tag = {};
				pEffectDesc._Effect_Shader_Tag = {};
				pEffectDesc._Effect_DiffuseTexture_Tag = {};
				pEffectDesc._Effect_NoiseTexture_Tag = {};
				pEffectDesc._Effect_DiffuseTexture_Tag = {};
				pEffectDesc._Effect_ShaderPass = {};

				pEffectDesc._Effect_bUseAtlas = false;
				pEffectDesc._Effect_TileCount = CEffectObject::_uint2{ 0, 0 };

				// =======   이펙트 스크롤 Value   ===========
				pEffectDesc._Effect_ScrollSpeed = { 0.f, 0.f };

				// ========   이펙트 왜곡 Scale Value   ==========
				pEffectDesc._Effect_DistortionScale = { 0.f, 0.f };

				// ==========   이펙트 Sacle Value   ==============
				pEffectDesc._Effect_StartScale = { 1.f, 1.f, 1.f };
				pEffectDesc._Effect_EndScale = { 1.f, 1.f, 1.f };

				static_cast<Effect*>(pGo)->Add_Part(static_cast<_uint>(PartsList.size()), ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect_Parts", &pEffectDesc);
				static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(PartsList.size())->Set_Name(effectName);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Name Change##Part_Setting"))
		{
			static char nameBuf[128] = {};
			string effectName = static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(PartIndex)->Get_Name();

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
				static_cast<Effect*>(pGo)->Get_Part<CEffectObject>(PartIndex)->Set_Name(effectName);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	
	ImGui::End();
	
}


HRESULT CParticle_System_Panel::EffectPanel_Initialize()
{
	if (FAILED(EffectFileResource_Setting()))
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
}
