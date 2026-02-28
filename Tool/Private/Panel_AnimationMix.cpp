#include "pch.h"
#include "Panel_AnimationMix.h"

// has obj
#include "AnimTool_Manager.h"

CPanel_AnimationMix::CPanel_AnimationMix(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pAnimToolManager(CAnimTool_Manager::GetInstance())
{
}

HRESULT CPanel_AnimationMix::Initialize()
{
	return S_OK;
}

HRESULT CPanel_AnimationMix::Render(CToolObject* pGo)
{
	ImGui::Begin("Animation Mixer");

    if (pModel)
    {
        Current_Info();
        ImGui::Separator();

        Set_AnimationMixInfo();
        ImGui::Separator();

        Set_AnimMationMix();
    }

	ImGui::End();
	return S_OK;
}

void CPanel_AnimationMix::Update(const _float fTimeDelta)
{
	ANIMCTRLINFO tInfo = m_pAnimToolManager->Get_AnimControllInfo();

    if (pModel = tInfo.pModel)
    {
        //m_bMix = tInfo.pModel->Get_MixBool();
        m_vecMixIdx = tInfo.pModel->Get_MixIdx();
    }

}

void CPanel_AnimationMix::Current_Info()
{
    /* mix 할래말래 */
	ImGui::Checkbox("Mix On", &m_bMix);

    if (ImGui::Button("Apply Mix##MixSize"))
    {
        pModel->Set_MixAnim(m_bMix);
    }

    //if (!m_vecMixIdx.empty())
    //{
    //    if (ImGui::TreeNode("Mix Index Vector"))
    //    {
    //        // 1. 현재 사이즈 출력
    //        ImGui::Text("Size : %d", (int)m_vecMixIdx.size());

    //        ImGui::Separator();

    //        // 2. 각 원소 출력 + 수정
    //        for (int i = 0; i < (int)m_vecMixIdx.size(); ++i)
    //        {
    //            ImGui::PushID(i); // ID 충돌 방지

    //            ImGui::SetNextItemWidth(80.f); // 길이 줄이고 싶으면 조절
    //            ImGui::InputInt("##MixIdx", &m_vecMixIdx[i]);

    //            ImGui::SameLine();
    //            ImGui::Text("[%d]", i);

    //            //ImGui::SameLine();
    //            //if (ImGui::Button("Delete"))
    //            //{
    //            //    m_vecMixIdx.erase(m_vecMixIdx.begin() + i);
    //            //    ImGui::PopID();
    //            //    break; // erase 후에는 루프 탈출
    //            //}

    //            ImGui::PopID();
    //        }

    //        //ImGui::Separator();

    //        //// 3. 원소 추가
    //        //static int newValue = 0;
    //        //ImGui::SetNextItemWidth(80.f);
    //        //ImGui::InputInt("New Value", &newValue);

    //        //if (ImGui::Button("Add"))
    //        //{
    //        //    m_vecMixIdx.push_back(newValue);
    //        //}

    //        //ImGui::TreePop();
    //    }
    //}
}

void CPanel_AnimationMix::Set_AnimationMixInfo()
{
    ImGui::SetNextItemWidth(120.f); // 원하는 픽셀 길이
    ImGui::InputInt("MixSize Reset", &m_iMixSize, 1);
    
    if (m_iMixSize < 0)
        m_iMixSize = 0;

    ImGui::SameLine();
    if (ImGui::Button("Apply Mix Size to Model##MixSize"))
    {
        pModel->Set_MixAnim_ResetSize(m_iMixSize);
    }
}

void CPanel_AnimationMix::Set_AnimMationMix()
{
    ImGui::SetNextItemWidth(120.f); // 원하는 픽셀 길이
    ImGui::InputInt("Mix Anim Index", &m_iMixAnimIdx, 1);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.f); // 원하는 픽셀 길이
    ImGui::InputInt("Mix Vector Index", &m_iMixVectorIdx, 1);

    Set_MixData();

    if (ImGui::Button("Make Mix Data to Model and Anim ##RootBone"))
    {
        // index 값 검사 후 적용
        if (m_iMixAnimIdx >= 0 &&
            m_iMixVectorIdx < pModel->Get_MixSize())
        {
            CComputeShader* pAnimMixCS = static_cast<CComputeShader*>(m_pAnimToolManager->Get_AnimControllInfo().pCurrentObject->Get_Script_Component(TEXT("ComputeShader_AnimMix")));
            pModel->Make_MixRatio(m_iMixAnimIdx, m_vecMixData, pAnimMixCS);
            pModel->Set_MixAnim_AnimIndex(m_iMixVectorIdx, m_iMixAnimIdx);
        }
    }
}

void CPanel_AnimationMix::Set_MixData()
{
    if (ImGui::TreeNode("Mix Data"))
    {
        ImGui::Text("Size : %d", (int)m_vecMixData.size());
        ImGui::Separator();

        for (int i = 0; i < (int)m_vecMixData.size(); ++i)
        {
            ImGui::PushID(i);

            if (ImGui::CollapsingHeader(("Element " + std::to_string(i)).c_str()))
            {
                // iParentIdx
                ImGui::SetNextItemWidth(100.f);
                ImGui::InputInt("Parent Idx", (_int*)&m_vecMixData[i].iParentIdx);

                // bInclude
                ImGui::Checkbox("Include", (_bool*)&m_vecMixData[i].bInClude);

                // fRatio
                ImGui::SetNextItemWidth(100.f);
                ImGui::DragFloat("Ratio", &m_vecMixData[i].fRatio, 0.01f, 0.f, 1.f);
            }

            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                m_vecMixData.erase(m_vecMixData.begin() + i);
                ImGui::PopID();
                break;
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        // 새 요소 추가
        if (ImGui::Button("Add Mix Data"))
        {
            CModel::DATA_ANIMIX newData{};
            newData.iParentIdx = -1;
            newData.bInClude = false;
            newData.fRatio = 0.f;

            m_vecMixData.push_back(newData);
        }

        ImGui::TreePop();
    }
}

CPanel_AnimationMix* CPanel_AnimationMix::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_AnimationMix* pInstance = new CPanel_AnimationMix(pLabel, pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPanel_AnimationMix is faield to Create");
		return nullptr;
	}

	return pInstance;
}

void CPanel_AnimationMix::Free()
{
	__super::Free();
}
