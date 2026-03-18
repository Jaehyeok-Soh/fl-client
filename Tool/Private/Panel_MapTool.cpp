#include "pch.h"
#include "Panel_MapTool.h"
#include "GameInstance.h"
#include "MapObject.h"
#include "Camera.h"
#include "LevelData.h"
#include "CameraMan.h"

CPanel_MapTool::CPanel_MapTool(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext), m_szBuffer{}, m_szTextureSplatingInfoData_SaveName{}, m_iSelectTextureSplatingInfoData{}
	, m_vecTextureSplatingInfoDataName{}
	, m_szLevelTypeName{}
	, m_szCameraCinematicSequence_SaveName{}
{
	for (_uint i = 0; i < ENUM_TO_UINT(EClientLevelType::END); ++i)
	{
		string strLevelName = ClientleveltypeToString((EClientLevelType)i);
		::strncpy_s(m_szLevelTypeName[i] , MAX_PATH , strLevelName.c_str() , MAX_PATH );
	}
}

HRESULT CPanel_MapTool::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pMapToolManager = CMapToolManager::GetInstance();


	m_pCameraMan = m_pGameInstance->Get_MainCamera();
	m_pCamera = m_pCameraMan->Get_Component<CCamera>();

	if (!m_pCamera) return E_FAIL;
	if (!m_pCameraMan) return E_FAIL;

	Safe_AddRef(m_pMapToolManager);
	Safe_AddRef(m_pGameInstance);


	if(FAILED(Make_DefaultTexture()))
		return E_FAIL;

	if (FAILED(Update_TextureSplatingInfoDataName()))
		return E_FAIL;


	return S_OK;
}



HRESULT CPanel_MapTool::Update_TextureSplatingInfoDataName()
{
	m_vecTextureSplatingInfoDataName.clear();
	m_vecTextureSplatingInfoDataName.reserve(m_pMapToolManager->m_mapTextureSplatingInfoDatas.size());

	for (auto& Pair : m_pMapToolManager->m_mapTextureSplatingInfoDatas)
	{
		m_vecTextureSplatingInfoDataName.push_back(Engine_Utils::ToString(Pair.first));
	}

	return S_OK;
}




HRESULT CPanel_MapTool::Make_DefaultTexture()
{
	uint32_t pixelColor = 0xFFFFFFFF;
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = 1;            
	tDesc.Height = 1;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tDesc.SampleDesc.Count = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT; 
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData = {};
	tData.pSysMem = &pixelColor;       
	tData.SysMemPitch = sizeof(uint32_t); 

	ID3D11Texture2D* pTexture = nullptr;
	if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexture)))
		return E_FAIL;

	ID3D11ShaderResourceView* pSRV = nullptr;
	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, nullptr, &pSRV)))
	{
		pTexture->Release();
		return E_FAIL;
	}

	pTexture->Release();

	m_pDefaultSRV = pSRV;

	return S_OK;
}

HRESULT CPanel_MapTool::Render(CToolObject* pGo)
{
	ImGui::Begin(" Map Tool Window ");

	if (ImGui::CollapsingHeader(" Camera Setting "))
	{
		if (FAILED(Render_CameraSetting()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}
	if (ImGui::CollapsingHeader(" Ray Setting "))
	{
		if (FAILED(Render_RaySetting()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}

	if (ImGui::CollapsingHeader(" Map Texture Setting "))
	{
		if (FAILED(Render_SplatingTextureSetting()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}

	if (ImGui::CollapsingHeader(" Save Scene Data Setting "))
	{
		if (FAILED((Render_SaveLevelDataSetting())))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}

	if (ImGui::CollapsingHeader(" Camera Cinematic Sequnce "))
	{
		if (FAILED((Render_CameraCinematicSequnce())))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}

	if (ImGui::CollapsingHeader(" Map Env Data Setting "))
	{
		if (FAILED((Render_EnvSetting())))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}

	if (ImGui::CollapsingHeader(" Map Tool Setting "))
	{
		if (FAILED(Render_CheckAndBind()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
		if (FAILED(Render_MakeMapObjectSetting()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}


	ImGui::End();

	Render_PreViewInfo();

	return S_OK;
}

HRESULT CPanel_MapTool::Render_SplatingTextureSetting()
{
	if (m_pMapToolManager == nullptr) return E_FAIL;


#pragma region Texture Splating Info Data Save


	/* 미리 저장된 데이터 모음 */

	if (m_vecTextureSplatingInfoDataName.empty())
		ImGui::Text(" Pre Save Texture Splating Info Data is Empty ");
	else
	{
		ImGui::SeparatorText(" Texture Splating Info Data List ");

		string CurSelectTextureSplatingInfoDataName = m_vecTextureSplatingInfoDataName[m_iSelectTextureSplatingInfoData];
		if (ImGui::BeginCombo("##TextureSplatingInfoDatas", CurSelectTextureSplatingInfoDataName.c_str()))
		{
			for (size_t i = 0; i < m_vecTextureSplatingInfoDataName.size(); ++i)
			{
				_bool isSelected = i == m_iSelectTextureSplatingInfoData;
				if (ImGui::Selectable(m_vecTextureSplatingInfoDataName[i].c_str(), isSelected))
					m_iSelectTextureSplatingInfoData = static_cast<_int>(i);
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::Button(" [ Load ] Texture Splating Info Data "))
		{
			m_pMapToolManager->Load_TextureSplatingInfoData(Engine_Utils::ToWString(CurSelectTextureSplatingInfoDataName));
		}

		ImGui::Separator();

		if (ImGui::Button(" [ Delete ] Texture Splating Info Data "))
		{
			/* 저장된 데이터들 삭제시켜준다 */
			m_pMapToolManager->Delete_TextureSplatingInfoData(Engine_Utils::ToWString(CurSelectTextureSplatingInfoDataName));
			Update_TextureSplatingInfoDataName();
		}

		ImGui::Separator();

	}


	/*----------------------- */

	ImGui::SeparatorText(" Save Texture Splating Info Data ");

	ImGui::InputText(" Save Name ", m_szTextureSplatingInfoData_SaveName, MAX_PATH);
	if (ImGui::Button(" Save Texture Splating Info Data "))
	{
		m_pMapToolManager->Save_TextureSplatingInfoData(Engine_Utils::ToWString(m_szTextureSplatingInfoData_SaveName));
		Update_TextureSplatingInfoDataName();
	}

	ImGui::Separator();

#pragma endregion


#pragma region [Base] Texture Binding

	ImGui::SeparatorText(" Reset Texture Splating Info ");

	if (ImGui::Button(" Reset Texture Splating Info "))
	{
		m_pMapToolManager->m_tTextureSplattingInfo.Free();
		m_pMapToolManager->Bind_MapTexture();
		m_pMapToolManager->Bind_Mix_RGBA_Info();
		return S_OK;
	}


	ImGui::Separator();

	if (ImGui::TreeNode("Bind [ Base ] Texture##Base Texture"))
	{
		m_strBuffer = m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture == nullptr ? "" : Engine_Utils::ToString(m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture->Get_Name());
		ImGui::Text(" Bind [ Base ] Texture => [ %s ] ", m_strBuffer.c_str());
		ImGui::SameLine();
		if (ImGui::ImageButton(" Bind Texture##Base", m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture == nullptr ? (ImTextureID)m_pDefaultSRV :
			(ImTextureID)m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture->Get_SRV(), ImVec2(32, 32)))
		{
			m_pMapToolManager->m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture;
			m_pMapToolManager->m_isTexArraySelect = false;
			ImGui::OpenPopup("Texture_Select_Modal");
		}

		Select_MapTexture();


		ImGui::TreePop();
	}

#pragma endregion 

#pragma region Mix [Splating Tile] Texture [DH , NBR] Binding

	if (ImGui::TreeNode(" Bind [ Splating DH , NBR ] Tile Texture  "))
	{
		/* 개수를 미리 초기화시켜주기 */
		m_iBuffer = 0;

		if (m_pMapToolManager->m_tTextureSplattingInfo.pMix_DH_Tile_Texture)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			m_pMapToolManager->m_tTextureSplattingInfo.pMix_DH_Tile_Texture->Get_SRV()->GetDesc(&desc);
			m_iBuffer = desc.Texture2DArray.ArraySize;
		}

		/* Texture Array 2D Count */
		ImGui::Text(" DH , NBR Text Array Count => [ %d ] ", m_iBuffer);
		ImGui::NewLine();

		/* DH Binding */
		m_strBuffer = m_pMapToolManager->m_tTextureSplattingInfo.pMix_DH_Tile_Texture == nullptr ? "" : Engine_Utils::ToString(m_pMapToolManager->m_tTextureSplattingInfo.pMix_DH_Tile_Texture->Get_Name());
		ImGui::Text(" Bind [ DH ] Texture => [ %s ] ", m_strBuffer.c_str());
		ImGui::SameLine();
		if (ImGui::ImageButton(" Bind Texture##Mix_DH ", m_pMapToolManager->m_tTextureSplattingInfo.pMix_DH_Tile_Texture == nullptr ? (ImTextureID)m_pDefaultSRV :
			(ImTextureID)m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices[0], ImVec2(32, 32)))
		{
			m_pMapToolManager->m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.pMix_DH_Tile_Texture;
			m_pMapToolManager->m_isTex_DH_ArraySelect = true;
			m_pMapToolManager->m_isTexArraySelect = true;
			ImGui::OpenPopup("Texture_Select_Modal");
		}

		ImGui::NewLine();

		/* NBR Binding  */
		m_strBuffer = m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture == nullptr ? "" : Engine_Utils::ToString(m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture->Get_Name());
		ImGui::Text(" Mix [ NRB ] Texture => [ %s ] ", m_strBuffer.c_str());
		ImGui::SameLine();
		if (ImGui::ImageButton(" Bind Texture##Mix_NBR ", m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture == nullptr ? (ImTextureID)m_pDefaultSRV :
			(ImTextureID)m_pMapToolManager->m_tTextureSplattingInfo.vecNBRTextureArraySlices[0], ImVec2(32, 32)))
		{
			m_pMapToolManager->m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture;
			m_pMapToolManager->m_isTexArraySelect = true;
			m_pMapToolManager->m_isTex_NBR_ArraySelect = true;
			ImGui::OpenPopup("Texture_Select_Modal");
		}


		Select_MapTexture();

		ImGui::TreePop();
	}

#pragma endregion

#pragma region RGBA Info Setting

	if (ImGui::TreeNode("RGBA Info Setting##RGBA Info Setting"))
	{
		if (ImGui::Button(" Add RGBA Info "))
		{
			m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.Add_Texture_And_Data();
			m_pMapToolManager->Bind_Mix_RGBA_Info();
		}
		
		_int iDeleteIndex{ -1 };
		_bool isbOpenTileSelectPopup	{false};
		_bool isRGBATextureBinding		{false};
		for (_int i = 0; i < m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count; ++i)
		{
			ImGui::PushID(i);
			sprintf_s(m_szBuffer, MAX_PATH, "RGBA Info [ %d ]", i);
			if (ImGui::TreeNode(m_szBuffer))
			{
				ImGui::Separator();
				if (ImGui::Button("Delete"))
					iDeleteIndex = i;
				ImGui::Separator();
				isRGBATextureBinding = m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i] == nullptr ? false : true;

#pragma region RGBA Texture Binding

				/* RGBA Texture Binding */
				sprintf_s(m_szBuffer, MAX_PATH, " Bind [ RGBA ] Texture => [ %s ] ", isRGBATextureBinding ?
					Engine_Utils::ToString(m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i]->Get_Name()).c_str() : "");
				ImGui::Text(m_szBuffer);

				if (ImGui::ImageButton(" Bind Texture##Mix_RGBA ", isRGBATextureBinding ? (ImTextureID)m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i]->Get_SRV() : (ImTextureID)m_pDefaultSRV,ImVec2(32, 32)))
				{
					m_pMapToolManager->m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i];
					m_pMapToolManager->m_isTexArraySelect = false;
					ImGui::OpenPopup("Texture_Select_Modal");
				}
#pragma endregion

#pragma region RGBA Data Settiong

				/* RGBA Texture가 Binding 되어있지 않다면 */
				if (!isRGBATextureBinding) { ImGui::Text(" RGBA Texture is None Binding..... "); }
				/* DH , NBR Texture 가 Binding 되어있지 않다면 */
				else if (m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices.empty()) { ImGui::Text(" DH , NBR Texture is None Binding....."); }
				else
				{
					ImVec2 ImageSize = ImVec2(48, 48);
					float fGroupSpacing = 25.0f;

					_bool isTriggerOpenPopup{ false };

					MIX_RGBA_DATA& CurrentData = m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.vecMix_RGBA_Data[i];

					/* Red Channel */
					ImGui::PushID("Red");
					Render_Single_Channel_Setting("RED", ImVec4(0.f, 0.3f, 0.f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_R],CurrentData.fRGBA_Mix_Forces[CHANNEL_R], CurrentData.fRGBA_Mix_Height_Forces[CHANNEL_R],CurrentData.iUseFlags[CHANNEL_R],&isbOpenTileSelectPopup); // 멤버변수 등
					ImGui::PopID();


					ImGui::SameLine();

					/* Green Channel */
					ImGui::PushID("Green");
					Render_Single_Channel_Setting("Green", ImVec4(0.f, 1.0f, 0.f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_G], CurrentData.fRGBA_Mix_Forces[CHANNEL_G], CurrentData.fRGBA_Mix_Height_Forces[CHANNEL_G], CurrentData.iUseFlags[CHANNEL_G], &isbOpenTileSelectPopup); // 멤버변수 등
					ImGui::PopID();

					ImGui::SameLine();

					/* Blue Channel */
					ImGui::PushID("Blue");
					Render_Single_Channel_Setting("Blue", ImVec4(0.f, 0.f, 1.0f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_B], CurrentData.fRGBA_Mix_Forces[CHANNEL_B], CurrentData.fRGBA_Mix_Height_Forces[CHANNEL_B],CurrentData.iUseFlags[CHANNEL_B], &isbOpenTileSelectPopup); // 멤버변수 등
					ImGui::PopID();

					ImGui::SameLine();

					/* Alpha Channel */
					ImGui::PushID("Alpha");
					Render_Single_Channel_Setting("Alpha", ImVec4(1.f, 1.f, 1.f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_A], CurrentData.fRGBA_Mix_Forces[CHANNEL_A], CurrentData.fRGBA_Mix_Height_Forces[CHANNEL_A],CurrentData.iUseFlags[CHANNEL_A], &isbOpenTileSelectPopup); // 멤버변수 등
					ImGui::PopID();

					if (isbOpenTileSelectPopup)
						ImGui::OpenPopup("Mix_Texture_Select");
				}


				Select_MapTexture();

				Select_MiXTextureIndex();

#pragma endregion


				ImGui::TreePop();
			}

			if(iDeleteIndex != -1)
			{
				m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.Delete_Texture_And_Data(iDeleteIndex);
				m_pMapToolManager->Bind_Mix_RGBA_Info();
			}

			ImGui::PopID();
		}

		ImGui::TreePop();
	}


#pragma endregion

	return S_OK;
}


HRESULT CPanel_MapTool::Render_Single_Channel_Setting(const char* szLabel, const ImVec4& vColor, OUT int& iConnectedIndex, OUT float& fForce, OUT float& fHeightForce, int& iFlag, OUT bool* pIsOpenPopup)
{
	ImGui::BeginGroup(); // 그룹 시작
	{
		_bool isFixInfo{};
		// 1. 라벨 (색상 입힘)
		ImGui::TextColored(vColor, "[ %s ]", szLabel);

		// 2. 미리보기 이미지 (현재 선택된 타일)
		// DH 텍스처 배열의 슬라이스 뷰를 가져옴 (범위 체크 필수)
		ID3D11ShaderResourceView* pPreviewSRV = nullptr;
		if (iConnectedIndex < m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices.size())
			pPreviewSRV = m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices[iConnectedIndex];
		else
			pPreviewSRV = m_pDefaultSRV; // 혹은 에러 이미지

		// 3. 이미지 버튼 (클릭 시 팝업 열기)
		if (ImGui::ImageButton("##TileBtn", (ImTextureID)pPreviewSRV, ImVec2(50, 50)))
		{
			*pIsOpenPopup = true;
			m_pSelectMixTileTextureIndex = &iConnectedIndex;
		}

		// 정보 표시 및 조작
		ImGui::PushItemWidth(60); // 입력창 너비 고정

		if (ImGui::DragFloat("Mix Force##Mix Force", &fForce, 0.1f, 0.1f, 100.0f, "T:%.1f"))
			isFixInfo = true;
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Mix Tiling Force");
		}

		if (ImGui::DragFloat("Mix Height Force##Mix Height Force", &fHeightForce, 0.0f, 0.01f, 100.0f , "T:%.1f"))
			isFixInfo = true;
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Mix Tiling Height Force");
		}

		ImGui::PopItemWidth();

		bool bUse = (iFlag != 0);
		if (ImGui::Checkbox("Use", &bUse))
		{
			iFlag = bUse ? 1 : 0;
			isFixInfo = true; 
		}

		ImGui::Text("Idx:%d", iConnectedIndex);


		if (isFixInfo)
			m_pMapToolManager->Bind_Mix_RGBA_Data_And_Count();

	}
	ImGui::EndGroup(); // 그룹 끝

	return S_OK;
}

void CPanel_MapTool::Select_MapTexture()
{
	m_pMapToolManager->Select_MapTexture();
	return;

	_bool isChangeTexture				{false};
	_bool isChangeTextureArray			{false};
	_bool isChange_DH_TextureArray		{false};
	_bool isChange_NBR_TextureArray		{false};
	_bool isCompatible					{false};
	_bool isTextureArray				{false};

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(800, 500));


	if (ImGui::BeginPopupModal("Texture_Select_Modal", NULL, ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("Select a Texture...");


		ImGui::SameLine();
		float clearBtnWidth = 160.0f;
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - clearBtnWidth - 20.0f);

		if (ImGui::Button("Clear Selection (None)", ImVec2(clearBtnWidth, 0)))
		{
			if (m_pMapToolManager->m_ppTargetSlot != nullptr)
			{
				Safe_Release(*m_pMapToolManager->m_ppTargetSlot);
				*m_pMapToolManager->m_ppTargetSlot = nullptr;
				m_pMapToolManager->m_ppTargetSlot = nullptr;
				isChangeTexture = true;
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::Separator();


		float totalWidth = ImGui::GetContentRegionAvail().x;
		float leftPaneWidth = totalWidth * 0.25f; // 25%
		float rightPaneWidth = totalWidth - leftPaneWidth - ImGui::GetStyle().ItemSpacing.x; // 나머지
		float paneHeight = 350.0f;


		ImGui::BeginChild("LeftPane", ImVec2(leftPaneWidth, paneHeight), true);
		{
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "[ Categories ]");
			ImGui::Separator();

			for (auto& pair : m_pMapToolManager->m_umapMapTextures)
			{
				wstring currentCategoryW = pair.first;
				string currentCategoryStr = Engine_Utils::ToString(currentCategoryW);
				bool isSelected = (m_pMapToolManager->m_selectedCategoryName == currentCategoryW);

				if (ImGui::Selectable(currentCategoryStr.c_str(), isSelected))
				{
					m_pMapToolManager->m_selectedCategoryName = currentCategoryW;

				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine(); 

		ImGui::BeginChild("RightPane", ImVec2(rightPaneWidth, paneHeight), true);
		{
			auto iter = m_pMapToolManager->m_umapMapTextures.find(m_pMapToolManager->m_selectedCategoryName);

			if (iter == m_pMapToolManager->m_umapMapTextures.end())
			{
				ImGui::TextDisabled("Please select a category.");
			}
			else
			{
				vector<CTextureBase*>& textureList = iter->second;

				float button_sz = 80.0f;
				float spacing = 10.0f;
				float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

				for (size_t i = 0; i < textureList.size(); ++i)
				{
					CTextureBase* pTex = textureList[i];
					if (pTex == nullptr) continue;

					ID3D11ShaderResourceView* pSRV = pTex->Get_SRV();
					if (pSRV == nullptr) continue;

					ImGui::PushID((int)i); 

					D3D11_SHADER_RESOURCE_VIEW_DESC desc;
					pSRV->GetDesc(&desc);

					isTextureArray = (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2DARRAY);

					isCompatible = (m_pMapToolManager->m_isTexArraySelect == isTextureArray);

					ImGui::BeginGroup();
					{
						if (!isCompatible) ImGui::BeginDisabled(true);

						if (ImGui::ImageButton("##TexBtn", (ImTextureID)pSRV, ImVec2(button_sz, button_sz)))
						{
							if (isCompatible && m_pMapToolManager->m_ppTargetSlot != nullptr)
							{
								Safe_Release(*m_pMapToolManager->m_ppTargetSlot);
								*m_pMapToolManager->m_ppTargetSlot = pTex;
								Safe_AddRef(*m_pMapToolManager->m_ppTargetSlot);

								m_pMapToolManager->m_ppTargetSlot = nullptr;
								isChangeTexture = true;  
								isChangeTextureArray = isTextureArray;
								isChange_DH_TextureArray = m_pMapToolManager->m_isTex_DH_ArraySelect;
								isChange_NBR_TextureArray = m_pMapToolManager->m_isTex_NBR_ArraySelect;
							}
							ImGui::CloseCurrentPopup();
						}

						if (!isCompatible) ImGui::EndDisabled();

						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
						{
							if (isCompatible)
								ImGui::SetTooltip("%s", Engine_Utils::ToString(pTex->Get_Name()).c_str());
							else
								ImGui::SetTooltip("[Incompatible Type]\n%s", Engine_Utils::ToString(pTex->Get_Name()).c_str());
						}

						ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + button_sz); 

						string texName = Engine_Utils::ToString(pTex->Get_Name());
						if (texName.length() > 9)
							texName = texName.substr(0, 9) + "...";

						if (!isCompatible) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.f));

						ImGui::Text("%s", texName.c_str());

						if (!isCompatible) ImGui::PopStyleColor();

						ImGui::PopTextWrapPos();
					}
					ImGui::EndGroup();

					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + spacing + button_sz;

					if (i + 1 < textureList.size() && next_button_x2 < window_visible_x2)
						ImGui::SameLine(0.0f, spacing);

					ImGui::PopID();
				}
			}
		}
		ImGui::EndChild();

		// -----------------------------------------------------------
		// [하단] 취소 버튼
		// -----------------------------------------------------------
		ImGui::Separator();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_pMapToolManager->m_ppTargetSlot = nullptr;
			m_pMapToolManager->m_isTex_DH_ArraySelect = false;
			m_pMapToolManager->m_isTex_NBR_ArraySelect = false;
			m_pMapToolManager->m_isTexArraySelect = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (isChangeTexture)
	{
		m_pMapToolManager->Bind_MapTexture();
		m_pMapToolManager->Bind_Mix_RGBA_Texture();
		if (isChangeTextureArray)
		{
			if (m_pMapToolManager->m_isTex_DH_ArraySelect)
				m_pMapToolManager->Slice_DH_Texture();
			if (m_pMapToolManager->m_isTex_NBR_ArraySelect)
				m_pMapToolManager->Slice_NBR_Texture();

			m_pMapToolManager->m_isTex_DH_ArraySelect = false;
			m_pMapToolManager->m_isTex_NBR_ArraySelect = false;
			m_pMapToolManager->m_isTexArraySelect = false;

		}
	}
}

void CPanel_MapTool::Select_MiXTextureIndex()
{
	// 데이터 없으면 리턴
	if (m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices.empty())
		return;

	// 팝업 시작
	if (ImGui::BeginPopup("Mix_Texture_Select"))
	{
		ImGui::Text("Select Texture Tile");
		ImGui::Separator();

		int iSliceCount = (int)m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices.size();
		int iColCount = 4; // 한 줄에 4개씩

		for (int i = 0; i < iSliceCount; ++i)
		{
			ID3D11ShaderResourceView* pSliceSRV = m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices[i];
			ImGui::BeginGroup();
			{
				ImGui::PushID(i);

				// 이미지 버튼
				if (ImGui::ImageButton("", (ImTextureID)pSliceSRV, ImVec2(64, 64)))
				{
					if (m_pSelectMixTileTextureIndex != nullptr)
					{
						*m_pSelectMixTileTextureIndex = i;
						m_pMapToolManager->Bind_Mix_RGBA_Data_And_Count();
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::PopID();

				// 텍스트 (이미지 바로 아래에 찍힘)
				ImGui::Text("Index %d", i);
			}
			ImGui::EndGroup();
			if ((i + 1) % iColCount != 0 && (i + 1) < iSliceCount)
			{
				ImGui::SameLine();
			}
		}
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));

		// 취소 버튼 (가로 꽉 차게)
		if (ImGui::Button(" Cancel ", ImVec2(-1, 0)))
		{; 
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopStyleColor(3); // 스타일 복구

		ImGui::EndPopup(); // 팝업 끝
	}
}


HRESULT CPanel_MapTool::Render_RaySetting()
{
	ImGui::NewLine();
	ImGui::DragFloat(" Ray Range ", &m_pMapToolManager->m_fMouseRange, 0.1f);
	ImGui::NewLine();
	ImGui::DragFloat(" Mouse Wheel Speed ", &m_pMapToolManager->m_fMouseWheelSpeed, 0.1f);

	return S_OK;
}

HRESULT CPanel_MapTool::Render_CheckAndBind()
{
	ImGui::SeparatorText(" Chekc Static & Instance Model  Merget InstanceModel ");

	/* UE Model Data 전용 */
	if (ImGui::Button(" Bind Staitc & Instance Model To Instance Model "))
		m_pMapToolManager->Check_And_Bind_FromUE();


	ImGui::Separator();
	return S_OK;
}


HRESULT CPanel_MapTool::Render_MakeMapObjectSetting()
{
	ImGui::SeparatorText(" Map Object Batch Mode ");

#pragma region Empalce Type

	m_iBuffer = static_cast<_int>(m_pMapToolManager->m_eMapToolEmplaceType);
	m_strBuffer = EMapTool_EmplaceType_ToString(static_cast<EMapTool_EmplaceType>(m_iBuffer));
	if (ImGui::BeginCombo("##MapObjectEmplaceMode", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EMapTool_EmplaceType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(EMapTool_EmplaceType_ToString(static_cast<EMapTool_EmplaceType>(i)).c_str(), &isSelected))
				m_pMapToolManager->m_eMapToolEmplaceType = static_cast<EMapTool_EmplaceType>(i);
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

#pragma endregion

	ImGui::Separator();

	ImGui::SeparatorText(" Map Object Batch Mode ");

#pragma region Batch Mode

	ImGui::NewLine();

	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MapToolObjectBatchMode());
	m_strBuffer = MapToolObjectBatchMode_ToString(static_cast<EMapToolObjectBatchMode>(m_iBuffer));
	if (ImGui::BeginCombo("##MapObjectBatchMode", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EMapToolObjectBatchMode::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(MapToolObjectBatchMode_ToString(static_cast<EMapToolObjectBatchMode>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MapToolObjectBatchMode(static_cast<EMapToolObjectBatchMode>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}


	if (ImGui::TreeNode(" Brush Option Setting "))
	{
		m_pMapToolManager->m_tBrushModeOption.Render_ImGui();
		ImGui::TreePop();
	}

	ImGui::NewLine();

	ImGui::Separator();

#pragma endregion

#pragma region Discard Color

	if (ImGui::TreeNode(" Dicard Color Setting "))
	{
		ImGui::Spacing();
		ImGui::Text("Adjust Values (Speed: 0.005)");

		// Vec4 구조체의 시작 주소를 float 포인터로 받아옵니다.
		// 배열처럼 [0], [1], [2], [3] 으로 R, G, B, A에 접근할 수 있습니다.
		float* pColor = (float*)&m_pMapToolManager->m_vDiscardColor;

		bool bIsChanged = false;

		// 한 줄에 하나씩! 싹싹 잘 보이게 DragFloat를 4번 따로 만듭니다.
		bIsChanged |= ImGui::DragFloat("R (Red)", &pColor[0], 0.005f, 0.0f, 1.0f, "%.3f");
		bIsChanged |= ImGui::DragFloat("G (Green)", &pColor[1], 0.005f, 0.0f, 1.0f, "%.3f");
		bIsChanged |= ImGui::DragFloat("B (Blue)", &pColor[2], 0.005f, 0.0f, 1.0f, "%.3f");
		bIsChanged |= ImGui::DragFloat("A (Alpha)", &pColor[3], 0.005f, 0.0f, 1.0f, "%.3f");

		if (bIsChanged)
		{
			m_pMapToolManager->Set_GPU_DiscardColor();
		}

		ImGui::Spacing();

		// 직관적으로 무슨 색인지 눈으로 확인하기 위한 컬러 픽커 (선택 사항)
		ImGui::ColorEdit4("Preview Color", pColor, ImGuiColorEditFlags_NoInputs);

		ImGui::Spacing();
		ImGui::Separator();

		ImGui::TreePop();
	}

#pragma endregion


	ImGui::SeparatorText("Make Map Object Setting");

	ImGui::NewLine();


#pragma region Make Section Number
	ImGui::SeparatorText(" Section Number ");

	if (ImGui::InputInt("##MakeObjectSection Number", &m_pMapToolManager->m_iMakeSectionNumber))
	{
		if (m_pMapToolManager->m_iMakeSectionNumber < 0)
			m_pMapToolManager->m_iMakeSectionNumber = 0;
	}

	ImGui::Separator();
#pragma endregion

	ImGui::SeparatorText(" Level Type ");

#pragma region Draw Type

	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MakeMapObejctClientLevelType());
	m_strBuffer = ClientleveltypeToString(static_cast<EClientLevelType>(m_iBuffer));
	if (ImGui::BeginCombo("##ClientLevelType", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EClientLevelType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(ClientleveltypeToString(static_cast<EClientLevelType>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MakeMapObjectClientLevelType(static_cast<EClientLevelType>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

#pragma endregion

	ImGui::Separator();


	ImGui::SeparatorText(" Draw Type ");

#pragma region Draw Type

	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MakeMapObjectDrawType());
	m_strBuffer = EMapObject_DrawType_ToString(static_cast<EMapObject_DrawType>(m_iBuffer));
	if (ImGui::BeginCombo("##DrawType", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EMapObject_DrawType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(EMapObject_DrawType_ToString(static_cast<EMapObject_DrawType>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MakeMapObjectDrawType(static_cast<EMapObject_DrawType>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}


#pragma endregion

	ImGui::Separator();

	ImGui::SeparatorText(" Cliet Make Path Setting ");

#pragma region Client Make Path Combo
	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MakeMapObjectClientMakePath());
	m_strBuffer = ClientMakePath_ToString(static_cast<EClientMakePath>(m_iBuffer));
	if (ImGui::BeginCombo("##ClientMakePath", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EClientMakePath::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(ClientMakePath_ToString(static_cast<EClientMakePath>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MakeMapObjectClientMakePath(static_cast<EClientMakePath>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
#pragma endregion

	ImGui::Separator();


	return S_OK;
}

HRESULT CPanel_MapTool::Render_EnvSetting()
{
	auto& tCB_EnvData = m_pMapToolManager->m_pLevelData->m_tCB_EnvData;

	ImGui::SeparatorText(" Wind Setting ");

	ImGui::NewLine();

	ImGui::Text(" Direction ");
	if (ImGui::DragFloat3("##Wind Direction", &tCB_EnvData.vWindDirection.x, 0.01f, -1.f, 1.f, "%.2f"))
	{
		m_pMapToolManager->Set_GPU_EnvData();
	}

	ImGui::Text(" Power ");
	if (ImGui::DragFloat("##Wind Power", &tCB_EnvData.fWindPower, 0.01f, 0.f, 100.f, "%.2f"))
	{
		m_pMapToolManager->Set_GPU_EnvData();
	}

	ImGui::NewLine();

	ImGui::Separator();


	return S_OK;
}

HRESULT CPanel_MapTool::Render_CameraCinematicSequnce()
{
	if (m_pMapToolManager->m_pCamCinematicSequence == nullptr) return E_FAIL;

	auto& pCamCinematicSequence = m_pMapToolManager->m_pCamCinematicSequence;


#pragma region Cam Cinematic Sequence Name List

	ImGui::SeparatorText("Camera Cinematic Sequence List");

	if (m_pMapToolManager->m_vecCamCinematicSequenceNames.empty())
		m_strBuffer = "Empty";
	else
	{
		if (m_pMapToolManager->m_vecCamCinematicSequenceNames.size() < m_iSelectCamCinematicSequenceName)
			m_iSelectCamCinematicSequenceName = 0;
		m_strBuffer = m_pMapToolManager->m_vecCamCinematicSequenceNames[m_iSelectCamCinematicSequenceName];
	}

	if (ImGui::BeginCombo("##Cam Cinematic Sequence Name List", m_strBuffer.c_str()))
	{
		for (_uint i = 0; i < static_cast<_uint>(m_pMapToolManager->m_vecCamCinematicSequenceNames.size()); ++i)
		{
			bool isSelected = i == m_iSelectCamCinematicSequenceName;
			if (ImGui::Selectable(m_pMapToolManager->m_vecCamCinematicSequenceNames[i].c_str() , &isSelected))
			{
				m_iSelectCamCinematicSequenceName = i;
				m_strBuffer = m_pMapToolManager->m_vecCamCinematicSequenceNames[i];
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Load Camera Cinematic Sequence"))
	{
		if (FAILED(m_pMapToolManager->Load_Camera_Cinematic_Sequence(Engine_Utils::ToWString(m_strBuffer))))
		{
			MSG_BOX(" Load Camera Cinematic Sequence 실패");
		}
	}

#pragma endregion
	
	ImGui::Separator();

	ImGui::NewLine();

	ImGui::SeparatorText(" Camera Cinematic Sequence Info ");


	ImGui::NewLine();

	if (ImGui::Button(" Play Cinematic Test"))
	{
		m_pGameInstance->Play_CameraCinematic(m_pMapToolManager->m_pCamCinematicSequence);
	}

	ImGui::NewLine();


	if (ImGui::Button("Save Camera Cinematic Sequence [ Data ] "))
	{
		if (FAILED(m_pMapToolManager->Save_Camera_Cinematic_Sequence(Engine_Utils::ToWString(pCamCinematicSequence->strDataName))))
		{
			MSG_BOX(" Save Camera Cinematic Sequence 실패");
		}
	}

	if (ImGui::Button("Save Camera Cinematic Sequence  [ Json ] "))
	{
		if (FAILED(m_pGameInstance->GameDataManager_Save_CameraCinematicSequence()))
		{
			MSG_BOX(" Save Camera Cinematic Sequence 실패");
		}
	}

	ImGui::Separator();

	if (ImGui::Button(" Reset Camera Cinematic Sequence "))
	{
		/* 전체 리셋 */
		pCamCinematicSequence->Reset();
	}


	ImGui::NewLine();

	_bool isDebugRender = pCamCinematicSequence->isDebugRender;
	ImGui::Checkbox("Debug Render##Debug Render",&pCamCinematicSequence->isDebugRender);

	ImGui::NewLine();

	ImGui::InputText("Name" , &pCamCinematicSequence->strDataName);

	ImGui::NewLine();

	// =========================================================
		// [추가] Start Cinematic Global Events UI
		// =========================================================
	ImGui::SeparatorText(" Start Cinematic Events ");

	if (ImGui::Button("Add Start Event"))
	{
		// _uint 배열이므로 "NONE" 문자열 대신 숫자 0 (혹은 ENUM_TO_UINT(NONE))을 넣습니다.
		pCamCinematicSequence->vecStartCinematic_GlobalEventIndex.push_back(0);
	}

	ImGui::PushID("StartEvents_Group"); // ID 겹침 방지용 그룹
	for (int i = 0; i < pCamCinematicSequence->vecStartCinematic_GlobalEventIndex.size(); ++i)
	{
		ImGui::PushID(i);

		// string 변환 없이 벡터에 있는 정수값을 바로 꺼내옵니다.
		int iBuffer = static_cast<int>(pCamCinematicSequence->vecStartCinematic_GlobalEventIndex[i]);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40.f);

		// 콤보박스에서 값이 변경되면...
		if (ImGui::Combo("##StartEventCombo", &iBuffer, g_szGlobalBroadCastType, (int)EGlobal_Broadcast_Type::END))
		{
			// 변경된 정수값을 다시 벡터에 쏙 집어넣습니다.
			pCamCinematicSequence->vecStartCinematic_GlobalEventIndex[i] = static_cast<_uint>(iBuffer);
		}

		ImGui::SameLine();
		if (ImGui::Button("X"))
		{
			// erase 할 때도 이름 잘 맞춰서 삭제!
			pCamCinematicSequence->vecStartCinematic_GlobalEventIndex.erase(pCamCinematicSequence->vecStartCinematic_GlobalEventIndex.begin() + i);
			ImGui::PopID();
			--i;
			continue;
		}
		ImGui::PopID();
	}
	ImGui::PopID(); // StartEvents_Group 해제

	ImGui::NewLine();

	// =========================================================
	// [추가] End Cinematic Global Events UI
	// =========================================================
	ImGui::SeparatorText(" End Cinematic Events ");

	if (ImGui::Button("Add End Event"))
	{
		// 여기도 마찬가지로 숫자 0 삽입
		pCamCinematicSequence->vecEndCinematic_GlobalEventIndex.push_back(0);
	}

	ImGui::PushID("EndEvents_Group"); // ID 겹침 방지용 그룹
	for (int i = 0; i < pCamCinematicSequence->vecEndCinematic_GlobalEventIndex.size(); ++i)
	{
		ImGui::PushID(i);

		// 정수값 바로 꺼내기
		int iBuffer = static_cast<int>(pCamCinematicSequence->vecEndCinematic_GlobalEventIndex[i]);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40.f);

		if (ImGui::Combo("##EndEventCombo", &iBuffer, g_szGlobalBroadCastType, (int)EGlobal_Broadcast_Type::END))
		{
			// 정수값 바로 갱신
			pCamCinematicSequence->vecEndCinematic_GlobalEventIndex[i] = static_cast<_uint>(iBuffer);
		}

		ImGui::SameLine();
		if (ImGui::Button("X"))
		{
			// 지울 때도 Index 벡터에서 삭제!
			pCamCinematicSequence->vecEndCinematic_GlobalEventIndex.erase(pCamCinematicSequence->vecEndCinematic_GlobalEventIndex.begin() + i);
			ImGui::PopID();
			--i;
			continue;
		}
		ImGui::PopID();
	}
	ImGui::PopID(); // EndEvents_Group 해제

	ImGui::NewLine();
	ImGui::Separator();
	// =========================================================

	ImGui::Separator();

	ImGui::SeparatorText(" Add Buttons");

	if (ImGui::Button(" Add Default "))
	{
		pCamCinematicSequence->Add_KeyFrameData();
	}

	ImGui::SameLine();

	if (ImGui::Button(" Add Copy Camera "))
	{
		pCamCinematicSequence->Copy_Camera(m_pGameInstance->Get_MainCamera());
	}

	ImGui::Separator();

	ImGui::NewLine();

	if (pCamCinematicSequence->vecCamKeyFrameData.empty())
	{
		ImGui::Text(" Camera KeyFrame Data is Empty..");
	}
	else
	{
		_int iDeleteIndex{ -1 };
		_int iResetIndex{ -1 };
		_int iCopyCameraIndex{ -1 };

		// 키프레임 리스트 순회 (삭제 시 안전하게 인덱스를 다루기 위해 for 루프를 수동 증감합니다)
		for (_uint i = 0; i < pCamCinematicSequence->vecCamKeyFrameData.size(); )
		{
			// ★ 아주 중요: ImGui는 이름이 같으면 겹치기 때문에, 반드시 고유 ID를 푸시해야 합니다.
			ImGui::PushID(i);

			_int iDeleteIndex{-1};

			string strTreeNodeName = "KeyFrame[" + std::to_string(i) + "]";

			// TreeNodeEx를 써서 기본적으로 펼쳐져 있게 하거나 닫혀있게 설정 가능
			bool bNodeOpen = ImGui::TreeNodeEx(strTreeNodeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

			// 트리 노드와 같은 줄 오른쪽 끝에 삭제 버튼 배치
			ImGui::SameLine();

			/* 중간사이에 끼워넣기가능 */
			if (ImGui::Button("Insert"))
			{
				pCamCinematicSequence->Insert_KeyFrameData(i);
			}
			if (ImGui::Button("Insert(Cam)"))
			{
				pCamCinematicSequence->Insert_KeyFrameData(i,m_pGameInstance->Get_MainCamera());
			}
			if (ImGui::Button("Delete"))
			{
				if(iDeleteIndex == -1)
					iDeleteIndex = i;
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				if (iResetIndex == -1)
					iResetIndex = i;
			}

			ImGui::SameLine();
			if (ImGui::Button("Copy Camera"))
			{
				if (iCopyCameraIndex == -1)
					iCopyCameraIndex = i;
			}

			if (ImGui::Button("Move To Camera"))
			{
				m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Set_WorldMatrix(pCamCinematicSequence->vecCamKeyFrameData[i].Get_WorldMatrix());
			}

			// 노드가 펼쳐져 있을 때만 내부 UI 렌더링
			if (bNodeOpen)
			{
				// 코드가 길어지니 레퍼런스로 받아옵니다.
				auto& KeyFrame = pCamCinematicSequence->vecCamKeyFrameData[i];

				ImGui::SeparatorText("Time & FOV Info");
				ImGui::DragFloat("Duration (Sec)", &KeyFrame.fDuration, 0.1f, 0.f, 100.f, "%.2f");
				ImGui::DragFloat("Hold Time (Sec)", &KeyFrame.fHoldTime, 0.1f, 0.f, 100.f, "%.2f");
				ImGui::DragFloat("FOV", &KeyFrame.fFov, 0.5f, 10.f, 180.f, "%.1f");

				ImGui::SeparatorText("Move Info");
				/* Enum 콤보박스 (TODO: 실제 Enum 문자열에 맞게 수정 필요!) */
				ImGui::Combo("Move Base Target", (int*)&KeyFrame.eMoveBaseTarget,g_szCinematicTarget,(int)ECinematicTarget::END);
				if(ImGui::InputInt("Move Bone Index", &KeyFrame.iMoveBaseTargetBoneIndex))
				{
					if (KeyFrame.iMoveBaseTargetBoneIndex < -1)
						KeyFrame.iMoveBaseTargetBoneIndex = -1;
				}
				ImGui::DragFloat3("Position", (float*)&KeyFrame.vPosition, 0.1f);

				ImGui::SeparatorText("LookAt Info");
				/* Enum 콤보박스 (TODO: 실제 Enum 문자열에 맞게 수정 필요!) */
				ImGui::Combo("LookAt Target", (int*)&KeyFrame.eLookAtTarget , g_szCinematicTarget, (int)ECinematicTarget::END);
				if (ImGui::InputInt("LookAt Bone Index", &KeyFrame.iLookAtBoneIndex))
				{
					if (KeyFrame.iLookAtBoneIndex < -1)
						KeyFrame.iLookAtBoneIndex = -1;
				}
				ImGui::DragFloat3("LookAt Offset", (float*)&KeyFrame.vLookAtOffset, 0.1f);
				ImGui::DragFloat3("Pitch / Yaw / Roll", (float*)&KeyFrame.vPitchYawRoll, 0.1f);

				ImGui::SeparatorText("Lerp Types");
				// 배열의 주소(g_szLerpTypes)와 배열의 크기(END)를 넘겨주면 알아서 콤보박스가 완성됩니다!
				ImGui::Combo("Move Lerp", (int*)&KeyFrame.eMoveLerpType, g_szLerpTypes, (int)Engine::ELerpType::END);
				ImGui::Combo("LookAt Lerp", (int*)&KeyFrame.eLookAtLerpType, g_szLerpTypes, (int)Engine::ELerpType::END);
				ImGui::Combo("Fov Lerp", (int*)&KeyFrame.eFovLerpType, g_szLerpTypes, (int)Engine::ELerpType::END);



				ImGui::SeparatorText("Events");

				// TreeNode가 열렸을 때만 안쪽 내용을 그리고 TreePop을 합니다.
				if (ImGui::TreeNode("On Reach Events"))
				{
					// 1. 이벤트 추가 버튼
					if (ImGui::Button("Add##OnReachEvents"))
					{
						KeyFrame.vecOnReachEventIndex.push_back(0); // 기본값 0으로 추가
					}

					_int iEventToDelete = -1; // 삭제할 이벤트의 인덱스를 기억할 변수

					// 2. 현재 들어있는 이벤트 리스트 순회
					for (_uint j = 0; j < KeyFrame.vecOnReachEventIndex.size(); ++j)
					{
						ImGui::PushID(j); 

						// ImGui는 int 포인터를 쓰기 때문에 _uint를 잠시 int로 캐스팅
						int iEventVal = static_cast<int>(KeyFrame.vecOnReachEventIndex[j]);

						if (ImGui::Combo("##OnReachEvents", &iEventVal, g_szGlobalBroadCastType, (int)EGlobal_Broadcast_Type::END))
						{
							// 정수값 바로 갱신
							KeyFrame.vecOnReachEventIndex[j] = static_cast<_uint>(iEventVal);
						}

						ImGui::SameLine();

						// 3. 개별 삭제 버튼
						if (ImGui::Button("Delte##OnReachEvents"))
						{
							iEventToDelete = j; // 눌린 버튼의 인덱스를 저장
						}

						ImGui::PopID();
					}

					// 4. 루프가 끝난 뒤에 안전하게 삭제 처리 (중간 삭제 완벽 지원)
					if (iEventToDelete != -1)
					{
						KeyFrame.vecOnReachEventIndex.erase(KeyFrame.vecOnReachEventIndex.begin() + iEventToDelete);
					}

					ImGui::TreePop(); // Arrival Events용 TreePop
				}


				// TreeNode가 열렸을 때만 안쪽 내용을 그리고 TreePop을 합니다.
				if (ImGui::TreeNode("Hold Time End Events"))
				{
					// 1. 이벤트 추가 버튼
					if (ImGui::Button("Add##HoldTimeEnd"))
					{
						KeyFrame.vecHoldTimeEndEventIndex.push_back(0); // 기본값 0으로 추가
					}

					_int iEventToDelete = -1; // 삭제할 이벤트의 인덱스를 기억할 변수

					// 2. 현재 들어있는 이벤트 리스트 순회
					for (_uint j = 0; j < KeyFrame.vecHoldTimeEndEventIndex.size(); ++j)
					{
						ImGui::PushID(j); // 이벤트 리스트용 고유 ID 푸시

						// ImGui는 int 포인터를 쓰기 때문에 _uint를 잠시 int로 캐스팅
						int iEventVal = static_cast<int>(KeyFrame.vecHoldTimeEndEventIndex[j]);

						if (ImGui::Combo("##OnReachEvents", &iEventVal, g_szGlobalBroadCastType, (int)EGlobal_Broadcast_Type::END))
						{
							// 정수값 바로 갱신
							KeyFrame.vecHoldTimeEndEventIndex[j] = static_cast<_uint>(iEventVal);
						}

						ImGui::SameLine();

						// 3. 개별 삭제 버튼
						if (ImGui::Button("Delte##OnReachEvents"))
						{
							iEventToDelete = j; // 눌린 버튼의 인덱스를 저장
						}

						ImGui::PopID();
					}

					// 4. 루프가 끝난 뒤에 안전하게 삭제 처리 (중간 삭제 완벽 지원)
					if (iEventToDelete != -1)
					{
						KeyFrame.vecHoldTimeEndEventIndex.erase(KeyFrame.vecHoldTimeEndEventIndex.begin() + iEventToDelete);
					}

					ImGui::TreePop(); // Arrival Events용 TreePop
				}


				ImGui::NewLine();
				ImGui::TreePop(); // TreeNode를 닫아줌
			}

			ImGui::PopID(); // PushID 해제
			++i; // 삭제 버튼이 안 눌렸을 때만 인덱스 증가
		}

		if (iCopyCameraIndex != -1 )
		{
			/* iCopyCameraIndex */
			m_pMapToolManager->m_pCamCinematicSequence->Copy_Camera(m_pGameInstance->Get_MainCamera() , iCopyCameraIndex );
		}

		if (iResetIndex != -1)
		{
			/* CopyCamera */
			m_pMapToolManager->m_pCamCinematicSequence->Reset(iResetIndex);
		}

		if (iDeleteIndex != -1)
		{
			/* Delete */
			m_pMapToolManager->m_pCamCinematicSequence->Delete(iDeleteIndex);
		}

	}


	return S_OK;
}



HRESULT CPanel_MapTool::Render_CameraSetting()
{
	if (ImGui::TreeNode(" S R T "))
	{
		CTransform* pTransfrom = m_pCameraMan->Get_Component<CTransform>();

		if (!pTransfrom) return E_FAIL;

		Matrix WorldMatrix = pTransfrom->Get_WorldMatrix();

		Vec3 vLook = pTransfrom->Get_Info(TRANSFORM_INFO_STATE::LOOK);
		vLook.Normalize();

		Vec3 vPosition = WorldMatrix.Translation();
		Vec3 vRotation = WorldMatrix.ToEuler();
		Vec3 vScale = pTransfrom->Get_Scaled();


		if (ImGui::DragFloat3("Position", &vPosition.x))
			pTransfrom->Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
		ImGui::DragFloat3("Rotation", &vRotation.x);
		if (ImGui::DragFloat3("Scale", &vScale.x))
			pTransfrom->Set_Scale(vScale);

		ImGui::Text(" Look [ %.2f , %.2f , %.2f ] ", vLook.x, vLook.y, vLook.z);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" Info Setting "))
	{
		float fFov = m_pCamera->Get_Fov() * To_DEGREE;
		if (ImGui::DragFloat("Fov#Fov", &fFov))
			m_pCamera->Set_Fov(fFov * TO_RAD);
		float fFar = m_pCamera->Get_Far();
		if (ImGui::DragFloat("Far#Far", &fFar))
			m_pCamera->Set_Far(fFar);

		CTransform* pTransform = m_pCameraMan->Get_Component<CTransform>();

		float fMoveSpeed = pTransform->Get_MovePerSec();
		float fMoveTurnSpeed = pTransform->Get_RotatePerSec();

		if (ImGui::DragFloat(" Move Speed ", &fMoveSpeed, 0.1f))
			pTransform->Set_MovePerSec(fMoveSpeed);

		if (ImGui::DragFloat(" Rotation Speed ", &fMoveTurnSpeed, 0.1f))
			pTransform->Set_RotatePerSec(fMoveTurnSpeed);



		ImGui::TreePop();
	}


	return S_OK;
}

HRESULT CPanel_MapTool::Render_PreViewInfo()
{
	ImGui::Begin(" Preview Info ");

	if (m_pMapToolManager->m_pPreviewMapobject == nullptr)
	{
		ImGui::Text(" Preview Info is Empty ");
		ImGui::End();
		return S_OK;
	}
	
	ImGui::Separator();

	if (ImGui::Button(" Delete "))
	{
		m_pMapToolManager->Delete_Preview();
		ImGui::End();
		return S_OK;
	}

	ImGui::Separator();

	ImGui::NewLine();

	ImGui::SeparatorText(" Model Info ");

	ImGui::Text( " Model Name => [ %s ] " , m_pMapToolManager->m_pPreviewMapobject->Get_ModelFileName().c_str());

	ImGui::Separator();

	Vec3 vScale,vPosition;
	Quat vQuat{};

	m_pMapToolManager->m_pPreviewMapobject->Get_SRT(vScale, vQuat,vPosition);


	if (ImGui::BeginTable("SRT_Quat_Display", 5, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg))
	{
		// 헤더 설정
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
		ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("W", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		// --- Position 행 ---
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Position");
		ImGui::TableSetColumnIndex(1); ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%.2f", vPosition.x);
		ImGui::TableSetColumnIndex(2); ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%.2f", vPosition.y);
		ImGui::TableSetColumnIndex(3); ImGui::TextColored(ImVec4(0.4f, 0.4f, 1.0f, 1.0f), "%.2f", vPosition.z);
		ImGui::TableSetColumnIndex(4); ImGui::TextUnformatted("-"); // Position은 W가 없음

		// --- Rotation 행 (Quaternion) ---
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Quat Rot");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%.4f", vQuat.x);
		ImGui::TableSetColumnIndex(2); ImGui::Text("%.4f", vQuat.y);
		ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", vQuat.z);
		ImGui::TableSetColumnIndex(4); ImGui::Text("%.4f", vQuat.w);

		// --- Scale 행 ---
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Scale");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", vScale.x);
		ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", vScale.y);
		ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", vScale.z);
		ImGui::TableSetColumnIndex(4); ImGui::TextUnformatted("-"); // Scale도 W가 없음

		ImGui::EndTable();
	}


	m_pMapToolManager->DrawImGui_Preview();

	ImGui::End();

	return S_OK;
}

HRESULT CPanel_MapTool::Render_SaveLevelDataSetting()
{
	ImGui::SeparatorText(" Save Scene Data Setting ");

	if (m_pMapToolManager->m_pLevelData == nullptr) return E_FAIL;

	ImGui::NewLine();

	ImGui::SeparatorText("Use Texture Splating Info Name [ None => Don't Use ]");
	ImGui::NewLine();

	
	ImGui::SeparatorText(" Texture Splating Info Data List ");

	string CurSelectTextureSplatingInfoDataName = m_vecTextureSplatingInfoDataName[m_iSelectTextureSplatingInfoData];
	if (ImGui::BeginCombo("##TextureSplatingInfoDatas_2", CurSelectTextureSplatingInfoDataName.c_str()))
	{
		for (size_t i = 0; i < m_vecTextureSplatingInfoDataName.size(); ++i)
		{
			_bool isSelected = i == m_iSelectTextureSplatingInfoData;
			if (ImGui::Selectable(m_vecTextureSplatingInfoDataName[i].c_str(), isSelected))
			{
				m_iSelectTextureSplatingInfoData = static_cast<_int>(i);
				m_pMapToolManager->m_pLevelData->m_strTextureSplatingInfoName = m_vecTextureSplatingInfoDataName[i];
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Text(" Save Texture Splating Info Name => [ %s ] ", m_pMapToolManager->m_pLevelData->m_strTextureSplatingInfoName.c_str());
	ImGui::NewLine();
	if (ImGui::Button(" Don't Use Texture Splating Info "))
		m_pMapToolManager->m_pLevelData->m_strTextureSplatingInfoName = "None";

	ImGui::Separator();


	ImGui::SeparatorText(" Level Type ");

	m_strBuffer = ClientleveltypeToString(m_pMapToolManager->m_pLevelData->m_eClientLevelType);

	if (ImGui::BeginCombo("##LevelType", m_strBuffer.c_str()))
	{
		for (size_t i = 0; i < ENUM_TO_UINT(EClientLevelType::END) ; ++i)
		{
			_bool isSelected = i == ENUM_TO_UINT(m_pMapToolManager->m_pLevelData->m_eClientLevelType);
			if (ImGui::Selectable(m_szLevelTypeName[i], isSelected))
			{
				m_pMapToolManager->m_pLevelData->m_eClientLevelType = (EClientLevelType)i;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	
	ImGui::SeparatorText(" Map Min Max Box Setting ");

	ImGui::NewLine();

	/* Center */

	ImGui::DragFloat3("Center", &m_pMapToolManager->m_pLevelData->m_vMapMinMaxBox_Center.x, 0.1f);
	ImGui::DragFloat3("Extents", &m_pMapToolManager->m_pLevelData->m_vMapMinMaxBox_Extents.x);
	

	ImGui::Separator();

	m_pMapToolManager->m_pLevelData->Draw_ImGui();

	ImGui::Separator();
	return S_OK;
}

void CPanel_MapTool::Update(const _float fTimeDelta)
{


	return;
}


CPanel_MapTool* CPanel_MapTool::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_MapTool* pPanel = new CPanel_MapTool(pLabel,pOwner,pDevice,pDeviceContext);
	if (FAILED(pPanel->Initialize()))
	{
		Safe_Release(pPanel);
		MSG_BOX(" Panel MapTool is failed to Create ");
		return nullptr;
	}
	return pPanel;
}

void CPanel_MapTool::Free()
{
	Super::Free();

	Safe_Release(m_pDefaultSRV);

	Safe_Release(m_pMapToolManager);
	Safe_Release(m_pGameInstance);
}