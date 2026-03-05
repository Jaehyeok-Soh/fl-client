#include "pch.h"
#include "Panel_MapObjectList.h"
#include "Engine_Utils.h"
#include "ImGui_Layout_Transform.h"
#include "Level_Map.h"
#include "CameraMan.h"
#include "Camera.h"
#include "Panel_FileExplore.h"
#include "Panel_MapTool.h"
#include "Model.h"
#include <fstream>
#include "MapToolManager.h"
#include "InstanceMesh.h"
#include "AsTypes.h"
#include "Mesh.h"
#include "MapObject.h"
#include "DataStruct_Map.h"
#include "GameInstance.h"

USING(Tool)

CPanel_MapObjectList::CPanel_MapObjectList(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext), m_pGameInstance(CGameInstance::GetInstance()), m_pTransformLayout(nullptr), m_pCamera(nullptr)
	, m_pCameraCom(nullptr), m_wszMapObjectLayerTag{}, m_szFindName{}, m_iSelectInstanceID{} 
	, m_iSelectOriginMtlTexture{ 0 }, m_pMapToolManager{ CMapToolManager::GetInstance() }, m_szMakeMonsterTypeName{}
{

	m_pTransformLayout = CImGui_Layout_Transform::Create("Layout_Transform", m_pDevice, m_pDeviceContext);

	Safe_AddRef(m_pGameInstance);

	m_pCamera = static_cast<Engine::CCameraMan*>(m_pGameInstance->Get_MainCamera());

	m_pCameraCom = m_pCamera->Get_Component<CCamera>();

	m_arrayMtl_SRVs.fill(nullptr);



	for (_uint i = 0; i < ENUM_TO_UINT(DTO::EMakeMonsterType::END); ++i)
	{
		string strCurName = DTO::MakeMonsterType_ToString(DTO::EMakeMonsterType(i));
		::strcpy_s(m_szMakeMonsterTypeName[i] , MAX_PATH , strCurName.c_str());
	}

	for (_uint i = 0; i < ENUM_TO_UINT(DTO::EMakeObjectType::END); ++i)
	{
		string strCurName = DTO::MakeObjectType_ToString(DTO::EMakeObjectType(i));
		::strcpy_s(m_szMakeObjectTypeName[i], MAX_PATH, strCurName.c_str());
	}


	for (_uint i = 0; i < ENUM_TO_UINT(BATTLE_FIELD_DESC::Field_Type::END); ++i)
	{
		string strCurName = BATTLE_FIELD_DESC::FieldType_ToString(BATTLE_FIELD_DESC::Field_Type(i));
		::strcpy_s(m_szBattleFieldTypeName[i], MAX_PATH, strCurName.c_str());
	}

}


HRESULT CPanel_MapObjectList::Initialize()
{
	if (FAILED(Ready_LayerTag()))
		return E_FAIL;

	if (FAILED(Update_MapObjectList()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPanel_MapObjectList::Ready_LayerTag()
{
	return S_OK;
}

HRESULT CPanel_MapObjectList::Render(CToolObject* pGo)
{
	if (FAILED(Render_MapObjectList()))
		return E_FAIL;

	ImGui::Separator();

	if (FAILED(Render_SelectInfo()))
		return E_FAIL;

	if (FAILED(Render_SelectOverrideMaterialInfo()))
		return E_FAIL;

	if (FAILED(Render_SelectOriginMaterialInfo()))
		return E_FAIL;

	return S_OK;
}

void CPanel_MapObjectList::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Update_SelectObject();
}


HRESULT CPanel_MapObjectList::Update_MapObjectList()
{
	m_pLayer = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP) , g_wszMapObjectLayer);


	return S_OK;
}

void CPanel_MapObjectList::Update_SelectObject()
{
	m_pSelectMapObject = static_cast<CMapObject*>(static_cast<CLevel_Map*>(m_pOwnerLevel)->Get_SelectToolObject());
}


void CPanel_MapObjectList::Reset_SelectValue()
{
	m_iSelectOriginMtlTexture = 0;
	m_iSelectInstanceID = 0;
	m_pSelectMapObject = nullptr;

	m_iSelectMaterialIndex = 0;
	m_pSelectMaterial = nullptr;


	m_iSelectOverrideMtlTextureID = 0;
	m_iSelectOverrideMtlID = -1;





	m_strOriginMtlPath.clear();
	m_strOriginMtlName.clear();
}


HRESULT CPanel_MapObjectList::Render_MapObjectList()
{
	ImGui::Begin(m_strLabel.c_str());


	Update_SelectObject();

	if (FAILED(Update_MapObjectList()))
	{
		ImGui::End();
		return E_FAIL;
	}


	ImGui::SeparatorText(" Function ");

	//if (ImGui::Button("All Clear"))
	//{
	//	m_pSelectMapObject = nullptr;
	//	CMapToolManager::GetInstance()->Delete_Preview();
	//	static_cast<CLevel_Map*>(m_pOwnerLevel)->Set_SelectToolObjectNull();
	//	for (_uint i = 0; i < static_cast<_uint>(EMapObject_Type::END); ++i)
	//		m_pGameInstance->Clear_Layer(ENUM_TO_UINT(ELevelType::MAP) , g_wszMapObjectLayer );
	//}

	//ImGui::Separator();

	ImGui::NewLine();

	m_strBuffer = m_eShowMapObjectFilter == EClientMakePath::END ? "All" : ClientMakePath_ToString(m_eShowMapObjectFilter);

	if (ImGui::BeginCombo(" Clinet Make Path Select " , m_strBuffer.c_str()))
	{
		for (_int i = 0; i <= ENUM_TO_UINT(EClientMakePath::END); ++i)
		{
			EClientMakePath eClientMakePath = static_cast<EClientMakePath>(i);
			m_strBuffer = eClientMakePath == EClientMakePath::END ? "All" : ClientMakePath_ToString(eClientMakePath);
			bool isSelected = eClientMakePath == m_eShowMapObjectFilter;
			if (ImGui::Selectable(m_strBuffer.c_str(), &isSelected))
				m_eShowMapObjectFilter = eClientMakePath;
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Select Client Make Path Clear"))
	{
		m_pSelectMapObject = nullptr;
		if (m_pLayer)
		{
			for (auto& GameObject : *m_pLayer)
			{
				if (!GameObject)
					continue;
				bool isDelete{ false };
				
				static_cast<CLevel_Map*>(m_pOwnerLevel)->On_ChangeSelectedObject(nullptr);

				if (m_eShowMapObjectFilter == EClientMakePath::END || static_cast<CMapObject*>(GameObject)->Get_ClientMakePath() == m_eShowMapObjectFilter)
				{
					m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), GameObject);
				}
			}
		}
	}


	ImGui::Separator();                                                                                                                                                              


	ImGui::NewLine();
	
	ImGui::InputText("Search", m_szFindName, MAX_PATH);
	
	ImGui::NewLine();

	ImGui::Separator();

	ImGui::Text( " Object Count [ %d ]" , m_iObjectCount );

	ImGui::Separator();


	m_iObjectCount = 0;

	if (!m_pLayer)
	{
		ImGui::Text(" Empty Layer ");
	}
	else
	{
		if (ImGui::CollapsingHeader(" Map Object List "))
		{
			UINT32 iIndex = 0;
			for (auto& GameObject : *m_pLayer)
			{
				if (!GameObject)
				{
					iIndex++;
					continue;
				}

				CMapObject* pMapObject = static_cast<CMapObject*>(GameObject);
				if (pMapObject->Get_MapObjectDrawType() == EMapObject_DrawType::Instance) m_iObjectCount += pMapObject->Get_InstanceCount();
				else m_iObjectCount++;

				if (m_eShowMapObjectFilter != EClientMakePath::END && m_eShowMapObjectFilter != pMapObject->Get_ClientMakePath())
				{
					iIndex++;
					continue;
				}

				string strName = pMapObject->Get_Name();

				if (strlen(m_szFindName) > 0)
					if (strName.find(m_szFindName) == string::npos)
					{
						iIndex++;
						continue;
					}

				ImGui::PushID(iIndex);

				ImGui::Selectable(strName.c_str(), pMapObject == m_pSelectMapObject);

				_int iCount = pMapObject->Get_InstanceCount();
				m_strBuffer = iCount == 1 ? "" : std::to_string(iCount);

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					Reset_SelectValue();
					static_cast<CLevel_Map*>(m_pOwnerLevel)->On_ChangeSelectedObject(pMapObject);
				}

				else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					m_pCamera->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, pMapObject->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS));

				ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(m_strBuffer.c_str()).x);
				ImGui::Text(m_strBuffer.c_str());

				iIndex++;
				ImGui::PopID();
			}
		}
	}


	ImGui::End();

	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_SelectMaterial()
{

	Update_SelectObject();

	if (m_pSelectMaterial == nullptr)
	{
		ImGui::Text(" Select Material Emtpy ");
		return S_OK;
	}


	m_strBuffer = Engine_Utils::ToString(m_pSelectMaterial->Get_Name());

	/* Material 이름 */

	ImGui::NewLine();
	
	ImGui::SeparatorText(" Material Info ");

	ImGui::Separator();

	ImGui::SeparatorText( "  Textures " );


	return S_OK;
}


HRESULT CPanel_MapObjectList::Render_TransformInfo()
{

	Update_SelectObject();

	if (m_pSelectMapObject == nullptr) return S_OK;

	ImGui::SeparatorText("Reset / Resister");

	if (ImGui::Button(" Reset "))
	{
		m_pSelectMapObject->Reset_OriginTransform();
	}
	ImGui::SameLine();
	if (ImGui::Button(" Register "))
	{
		m_pSelectMapObject->Override_OriginTransform();
	}


	if (ImGui::TreeNode(" Origin SRT "))
	{
		SRT_DATA tData = m_pSelectMapObject->Get_SRTData( true, m_iSelectInstanceID);

		ImGui::Text(" Scale	=> X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]", tData.vScale.x, tData.vScale.y, tData.vScale.z);
		ImGui::Text(" Degree	=> X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]  W : [%.2f] ", tData.vQuat.x, tData.vQuat.y, tData.vQuat.z, tData.vQuat.w);
		ImGui::Text(" Position => X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]", tData.vPosition.x, tData.vPosition.y, tData.vPosition.z);

		ImGui::TreePop();
	}

	ImGui::Separator();

	ImGui::NewLine();

	ImGui::SeparatorText(" Current SRT ");

	m_pTransformLayout->Render(m_pSelectMapObject);

	ImGui::Separator();

	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_SelectInfo()
{
	ImGui::Begin(" Select Info ");


	Update_SelectObject();


	if (m_pSelectMapObject == nullptr)
	{
		ImGui::Text(" Select Map Object is Empty ");
		ImGui::End();
		return S_OK;
	}
	else
	{
		EMapObject_Type eMapObjectType = static_cast<CMapObject*>(m_pSelectMapObject)->Get_MapObjectType();

		ImGui::NewLine();


		ImGui::SeparatorText(" Delete & Cancel & Brush ");

#pragma region Delete 

		if (ImGui::Button(" Delete [ Warning : [Instance] Draw Object All Delete ] "))
		{
			m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), m_pSelectMapObject);
			static_cast<CLevel_Map*>(m_pOwnerLevel)->On_ChangeSelectedObject(nullptr);
			m_pSelectMapObject = nullptr;
			ImGui::End();
			return S_OK;
		}

#pragma endregion

#pragma region Cancel Select

		if (ImGui::Button(" Cancel Select "))
		{
			//if (m_pMapToolManager->Get_PrevieObject())
			//{
			//	MSG_BOX("Preview Object");
			//}


			static_cast<CLevel_Map*>(m_pOwnerLevel)->On_ChangeSelectedObject(nullptr);
			m_pSelectMapObject = nullptr;
			ImGui::End();
			return S_OK;
		}

#pragma endregion

		ImGui::SameLine();

#pragma region Cancel Select

		if (ImGui::Button("  Brush Sclae Rotation Set  "))
		{
			//if (CMapToolManager::GetInstance()->Get_PrevieObject() != nullptr)
			//{
			//	MSG_BOX(" Preview Object가 존재하고 있습니다 안전한 사용을 위해 프리뷰 오브젝트가 없을떄 등록이 가능합니다 ");
			//	ImGui::End();
			//	return S_OK;
			//}

			SRT_DATA tSRT = m_pSelectMapObject->Get_SRTData(false);
			
			/* Brush  */
			CMapToolManager::GetInstance()->Set_BrushScale(tSRT.vScale);
			CMapToolManager::GetInstance()->Set_BrushRotation(tSRT.vQuat);
			ImGui::End();
			return S_OK;
		}

#pragma endregion

		ImGui::Separator();
		ImGui::NewLine();

		ImGui::SeparatorText(" Basic Map Object Info ");

		if (ImGui::TreeNode("UE Data Info"))
		{
			bool isUELoaded = m_pSelectMapObject->Get_IsUELoaded();
			ImGui::Text("Is UE Data Load Object => [ %s ] ", isUELoaded == true ? string("true").c_str() : string("false").c_str());
			ImGui::Text(" Raw Data Path => [ %s ]", Engine_Utils::ToString(m_pSelectMapObject->Get_UERawDataPath()).c_str());
			ImGui::TreePop();
		}

		bool isLoaded = m_pSelectMapObject->Get_IsLoaded();
		ImGui::Text("Is Loaded Object => [ %s ]", isLoaded == true ? string("true").c_str() : string("false").c_str());
		ImGui::Text(" Map Object Name : [ %s ]", m_pSelectMapObject->Get_Name().c_str());


		ImGui::NewLine();

		m_iBuffer = static_cast<_int>(m_pSelectMapObject->Get_SectionNumber());
		ImGui::Text(" Section Number : [ %d ] " , m_iBuffer);
		if (ImGui::InputInt("##SectionNumber", &m_iBuffer))
			m_pSelectMapObject->Set_SectionNumber(static_cast<_uint>(m_iBuffer));

		ImGui::NewLine();
		ImGui::Separator();

		ImGui::SeparatorText(" Type Setting ");

		ImGui::NewLine();

#pragma region Draw Type

		ImGui::SeparatorText("Draw Type");
		m_iBuffer = ENUM_TO_UINT(m_pSelectMapObject->Get_MapObjectDrawType());
		if (ImGui::BeginCombo("##Draw_Type", EMapObject_DrawType_ToString(static_cast<EMapObject_DrawType>(m_iBuffer)).c_str()))
		{
			for (_int i = 0; i < ENUM_TO_UINT(EMapObject_DrawType::END); ++i)
			{
				bool isSelected = m_iBuffer == i;
				if (ImGui::Selectable(EMapObject_DrawType_ToString(static_cast<EMapObject_DrawType>(i)).c_str(), &isSelected))
				{
					m_pSelectMapObject->Set_MapObjectDrawType(static_cast<EMapObject_DrawType>(i));
					if (!m_pSelectMapObject)
					{
						static_cast<CLevel_Map*>(m_pOwnerLevel)->On_ChangeSelectedObject(nullptr);
						ImGui::EndCombo();
						ImGui::End();
						return S_OK;
					}
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
#pragma endregion

#pragma region Client Make Path

		ImGui::SeparatorText("Client Make Path");
		m_iBuffer = ENUM_TO_UINT(m_pSelectMapObject->Get_ClientMakePath());
		if (ImGui::BeginCombo("##Client Make Path ", ClientMakePath_ToString(static_cast<EClientMakePath>(m_iBuffer)).c_str()))
		{
			for (_int i = 0; i < ENUM_TO_UINT(EClientMakePath::END); ++i)
			{
				bool isSelected = m_iBuffer == i;
				if (ImGui::Selectable(ClientMakePath_ToString(static_cast<EClientMakePath>(i)).c_str(), &isSelected))
					m_pSelectMapObject->Set_ClientMakePath(static_cast<EClientMakePath>(i));
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
#pragma endregion

#pragma region Client Level Type

		ImGui::SeparatorText("Client Level Type ");
		m_iBuffer = ENUM_TO_UINT(m_pSelectMapObject->Get_ClientLevelType());
		if (ImGui::BeginCombo("##Client Level Type", ClientleveltypeToString(static_cast<EClientLevelType>(m_iBuffer)).c_str()))
		{
			for (_int i = 0; i < ENUM_TO_UINT(EClientLevelType::END); ++i)
			{
				bool isSelected = m_iBuffer == i;
				if (ImGui::Selectable(ClientleveltypeToString(static_cast<EClientLevelType>(i)).c_str(), &isSelected))
					m_pSelectMapObject->Set_ClientLeveType(static_cast<EClientLevelType>(i));
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

#pragma endregion


		ImGui::Separator();


		ImGui::SeparatorText(" Select Instance ID ");
		_int iCountInstance = m_pSelectMapObject->Get_InstanceCount();
		m_iSelectInstanceID = m_pSelectMapObject->Get_SelectedInstanceID();
		if (m_iSelectInstanceID > iCountInstance - 1 || m_iSelectInstanceID < 0)
		{
			m_iSelectInstanceID = 0;
			m_pSelectMapObject->Set_SelectedInstanceID(m_iSelectInstanceID);
		}

		if (ImGui::SliderInt("Instance", &m_iSelectInstanceID, 0, iCountInstance - 1, "Instance %d"))
			m_pSelectMapObject->Set_SelectedInstanceID(m_iSelectInstanceID);

		ImGui::Separator();

		if (m_pSelectMapObject->Get_MapObjectDrawType() == EMapObject_DrawType::Instance)
		{
			ImGui::SeparatorText(" Instance Add & Delete ");

			if (ImGui::Button(" Add Instance "))
			{
				if (CMapToolManager::GetInstance()->Get_PrevieObject() != nullptr)
				{
					MSG_BOX(" 프리뷰 오브젝트가 존재하고 있습니다 안전을 위해 Preview 오브젝트를 먼저 제거 해주세요 ");
					ImGui::End();
					return S_OK;
				}

				CMapToolManager* pMapToolMgr = CMapToolManager::GetInstance();
				SRT_DATA tSRT;
				if (pMapToolMgr->Get_MapToolObjectBatchMode() == EMapToolObjectBatchMode::Brush)
					pMapToolMgr->Get_SRT_BrushData(tSRT.vScale, tSRT.vQuat, tSRT.vPosition);
				else
					tSRT = m_pSelectMapObject->Get_SRTData(false); // 현재 선택된 놈의SRT를 복제

				m_pSelectMapObject->Add_InstanceData(tSRT);
				pMapToolMgr->Set_PreviewMapObject(m_pSelectMapObject);
			}

			if (ImGui::Button(" Delete Instance "))
			{
				if (CMapToolManager::GetInstance()->Get_PrevieObject() != nullptr)
				{
					MSG_BOX(" 프리뷰 오브젝트가 존재하고 있습니다 안전을 위해 Preview 오브젝트를 먼저 제거 해주세요 ");
					ImGui::End();
					return S_OK;
				}

				m_pSelectMapObject->Delete_InstanceData();
				ImGui::End();
				return S_OK;
			}

		}
		
		else
		{
			ImGui::SeparatorText(" Add Object");

			if (ImGui::Button(" Add "))
			{
				if (CMapToolManager::GetInstance()->Get_PrevieObject() != nullptr)
				{
					MSG_BOX(" 프리뷰 오브젝트가 존재하고 있습니다 안전을 위해 Preview 오브젝트를 먼저 제거 해주세요 ");
					ImGui::End();
					return S_OK;
				}
				/* Brush  */
				CMapToolManager* pMapToolMgr = CMapToolManager::GetInstance();
				SRT_DATA tSRT;

				if (pMapToolMgr->Get_MapToolObjectBatchMode() == EMapToolObjectBatchMode::Brush)
					pMapToolMgr->Get_SRT_BrushData(tSRT.vScale, tSRT.vQuat, tSRT.vPosition);
				else
					tSRT = m_pSelectMapObject->Get_SRTData(false);

				/* 똑같은 Object 생성해야함... */
				pMapToolMgr->Set_PreviewMapObject(CMapObject::Clone(m_pSelectMapObject, tSRT));
			}
		}

		if (ImGui::BeginTabBar("Detail Info"))
		{
			if (ImGui::BeginTabItem(" Model Info "))
			{
				
				Render_ModelInfo();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(" Transform "))
			{

				Render_TransformInfo();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(" Descriptioin "))
			{

				Render_Description();

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}


	ImGui::End();
	return S_OK;
}


HRESULT CPanel_MapObjectList::Render_ModelInfo()
{
	Update_SelectObject();


	if (m_pSelectMapObject == nullptr) return E_FAIL;



	if (m_pSelectMapObject->Get_MapObjectDrawType() == EMapObject_DrawType::Collider)
	{
		ImGui::Text(" Model Info is Empty.. ");
		return S_OK;
	}

	CModel* pModel = m_pSelectMapObject->Get_Component<CModel>();

	if (!pModel)
		return S_OK;

	_uint iMtlCount = pModel->Get_MaterialCount();

	ImGui::SeparatorText(" Model Info ");

	ImGui::NewLine();

	ImGui::Text("Model Name : [ %s ]",  m_pSelectMapObject->Get_ModelFileName().c_str() );

	ImGui::NewLine();

	ImGui::SeparatorText(" Mateiral Info ");

	ImGui::PushID("Matrial");

	ImGui::BeginChild(" Mateiral Info ", m_vTextureInfoTableSize, true);

	if (ImGui::BeginTable("Mateiral Info Table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Slot Num", ImGuiTableColumnFlags_WidthFixed,50.0f);
		ImGui::TableSetupColumn("Material Name", ImGuiTableColumnFlags_WidthStretch);

		vector<CMaterial*> vecMtl = pModel->Get_Materials();

		for (_uint i = 0; i < ENUM_TO_UINT(vecMtl.size()); ++i)
		{

			ImGui::PushID(i);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			bool bSelected = (m_iSelectMaterialIndex == i);
			m_strBuffer = vecMtl[i] == nullptr ? "" : Engine_Utils::ToString(vecMtl[i]->Get_Name());
			if (ImGui::Selectable(m_strBuffer.c_str(), bSelected))
			{
				if(!m_strBuffer.empty())
					m_pSelectMaterial = vecMtl[i];
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(Engine_Utils::ToString(vecMtl[i]->Get_Name()).c_str());
			ImGui::PopID();

		}
		ImGui::EndTable();
	}




	ImGui::EndChild();


	ImGui::PopID();

	return S_OK;
}


HRESULT CPanel_MapObjectList::Render_SelectOverrideMaterialInfo()
{
	//ImGui::Begin(" Override Material Info ");

	Update_SelectObject();

	//if ( m_iSelectOverrideMtlID == -1 || m_pSelectMapObject == nullptr )
	//{
	//	ImGui::Text(" Override Material Is Empty");
	//	ImGui::End();
	//	return S_OK;
	//}


	//OVERRIDE_MATERIALS tOverrideMtl = m_pSelectMapObject->Get_UsingModelInfo().vecOverrideMaterial[m_iSelectOverrideMtlID];


	//ImGui::SeparatorText(" Override Material Info ");
	//ImGui::NewLine();
	//ImGui::Text(" Material Name => [ %s ] ", Engine_Utils::ToString(tOverrideMtl.wstrMtl_JsonFile_Name).c_str());
	//
	//ImGui::NewLine();

	//ImGui::Text(" Material Path => [ %s ] ", Engine_Utils::ToString(tOverrideMtl.wstrMtl_JsonFile_Path).c_str());
	//

	//if (tOverrideMtl.vecUsingTextureInfo.empty())
	//{
	//	ImGui::NewLine();
	//	ImGui::TextWrapped(" Using Texture Is Empty Check Json File ");
	//	ImGui::End();
	//	return S_OK;
	//}

	//
	//ImGui::BeginChild("Using Texture Info", m_vTextureInfoTableSize , true);

	//if (ImGui::BeginTable("TextureInfoTable##Overrdie", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV))
	//{
	//	ImGui::TableSetupColumn("Slot Type##Overrdie", ImGuiTableColumnFlags_WidthFixed, 45.0f);
	//	ImGui::TableSetupColumn("Texture Name##Overrdie", ImGuiTableColumnFlags_WidthStretch);

	//	for (_uint i = 0; i < ENUM_TO_UINT(tOverrideMtl.vecUsingTextureInfo.size()); ++i)
	//	{
	//		ImGui::PushID(i);
	//		string strMtlSLotName  = Engine_Utils::ToString(tOverrideMtl.vecUsingTextureInfo[i].first);
	//		string strUsingTexName = path(tOverrideMtl.vecUsingTextureInfo[i].second).filename().stem().string();

	//		ImGui::TableNextRow();
	//		ImGui::TableSetColumnIndex(0);

	//		bool bSelected = (m_iSelectOverrideMtlTextureID == i);
	//		if (ImGui::Selectable(strMtlSLotName.c_str(), bSelected, ImGuiSelectableFlags_SpanAllColumns))
	//		{
	//			if (!strUsingTexName.empty())
	//				m_iSelectOverrideMtlTextureID = i;
	//		}

	//		ImGui::TableSetColumnIndex(1);
	//		ImGui::TextUnformatted(strUsingTexName.c_str());

	//		ImGui::PopID();
	//	}
	//	ImGui::EndTable();
	//}
	//ImGui::EndChild();


	//if(m_iSelectOverrideMtlTextureID >= tOverrideMtl.vecUsingTextureInfo.size())
	//	m_iSelectOverrideMtlTextureID = 0;

	//CTextureBase::RESOURCE_BASE_DESC tDesc{};
	//tDesc.wstrPath = tOverrideMtl.vecUsingTextureInfo[m_iSelectOverrideMtlTextureID].second;
	//tDesc.wstrName = path(tOverrideMtl.vecUsingTextureInfo[m_iSelectOverrideMtlTextureID].second).filename().stem();
	//CTextureBase* pTex = m_pGameInstance->GetOrAddTexture(tDesc.wstrName, &tDesc);
	//if (pTex)
	//{
	//	ID3D11ShaderResourceView* pSRV = pTex->Get_SRV();
	//	ImGui::Image((ImTextureID)pSRV, ImVec2(200, 200));
	//	Safe_Release(pTex);
	//}

	//ImGui::End();
	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_SelectOriginMaterialInfo()
{
	ImGui::Begin(" Origin Material Info ");

	Update_SelectObject();

	if ( !m_pSelectMapObject || m_pSelectMaterial == nullptr)
	{
		ImGui::Text(" Origin Material Is Empty");
		ImGui::End();
		return S_OK;
	}

	ImGui::SeparatorText(" Select Material Info ");

	ImGui::NewLine();

	ImGui::Text(" Material Name => [ %s ] ", Engine_Utils::ToString(m_pSelectMaterial->Get_Name()).c_str());

	ImGui::Separator();

	
	m_arrayMtl_SRVs = m_pSelectMaterial->Get_ArraySRV();
	m_arrayMtl_Textures = m_pSelectMaterial->Get_TextureNameArray();


	m_strBuffer = Engine_Utils::MaterialTextureType_ToString(static_cast<EMaterialTextureType>(m_iSelectMtSlot));

	ImGui::SeparatorText(" Slot Name ");
	if (ImGui::BeginCombo("##MaterialSLotName", m_strBuffer.c_str()))
	{
		for (_uint i = 0; i < ENUM_TO_UINT(EMaterialTextureType::MAX_COUNT); ++i)
		{
			if (m_arrayMtl_SRVs[i] == nullptr) continue;
			_bool isSelected = m_iSelectMtSlot == i;
			m_strBuffer = Engine_Utils::MaterialTextureType_ToString(static_cast<EMaterialTextureType>(i));
			if (ImGui::Selectable(m_strBuffer.c_str(), isSelected))
				m_iSelectMtSlot = i;
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();


	CTextureBase::RESOURCE_BASE_DESC tDesc{};
	if (m_iSelectMtSlot > m_arrayMtl_SRVs.size())
		m_iSelectMtSlot = static_cast<_uint>(m_arrayMtl_SRVs.size()) - 1;

	if (m_arrayMtl_SRVs[m_iSelectMtSlot])
	{
		ImGui::Image((ImTextureID)m_arrayMtl_SRVs[m_iSelectMtSlot], ImVec2(200, 200));
	}

	m_strBuffer = Engine_Utils::ToString(m_arrayMtl_Textures[m_iSelectMtSlot]);
	ImGui::Text(" Texture Name => [ %s ] " , m_strBuffer.c_str());


	ImGui::End();

	return S_OK;
}



CPanel_MapObjectList* CPanel_MapObjectList::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_MapObjectList* pPanel = new CPanel_MapObjectList(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pPanel->Initialize()))
	{
		Safe_Release(pPanel);
		MSG_BOX(" MapObjec List Panel Is Failed To Create");
		return nullptr;
	}

	return  pPanel;
}

void CPanel_MapObjectList::Free()
{
	Super::Free();

	m_pMapToolManager = nullptr;
	Safe_Release(m_pTransformLayout);
	Safe_Release(m_pGameInstance);
}



void CPanel_MapObjectList::Compute_LandScape_TextureUV(_uint iLandScapeIndex, OUT Vec2& vOut_LT, OUT Vec2& vOut_RB)
{
	// 0으로 나누기 방지 (안전장치)
	if (m_iLandScape_Col < 1) m_iLandScape_Col = 1;
	if (m_iLandScape_Row < 1) m_iLandScape_Row = 1;

	// 1. 한 칸의 UV 사이즈 계산 (가로/세로 길이)
	float fUnitU = 1.0f / (float)m_iLandScape_Col;
	float fUnitV = 1.0f / (float)m_iLandScape_Row;

	// 2. 현재 인덱스가 몇 번째 행(Row), 몇 번째 열(Col)인지 계산
	// iLandScapeIndex = iCurrentRow * m_iLandScape_Col + iCurrentCol
	_uint iIndex_Row = iLandScapeIndex / m_iLandScape_Col;
	_uint iIndex_Col = iLandScapeIndex % m_iLandScape_Col;

	// ---------------------------------------------------------
	// 3. 좌표 계산 (DirectX 텍스처 좌표계 기준: 좌상단 0,0)
	// ---------------------------------------------------------

	// U (가로): 왼쪽에서 오른쪽으로 증가
	float fLeft = (float)iIndex_Col * fUnitU;
	float fRight = fLeft + fUnitU;

	// V (세로): 위에서 아래로 증가 (일반적인 텍스처 좌표)
	// 만약 "0번 인덱스가 맨 위쪽"이라면 이 방식을 써야 합니다.
	//float fTop = (float)iIndex_Row * fUnitV;
	//float fBottom = fTop + fUnitV;

	 //[참고] 만약 님 말씀대로 "0번 인덱스가 맨 아래(Bottom)"부터 시작해야 한다면?
	 //  위의 fTop, fBottom 코드를 지우고 아래 주석을 푸세요.

	float fBottom = 1.0f - ((float)iIndex_Row * fUnitV);      // 아래쪽이 V값이 큼
	float fTop    = 1.0f - ((float)(iIndex_Row + 1) * fUnitV); // 윗쪽이 V값이 작음
	

	// 4. 결과 출력
	vOut_LT = Vec2(fLeft, fTop);
	vOut_RB = Vec2(fRight, fBottom);
}



#pragma region Desc

HRESULT CPanel_MapObjectList::Render_Description()
{
	Update_SelectObject();

	if (m_pSelectMapObject == nullptr)
	{
		ImGui::TextWrapped("  None Select Object  ");
		return S_OK;
	}


	CLIENT_MAKEPATH_DESC_BASE* pDesc = m_pSelectMapObject->Get_ClientMakePathDesc();

	if (pDesc == nullptr)
	{
		ImGui::TextWrapped(" This Object Don't Need Description ");
		return S_OK;
	}


	ImGui::SeparatorText(" Description Info ");

	EClientMakePath ePath = m_pSelectMapObject->Get_ClientMakePath();

	switch (ePath)
	{
	case Tool::EClientMakePath::StaticObject:				ImGuiUpdate_StaticObject_Desc					(static_cast<STATICOBJECT_DESC*>(pDesc));					return S_OK;
	case Tool::EClientMakePath::LandScape:					ImGuiUpdate_LandScape_Desc						(static_cast<LANDSCAPE_DESC*>(pDesc));						return S_OK;

	case Tool::EClientMakePath::Batch_Monster:				ImGuiUpdate_Batch_Monster_Desc					(static_cast<BATCH_MONSTER_DESC*>(pDesc));					return S_OK;
	case Tool::EClientMakePath::Batch_Object:				ImGuiUpdate_Batch_Object_Desc					(static_cast<BATCH_OBJECT_DESC*>(pDesc));					return S_OK;

	case Tool::EClientMakePath::TriggerBox_ChangeLevel:		ImGuiUpdate_TriggerBox_ChanageLevel_Desc	(static_cast<TRIGGERBOX_CHANGELEVEL_DESC*>(pDesc));				return S_OK;
	case Tool::EClientMakePath::TriggerBox_MonsterSpawner:	ImGuiUpdate_TriggerBox_MonsterSpawner		(static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*>(pDesc));			return S_OK;
	default:																																							return E_FAIL;
	}

	return S_OK;
}


#pragma region StaticObject

void CPanel_MapObjectList::ImGuiUpdate_StaticObject_Desc(STATICOBJECT_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	string strText = Engine_Utils::ToString(pDesc->wstrTest);


	ImGui::Text(" Test Text => [ %s ] " , strText.c_str());

	if (ImGui::InputText(" Test ", &strText))
		pDesc->wstrTest = Engine_Utils::ToWString(strText);



	return;
}

#pragma endregion

#pragma region LandScape
void CPanel_MapObjectList::ImGuiUpdate_LandScape_Desc(LANDSCAPE_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGui::NewLine();

	ImGui::SeparatorText(" Land Scpae Description ");

	/* 계산용 */

	ImGui::Text(" Setting To Compute UV ");

	ImGui::Text(" Total Count => [ %d ]", m_iLandScape_Row * m_iLandScape_Col );
	if (ImGui::InputInt(" Row Count ", &m_iLandScape_Row))
	{
		if (m_iLandScape_Row < 1)
			m_iLandScape_Row = 1;
	}
	if (ImGui::InputInt(" Col Count ", &m_iLandScape_Col))
	{
		if (m_iLandScape_Col < 1)
			m_iLandScape_Col = 1;
	}

	if (ImGui::Button("Compute UV"))
	{
		Compute_LandScape_TextureUV(pDesc->iIndex , pDesc->vTextureUV_LT , pDesc->vTextureUV_RB);
	}

	ImGui::NewLine();
	
	ImGui::SeparatorText(" Index ");

	ImGui::InputInt("Index" ,&pDesc->iIndex);

	ImGui::SeparatorText(" Texture UV ");

	ImGui::Text(" LT ");

	ImGui::InputFloat2( "LT" , &pDesc->vTextureUV_LT.x , "%.5f");

	ImGui::Text(" RB ");

	ImGui::InputFloat2("RB", &pDesc->vTextureUV_RB.x, "%.5f");

	ImGui::Separator();
}
#pragma endregion



#pragma region Batch Monster

void CPanel_MapObjectList::ImGuiUpdate_Batch_Monster_Desc(BATCH_MONSTER_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	m_strBuffer = DTO::MakeMonsterType_ToString(pDesc->eBatchMonsterType);
	m_iBuffer = _int(pDesc->eBatchMonsterType);

	if (ImGui::BeginCombo("Batch Monster Type List##Batch Monster Desc", m_strBuffer.c_str()))
	{
		for (_uint i = 0; i < ENUM_TO_UINT(DTO::EMakeMonsterType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(m_szMakeMonsterTypeName[i], &isSelected))
			{
				pDesc->eBatchMonsterType= DTO::EMakeMonsterType(i);
				m_pSelectMapObject->Ready_Batch_Monster();
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}


	return;

}

#pragma region Batch Object Desc

void CPanel_MapObjectList::ImGuiUpdate_Batch_Object_Desc(BATCH_OBJECT_DESC* pDesc)
{
	if (pDesc == nullptr) return;


	ImGui::SeparatorText( " Batch Object Type " );

	m_strBuffer = DTO::MakeObjectType_ToString(pDesc->eBatchObjectType);
	m_iBuffer = _int(pDesc->eBatchObjectType);

	if (ImGui::BeginCombo("Batch Object Type List##Batch Object Desc", m_strBuffer.c_str()))
	{
		for (_uint i = 0; i < ENUM_TO_UINT(DTO::EMakeObjectType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(m_szMakeObjectTypeName[i], &isSelected))
			{
				pDesc->eBatchObjectType= DTO::EMakeObjectType(i);
				pDesc->Change_BatchObjecType(pDesc->eBatchObjectType);
				m_pSelectMapObject->Ready_Batch_Object();
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();

	DTO::EMakeObjectType eType = pDesc->eBatchObjectType;

	switch (eType)
	{
	case DTO::EMakeObjectType::Battle_Field: ImGuiUpdate_Battle_Field_Desc(static_cast<BATTLE_FIELD_DESC*>(pDesc->pBatchObjectDesc));	break;
	default:																															return;
	}

	return;
}

#pragma endregion


void CPanel_MapObjectList::ImGuiUpdate_Battle_Field_Desc(BATTLE_FIELD_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGui::SeparatorText(" Battle Field Desc ");

	m_strBuffer = BATTLE_FIELD_DESC::FieldType_ToString(pDesc->eFieldType);
	m_iBuffer = _int(pDesc->eFieldType);

	if (ImGui::BeginCombo("Field Type##FieldType", m_strBuffer.c_str()))
	{
		for (_uint i = 0; i < ENUM_TO_UINT(BATTLE_FIELD_DESC::Field_Type::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(m_szBattleFieldTypeName[i], &isSelected))
				pDesc->eFieldType = BATTLE_FIELD_DESC::Field_Type(i);
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (pDesc->eFieldType == BATTLE_FIELD_DESC::Field_Type::Box)
	{
		ImGui::DragFloat3("Battle Field Extents##BattleFieldDesc",&pDesc->vExtents.x , 0.1f , 0.5f, 200.f );
	}
	else
	{
		ImGui::DragFloat("Battle Field Range##BattleFieldDesc", &pDesc->fRadius, 0.1f, 0.5, 200.f);
	}
	ImGui::Separator();


	return;
}

#pragma endregion

#pragma region Trigger Box

void CPanel_MapObjectList::ImGuiUpdate_TriggerBox(TRIGGERBOX_DESC* pDesc)
{
	ImGui::SeparatorText(" Trigger Box Desc ");

	if (ImGui::DragFloat3("Extents##TriggerBox_Extents", &pDesc->vExtents.x, 0.1f, 0.1f, 100.f, "%.2f"))
	{
		if (m_pSelectMapObject == nullptr) return;
		m_pSelectMapObject->Update_Collider();
	}

	ImGui::Separator();
}



#pragma region TriggerBox Change Level Desc

void CPanel_MapObjectList::ImGuiUpdate_TriggerBox_ChanageLevel_Desc(TRIGGERBOX_CHANGELEVEL_DESC* pDesc)
{

	/* 필수 요소 */
	ImGuiUpdate_TriggerBox(pDesc);

	ImGui::SeparatorText(" TriggerBox Change Level Desc");

	ImGui::SeparatorText("Client Level Type ");

	EClientLevelType eLevelType = StringToClientleveltype(pDesc->strChangeLevelTypeName);
	m_iBuffer = ENUM_TO_UINT(eLevelType);


	if (ImGui::BeginCombo("##Client Level Type", ClientleveltypeToString(static_cast<EClientLevelType>(m_iBuffer)).c_str()))
	{
		for (_int i = 0; i < ENUM_TO_UINT(EClientLevelType::END); ++i)
		{
			EClientLevelType eCurType = static_cast<EClientLevelType>(i);
			bool isSelected = m_iBuffer == i;
			string strCurName = ClientleveltypeToString(eCurType).c_str();
			if (ImGui::Selectable(strCurName.c_str(), &isSelected))
			{
				/* 이름 Update 해주기 */
				pDesc->strChangeLevelTypeName = strCurName;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();

}


#pragma region Monster Spawn Data ImGuiUpdate
void CPanel_MapObjectList::ImGuiUpdate_MonsterSpawnData(Engine::MonsterSpawnData* pMonsterSpawnData)
{
	if (pMonsterSpawnData == nullptr) return;


	ImGui::SeparatorText(" Monster Spawn Data ");


	ImGui::Text("--------Make Monster Type--------");

	m_strBuffer = DTO::MakeMonsterType_ToString(pMonsterSpawnData->eMakeMonsterType);
	m_iBuffer	= _int(pMonsterSpawnData->eMakeMonsterType);

	if (ImGui::BeginCombo("Make Monster Type List##Monstre Spawn Data Make Monster Type", m_strBuffer.c_str()))
	{
		for (_uint i = 0; i < ENUM_TO_UINT(DTO::EMakeMonsterType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(m_szMakeMonsterTypeName[i], &isSelected))
			{
				pMonsterSpawnData->eMakeMonsterType = DTO::EMakeMonsterType(i);
				/* Mosnter 다시 배당 */
				Safe_Release(pMonsterSpawnData->pDebugModel);
				pMonsterSpawnData->pDebugModel = m_pMapToolManager->Get_MonsterPreviewModel(pMonsterSpawnData->eMakeMonsterType);
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Text("-------Delay Time--------");

	ImGui::DragFloat("Delay Time##Monster Spawn Data Delay Time",&pMonsterSpawnData->fSpawnDelayTime,0.01f);

	ImGui::Text("-------SRT--------");

	ImGui::NewLine();

	ImGui::DragFloat3("Scale##Monster Spawn Data Scale",				&pMonsterSpawnData->vScale.x,		0.1f);
	ImGui::DragFloat3("PitchYawRoll##Monster Spawn Data PitchYawRoll",	&pMonsterSpawnData->vPitchYawRoll.x,	0.1f);
	ImGui::DragFloat3("Position##Monster Spawn Data Position",			&pMonsterSpawnData->vPosition.x,			0.1f);

	ImGui::NewLine();

	ImGui::Text("--------Use Debug Model--------");

	ImGui::Checkbox("Use Debug Model##Monster Spawn Data Use Debug Model", &pMonsterSpawnData->isPreviewDebugModel);

	ImGui::Separator();


	return;
}
#pragma endregion


void CPanel_MapObjectList::ImGuiUpdate_TriggerBox_MonsterSpawner(TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc)
{

	if (pDesc == nullptr) return;

	// 기본 TriggerBox 공통 데이터 (Extents 등)
	ImGuiUpdate_TriggerBox(pDesc);

	ImGui::SeparatorText(" Monster Spawn List ");

	// [ 데이터 추가 버튼 ]
	if (ImGui::Button(" + Add New Spawn Data ", ImVec2(-1, 0)))
	{
		pDesc->vecMonsterSpawnData.push_back(Engine::MonsterSpawnData());
		MonsterSpawnData& Data = pDesc->vecMonsterSpawnData.back();
		Data.pDebugModel = m_pMapToolManager->Get_MonsterPreviewModel(Data.eMakeMonsterType);
		
		if (m_pSelectMapObject)
		{
			Data.vPosition = m_pSelectMapObject->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		}
	}

	ImGui::Spacing();

	// 테이블 시작: [ID/Type | Control]
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("MonsterSpawnTable", 2, flags))
	{
		ImGui::TableSetupColumn("Spawn Info", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 80.0f);
		ImGui::TableHeadersRow();

		int iDeleteIndex = -1;

		for (int i = 0; i < (int)pDesc->vecMonsterSpawnData.size(); ++i)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			// 트리 노드로 개별 데이터 감싸기 (ID를 부여해야 충돌이 안 남)
			string strLabel = std::to_string(i) + ": " + DTO::MakeMonsterType_ToString(pDesc->vecMonsterSpawnData[i].eMakeMonsterType);
			bool bOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, ImGuiTreeNodeFlags_SpanFullWidth, strLabel.c_str());

			ImGui::TableSetColumnIndex(1);
			// 삭제 버튼
			string strDelBtnLabel = "Delete##" + std::to_string(i);
			if (ImGui::Button(strDelBtnLabel.c_str(), ImVec2(-1, 0)))
			{
				iDeleteIndex = i;
			}

			if (bOpen)
			{
				ImGuiUpdate_MonsterSpawnData(&pDesc->vecMonsterSpawnData[i]);
				ImGui::TreePop();
			}
		}

		// 삭제 처리
		if (iDeleteIndex != -1)
		{
			pDesc->vecMonsterSpawnData.erase(pDesc->vecMonsterSpawnData.begin() + iDeleteIndex);
		}

		ImGui::EndTable();
	}

}
#pragma endregion



#pragma endregion

#pragma endregion

#pragma endregion