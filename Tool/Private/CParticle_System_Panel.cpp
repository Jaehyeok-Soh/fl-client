#include "CParticle_System_Panel.h"
#include "GameInstance.h"
#include "Engine_Utils.h"

CParticle_System_Panel::CParticle_System_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CParticle_System_Panel::Update(const _float fDT)
{
	if (m_bTimeSetting)
		Time_Calculator(fDT);

}

HRESULT CParticle_System_Panel::Render(CToolObject* pGo)
{
	if (pGo == nullptr)
	{
		m_bTimeSetting = false;
		return E_FAIL;
	}

	Draw_ParticleSystem();
	Draw_Timer();

	return S_OK;
}

void CParticle_System_Panel::Time_Calculator(const float fDT)
{
	m_fTimeAccumulation += fDT * m_fPlayBackSpeed;
}

void CParticle_System_Panel::Draw_MeshFileList()
{

}

void CParticle_System_Panel::Draw_TextureFileList()
{

}

void CParticle_System_Panel::Draw_Timer()
{
	ImGui::Begin("Particles");
	
	ImGui::Spacing();

	// 1. 제어 버튼 (Pause, Restart, Stop)
	ImGui::Separator();

	// ===================   Play Restart Stop   ===================
	if (ImGui::Button("Play"))
	{
		m_bTimeSetting = true;
	}
	ImGui::SameLine();

	if (ImGui::Button("Pause"))
	{
		m_bTimeSetting = false;
	}
	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		m_bTimeSetting = false;
		m_fTimeAccumulation = 0.f;
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	//  ==================   Playback Speed   ====================== 

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Playback Speed");
	ImGui::Spacing();
	ImGui::InputFloat("##PlaybackSpeed", &m_fPlayBackSpeed, 0.f);
	ImGui::Spacing();

	// ==================   Playback Time   =======================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Playback Time");
	ImGui::Spacing();
	ImGui::InputFloat("##PlayBackTime", &m_fTimeAccumulation, 0.f);
	ImGui::Spacing();

	// Particles 
	// Speed Range
	// S
	ImGui::End();
}

void CParticle_System_Panel::Draw_ParticleSystem()
{
	ImGui::Begin("Particle System");
	
	ImGui::Separator();
	// 트랜스폼 - 파티클 시스템 위치 옮기기 (부모 위치 옮기기)

		// Position
		// Rotation
		// Scale

	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

	// 1. 시간 관련 설정 
		// ====================   Duration - 시스템이 한번 실행되는 총 시간   =====================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Duration");
	ImGui::SameLine();
	static float Duration = 0.f;
	ImGui::InputFloat("##Duration1", &Duration, 0.f);
	ImGui::Spacing();

	// ===================   Looping - 재생이 끝나면 반복할지 결정한다		=====================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Looping");
	ImGui::SameLine();
	static bool Looping = false;
	ImGui::Checkbox("##Looping1", &Looping);
	ImGui::Spacing();

	// ==================   Prewarm - 체크하면 게임 시작시 이미 시스템이 헌 서아쿨아 돌아간 것처럼 미리 입자가 퍼져있다.  =========================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Prewarm");
	ImGui::SameLine();
	static bool Prewarm = false;
	ImGui::Checkbox("##Prewarm1", &Prewarm);
	ImGui::Spacing();

	// =================   Start Delay - 재생 버튼을 누르고 실제 입자가 나오기까지 걸리는 시간   =========================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Start Delay");
	ImGui::SameLine();
	ImGui::InputFloat("##StartDelay1", &m_tCurrentDesc._Effect_StartDelay, 0.f);
	ImGui::Spacing();

	// 2. 파티클 초기 설정
		// ================   Start LifeTime - 입자가 태어나서 사라질 때까지의 수명   =======================================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Start LifeTime");
	ImGui::SameLine();
	ImGui::InputFloat("##StartLifeTime1", &m_tCurrentDesc._Effect_LifeTime, 0.f);
	ImGui::Spacing();

	// ===============   Start Speed - 입자가 처음 뿜어져 나올 때의 속도   =============================================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Start Speed");
	ImGui::SameLine();
	static float StartSpeed = 0.f;
	ImGui::InputFloat("##StartSpeed1", &StartSpeed, 0.f);
	ImGui::Spacing();

	// ==============  Start Size - 입자의 초기 사이즈   =========================================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Start Size");
	ImGui::SameLine();

	ImGui::Text("X"); ImGui::SameLine();
	ImGui::InputFloat("##StartX", &m_tCurrentDesc._Effect_StartScale.x); ImGui::SameLine();

	ImGui::Text("Y"); ImGui::SameLine();
	ImGui::InputFloat("##StartY", &m_tCurrentDesc._Effect_StartScale.y); ImGui::SameLine();

	ImGui::Text("Z"); ImGui::SameLine();
	ImGui::InputFloat("##StartZ", &m_tCurrentDesc._Effect_StartScale.z); ImGui::Spacing();

	// ==============  End Size - 입자의 마지막 사이즈   =========================================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("End Size");
	ImGui::SameLine();

	ImGui::Text("X"); ImGui::SameLine();
	ImGui::InputFloat("##EndX", &m_tCurrentDesc._Effect_EndScale.x); ImGui::SameLine();

	ImGui::Text("Y"); ImGui::SameLine();
	ImGui::InputFloat("##EndY", &m_tCurrentDesc._Effect_EndScale.y); ImGui::SameLine();

	ImGui::Text("Z"); ImGui::SameLine();
	ImGui::InputFloat("##EndZ", &m_tCurrentDesc._Effect_EndScale.z); ImGui::Spacing();

	// ==============   3D Start Rotation - 입자의 축마다의 회전값    =========================

	ImGui::AlignTextToFramePadding();
	ImGui::Text("3D Start Rotataion");
	ImGui::SameLine();
	static bool StartRotation_3D = false;

	ImGui::Checkbox("##StartRotation_3D", &StartRotation_3D); ImGui::Spacing();

	if (StartRotation_3D)
	{
		static float StartRotationX_3D = 0.f;
		static float StartRotationY_3D = 0.f;
		static float StartRotationZ_3D = 0.f;

		ImGui::Text("X"); ImGui::SameLine();
		ImGui::InputFloat("##StartRotationX_3D", &StartRotationX_3D); ImGui::SameLine();

		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::InputFloat("##StartRotationY_3D", &StartRotationY_3D); ImGui::SameLine();

		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::InputFloat("##StartRotationZ_3D", &StartRotationZ_3D); ImGui::Spacing();
	}

	// ==========   Start Rotation - 모든 축의 회전값   ===================================


	// ==============    Start Color - 이펙트 시작 색깔  ========================================




		// Gravity Source - 중력이 어디서 오는지 결정한다. 물리 상수 (9.8ms^2)
		// Gravity Modifier - 중력의 영향을 얼마나 강하게 받을지 정하는 배수. 0이면 무중력, 1이면 설정된 중력만큼 아래로 떨어진다.
			
			
		// Simulation Space - 파티클의 좌표계를 결정한다.
			// 1. Local - 부모가 움직이면 이미 생성된 입자도 부모를 따라 움직인다.
			// 2. World - 생성된 후에는 부모를 떠나 독립된 월드 좌표에서 움직입니다.

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Simulation Space");
	ImGui::Spacing();

	ImGui::Text("Local"); ImGui::SameLine(0, 20.f);
	ImGui::Text("World"); ImGui::Spacing();
	static bool Local = false;
	static bool World = false;

	ImGui::Checkbox("##Local1", &Local); ImGui::SameLine(0, 40.f);
	ImGui::Checkbox("##World1", &World); ImGui::Spacing();

	if (Local)
	{
		m_tCurrentDesc._Effect_SimulationType = E_SIMULATION_SPACE::LOCAL;
	}


	else if (World)
	{
		m_tCurrentDesc._Effect_SimulationType = E_SIMULATION_SPACE::WORLD;
	}

		// Simulation Speed - 전체 이펙트의 재생 속도 배수입니다. 2라면 2배속으로 빠르게 움직입니다.
		// Delta Time - 시간 계산 방식을 정합니다. 게임 속도에 맞춰진 Scaled 시간인가?
		// Scaling Mode - 부모 오브젝트의 크기가 변할 때 파티클이 어떻게 반응할지 결정
// 
		// Play On Awake* - 파티클 시스템이 씬에 생성되자마자 즉시 재생될지 여부를 결정한다.
		// Emitter Velocity Mode - 파티클이 방출될 때, 본체의 속도를 어떻게 계산할지 정합니다.
		// Max Particles = 동시에 존재할 수 있는 최대 입자 개수. 이 값만큼 iNSTANCE bUFFER의 크기를 잡아두면 성능상 좋음

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Max Particles");
	ImGui::Spacing();
	ImGui::InputInt("##MaxParticles", &m_tCurrentDesc._Effect_MaxParticle, 0);
	ImGui::Spacing();

		// Auto Random Seed - 난수 Seed를 다르게 생성해서 입자 모양이 랜덤하게 변한다.
		// Stop Action - 이펙트가 완전히 끝났을 때 해당 오브젝트를 어떻게 처리할지 정한다.
		// Culling Mode - 카메라 화면 밖에 있을 때 연산을 중단할지 여부를 결정해서 성능을 최적화합니다.
		// Ring Buffer Mode - 최대 입자 개수가 찼을 때, 가장 오래된 입자를 지우고 새 입자를 만들지 결정한다.

	ImGui::Separator();

	// ============================   Emission   =================================
	ImGui::Text("Emission");

	// - Emission
		// Rate Over Time - 1초당 생성할 파티클 개수. AccumulatedTime >= 1.0f / Rate일 때 입자를 생성하는 로직을 짠다.

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Rate Over Time");
	ImGui::Spacing();
	static float RateOverTime = 0.f;
	ImGui::InputFloat("##RateOverTime", &RateOverTime, 0.f);
	ImGui::Spacing();
		// Rate Over Distance - 본체가 이동한 거리에 비례해서 입자를 뿜습니다. 

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Rate Over Distance");
	ImGui::Spacing();
	static float RateOverDistance = 0.f;
	ImGui::InputFloat("##RateOverDistance", &RateOverDistance, 0.f);
	ImGui::Spacing();

		// Bursts - 특정 시간에 파티클을 한꺼번에 '확' 터뜨리는 기능입니다.
			// Time - 시스템 시작 후 몇 초 뒤에 터뜨릴지 정합니다. 
			// Count - 그 순간 한꺼번에 생성할 입자의 개수입니다.
			// Cycles - 이 작업을 몇 번 반복할지 정합니다.
			// Interval : 반복할 때 ㅅ이의 시간 간격입니다.
			// Probability - 해당 폭발이 일어날 확률(0~1)

	// ==========================  Shape  =======================================
	// - Shape			- 어디서 태어났냐
		// Shape
			// 분사 타입별 함수를 만들어서 변수 저장.
	ImGui::SeparatorText("Shape");
	ImGui::Text("Shape##Shape_EffectList"); ImGui::SameLine();

	static int EffectNumber = {};
	if (ImGui::TreeNode("##Effect_List"))
	{
		vector<string> m_pShapeList;
		m_pShapeList.clear();

		m_pShapeList.push_back("Sphere");
		m_pShapeList.push_back("Hemisphere");
		m_pShapeList.push_back("Circle");
		m_pShapeList.push_back("Cone");
		m_pShapeList.push_back("Box");
		m_pShapeList.push_back("Mesh");

		std::vector<const char*> iTems;
		iTems.reserve(static_cast<int>(m_pShapeList.size()));

		for (auto& str : m_pShapeList)
			iTems.push_back(str.c_str());

		ImGui::ListBox("", &EffectNumber, iTems.data(), static_cast<int>(m_pShapeList.size()), 6);

		if ((iTems.size() - 1) < EffectNumber)
		{
			ImGui::TreePop();
			return;
		}
		ImGui::TreePop();
	}

	switch (EffectNumber)
	{
		case 0:
			m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::SPHERE;
			break;
		case 1:
			m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::HEMISPHERE;
			break;
		case 2:
			m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::CIRCLE;
			break;
		case 3:
			m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::CONE;
			break;
		case 4:
			m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::BOX;
			break;
		case 5:
			m_tCurrentDesc._Effect_ShapeType = E_SHAPETYPE::MESH;
			break;
	}

	// ============   Radius   ============= 
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Radius");
	ImGui::Spacing();
	ImGui::InputFloat("##Radius", &m_tCurrentDesc._Effect_Radius, 0.f);
	ImGui::Spacing();

		// Type 
			// Mode 
		// Mesh
		// Single Material
		// Use Mesh Colors
		// Normal Offsset
		// Texture

		// Position
		// Rotation
		// Scale

	// Align To Direction
	// Randomize Direction
	// Spherize Direction
	// Randomize Position


	// Renderer		
		// Render Mode
		// Mesh Distribution
		// Meshes

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
	// depth를 사용하려면 범위 for을 돌리면 안된다. recursive_directory_iterator산하에 있는 함수이다.
	ResourceFileSearch(E_EFFECT_RESOURCETYPE::TEXTURE, m_sTextureFolderPath);
	ResourceFileSearch(E_EFFECT_RESOURCETYPE::MESH, m_sMeshFolderPath);
	
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
			string m_sFileName = Engine_Utils::GetFileNameFromPath(Engine_Utils::ToString(fullpath));

			switch (eType)
			{
				case E_EFFECT_RESOURCETYPE::TEXTURE:
					m_TextureFileNames.push_back(m_sFileName); break;

				case E_EFFECT_RESOURCETYPE::MESH:
					m_MeshFileNames.push_back(m_sFileName); break;
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
