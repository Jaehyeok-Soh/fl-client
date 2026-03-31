#include "pch.h"
#include "ImGui_GlobalInspector.h"
#include "ImGui_FrustrumLayout.h"
#include "ImGui_ShaderLayout.h"
#include "GameObject.h"
#include "Water.h"
#include "GameInstance.h"

CImGui_GlobalInspector::CImGui_GlobalInspector()
	: Super("GlobalInspector", ELayoutType::COUNT)
{
}

HRESULT CImGui_GlobalInspector::Initialize()
{
	if (FAILED(Add_Layout(ELayoutType::Frustrum, CImGui_FrustrumLayout::Create())))
		return E_FAIL;

	if (FAILED(Add_Layout(ELayoutType::Shader, CImGui_ShaderLayout::Create())))
		return E_FAIL;

	return S_OK;
}

void CImGui_GlobalInspector::Render(CGameObject* pGo)
{
	if (ImGui::Begin(m_strLabel.c_str()))
	{
		Render_Layout(ELayoutType::Frustrum, nullptr);
		Render_Layout(ELayoutType::Shader, nullptr);
	}


	/* 임시   */
	list<CGameObject*>* pList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::TUTORIAL_VILLAGE), g_wszWaterLayer);
	if (pList != nullptr)
	{
		// 1. 리스트에 물 객체가 있는지 먼저 확인!
		if (!pList->empty())
		{
			// 2. ImGui 창(또는 접기/펴기 헤더) 열기
			if (ImGui::CollapsingHeader("Water Ripple Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 3. 기준이 될 첫 번째 물 객체의 데이터를 가져옴

				_uint i = 0;
				for (auto& pObj : *pList)
				{
					ImGui::PushID(i);
					string strName = "Water Data [" + std::to_string(i) + "]";
					if (ImGui::TreeNode(strName.c_str()))
					{
						
						CB_WaterData* pWaterData = static_cast<CWater*>(pObj)->Get_WaterData();

						ImGui::DragFloat("Max Radius", &pWaterData->tWaterRippleEffect.fMaxRadius, 0.1f, 1.0f, 100.0f, "%.2f");

						ImGui::DragFloat("Ripple Freq", &pWaterData->tWaterRippleEffect.fRippleFreq, 0.1f, 0.1f, 50.0f, "%.2f");

						ImGui::DragFloat("Ripple Speed", &pWaterData->tWaterRippleEffect.fRippleSpeed, 0.1f, -20.0f, 20.0f, "%.2f");
						ImGui::DragFloat("Ripple Amp", &pWaterData->tWaterRippleEffect.fRippleAmp, 0.01f, 0.0f, 2.0f, "%.3f");


						ImGui::DragFloat("Ripple Thickness", &pWaterData->tWaterRippleEffect.fRingThickness, 0.001f, 0.0f, 2.0f, "%.3f");

						ImGui::TreePop();
					}
					++i;
					ImGui::PopID();
				}
			}
		}
	}


	ImGui::End();
}

CImGui_GlobalInspector* CImGui_GlobalInspector::Create()
{
	CImGui_GlobalInspector* pInstance = new CImGui_GlobalInspector();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_GlobalInspector::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_GlobalInspector::Free()
{
	Super::Free();
}
