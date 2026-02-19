#include "pch.h"
#include "Panel_MapTool.h"
#include "GameInstance.h"
#include "MapObject.h"
#include "Camera.h"
#include "CameraMan.h"

CPanel_MapTool::CPanel_MapTool(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext), m_szBuffer{}, m_isTexArraySelect{false}
{
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


#pragma region [Base] Texture Binding

	if (ImGui::TreeNode("Bind [ Base ] Texture##Base Texture"))
	{
		m_strBuffer = m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture == nullptr ? "" : Engine_Utils::ToString(m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture->Get_Name());
		ImGui::Text(" Bind [ Base ] Texture => [ %s ] ", m_strBuffer.c_str());
		ImGui::SameLine();
		if (ImGui::ImageButton(" Bind Texture##Base", m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture == nullptr ? (ImTextureID)m_pDefaultSRV :
			(ImTextureID)m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture->Get_SRV(), ImVec2(32, 32)))
		{
			m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.pBase_Texture;
			m_isTexArraySelect = false;
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
			m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.pMix_DH_Tile_Texture;
			m_isTexArraySelect = true;
			ImGui::OpenPopup("Texture_Select_Modal");
		}

		ImGui::NewLine();

		/* NBR Binding  */
		m_strBuffer = m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture == nullptr ? "" : Engine_Utils::ToString(m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture->Get_Name());
		ImGui::Text(" Mix [ NRB ] Texture => [ %s ] ", m_strBuffer.c_str());
		ImGui::SameLine();
		if (ImGui::ImageButton(" Bind Texture##Mix_NBR ", m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture == nullptr ? (ImTextureID)m_pDefaultSRV :
			(ImTextureID)m_pMapToolManager->m_tTextureSplattingInfo.vecDHTextureArraySlices[0], ImVec2(32, 32)))
		{
			m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.pMix_NBR_Tile_Texture;
			m_isTexArraySelect = true;
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
		bool isbOpenTileSelectPopup = false;
		bool isRGBATextureBinding{ false };
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
					m_ppTargetSlot = &m_pMapToolManager->m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i];
					m_isTexArraySelect = false;
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
					Render_Single_Channel_Setting("RED", ImVec4(1.f, 0.3f, 0.3f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_R],CurrentData.fRGBA_Mix_Forces[CHANNEL_R],CurrentData.iUseFlags[CHANNEL_R],&isbOpenTileSelectPopup); // 멤버변수 등
					ImGui::PopID();


					ImGui::SameLine();

					/* Green Channel */
					ImGui::PushID("Green");
					Render_Single_Channel_Setting("Green", ImVec4(0.3f, 1.0f, 0.3f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_G], CurrentData.fRGBA_Mix_Forces[CHANNEL_G], CurrentData.iUseFlags[CHANNEL_G], &isbOpenTileSelectPopup); // 멤버변수 등
					ImGui::PopID();

					ImGui::SameLine();

					/* Blue Channel */
					ImGui::PushID("Blue");
					Render_Single_Channel_Setting("Blue", ImVec4(0.3f, 1.0f, 0.3f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_B], CurrentData.fRGBA_Mix_Forces[CHANNEL_B], CurrentData.iUseFlags[CHANNEL_B], &isbOpenTileSelectPopup); // 멤버변수 등
					ImGui::PopID();

					ImGui::SameLine();

					/* Alpha Channel */
					ImGui::PushID("Alpha");
					Render_Single_Channel_Setting("Alpha", ImVec4(0.3f, 1.0f, 0.3f, 1.f), CurrentData.iRGBA_Connected_Tile_Index[CHANNEL_A], CurrentData.fRGBA_Mix_Forces[CHANNEL_A], CurrentData.iUseFlags[CHANNEL_A], &isbOpenTileSelectPopup); // 멤버변수 등
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


HRESULT CPanel_MapTool::Render_Single_Channel_Setting(const char* szLabel, const ImVec4& vColor, OUT int& iConnectedIndex, OUT float& fForce, int& iFlag, OUT bool* pIsOpenPopup)
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

		if (ImGui::DragFloat("##Force", &fForce, 0.1f, 0.1f, 100.0f, "T:%.1f"))
			isFixInfo = true;
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Tiling Force");
		}

		ImGui::PopItemWidth();

		// 사용 여부 체크박스 (int를 bool처럼)
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
	_bool isChangeTexture	{false};
	_bool isChangeTextureArray{false};
	_bool isCompatible		{false};
	_bool isTextureArray	{false};

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
			if (m_ppTargetSlot != nullptr)
			{
				Safe_Release(*m_ppTargetSlot);
				*m_ppTargetSlot = nullptr;
				m_ppTargetSlot = nullptr;
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
				bool isSelected = (m_selectedCategoryName == currentCategoryW);

				if (ImGui::Selectable(currentCategoryStr.c_str(), isSelected))
				{
					m_selectedCategoryName = currentCategoryW;
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine(); 

		ImGui::BeginChild("RightPane", ImVec2(rightPaneWidth, paneHeight), true);
		{
			auto iter = m_pMapToolManager->m_umapMapTextures.find(m_selectedCategoryName);

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

					isCompatible = (m_isTexArraySelect == isTextureArray);

					ImGui::BeginGroup();
					{
						if (!isCompatible) ImGui::BeginDisabled(true);

						if (ImGui::ImageButton("##TexBtn", (ImTextureID)pSRV, ImVec2(button_sz, button_sz)))
						{
							if (isCompatible && m_ppTargetSlot != nullptr)
							{
								Safe_Release(*m_ppTargetSlot);
								*m_ppTargetSlot = pTex;        
								Safe_AddRef(*m_ppTargetSlot); 

								m_ppTargetSlot = nullptr;      
								isChangeTexture = true;  
								isChangeTextureArray = isTextureArray;
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
			m_ppTargetSlot = nullptr;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (isChangeTexture)
	{
		m_pMapToolManager->Bind_MapTexture();
		m_pMapToolManager->Bind_Mix_RGBA_Texture();
		if (isChangeTextureArray)
			m_pMapToolManager->Slice_DH_Texture();
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