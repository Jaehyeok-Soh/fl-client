#include "pch.h"
#include "Panel_MapObjectList.h"
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
#include "Light.h"
#include "DataStruct_Map.h"
#include "Effect_Env.h"
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
	case Tool::EClientMakePath::StaticObject:						ImGuiUpdate_StaticObject_Desc						(static_cast<STATICOBJECT_DESC*>(pDesc));								return S_OK;
	case Tool::EClientMakePath::LandScape:							ImGuiUpdate_LandScape_Desc							(static_cast<LANDSCAPE_DESC*>(pDesc));									return S_OK;


	case Tool::EClientMakePath::Tree:								ImGuiUpdate_Tree_Desc								(static_cast<TREE_DESC*>(pDesc));										return S_OK;
	case Tool::EClientMakePath::Moss:								ImGuiUpdate_Moss_Desc								(static_cast<MOSS_DESC*>(pDesc));										return S_OK;
	case Tool::EClientMakePath::Grass:								ImGuiUpdate_Grass_Desc								(static_cast<GRASS_DESC*>(pDesc));										return S_OK;
	case Tool::EClientMakePath::Vine:								ImGuiUpdate_Vine_Desc								(static_cast<VINE_DESC*>(pDesc));										return S_OK;
	case Tool::EClientMakePath::Bush:								ImGuiUpdate_Bush_Desc								(static_cast<BUSH_DESC*>(pDesc));										return S_OK;

	case Tool::EClientMakePath::Water:								ImGuiUpdate_Water_Desc								(static_cast<WATER_DESC*>(pDesc));										return S_OK;
	case Tool::EClientMakePath::Env:								ImGuiUpdate_Env_Desc								(static_cast<ENV_DESC*>(pDesc));										return S_OK;


	case Tool::EClientMakePath::Batch_Monster:						ImGuiUpdate_Batch_Monster_Desc						(static_cast<BATCH_MONSTER_DESC*>(pDesc));								return S_OK;
	case Tool::EClientMakePath::Batch_Object:						ImGuiUpdate_Batch_Object_Desc						(static_cast<BATCH_OBJECT_DESC*>(pDesc));								return S_OK;

	case Tool::EClientMakePath::TriggerBox_ChangeLevel:				ImGuiUpdate_TriggerBox_ChanageLevel_Desc			(static_cast<TRIGGERBOX_CHANGELEVEL_DESC*>(pDesc));						return S_OK;
	case Tool::EClientMakePath::TriggerBox_MonsterSpawner:			ImGuiUpdate_TriggerBox_MonsterSpawner				(static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*>(pDesc));					return S_OK;
	case Tool::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:	ImGuiUpdate_TriggerBox_GlobalEvent_BroadCaster		(static_cast<TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC*>(pDesc));			return S_OK;
	case Tool::EClientMakePath::TriggerBox_TutorialUIEvent:			ImGuiUpdate_TriggerBox_TutorialUIEvent				(static_cast<TRIGGERBOX_TUTORIALUIEVENT_DESC*>(pDesc));					return S_OK;
	default:																																													return S_OK;
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


#pragma region Plants


void CPanel_MapObjectList::ImGuiUpdate_Plants_Desc(PLANTS_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGui::SeparatorText(" Plants Description ");
	
	ImGui::NewLine();

	ImGui::Text(" if Instance Model) Index 0 Descripton Color Only Apply  ");

	ImGui::NewLine();

	ImGui::ColorEdit4("Plant Color", (float*)&pDesc->vMITint_Color);

	ImGui::NewLine();

	ImGui::NewLine();

	ImGui::DragFloat("Plant Diffuse Color Power", &pDesc->fDiffuseColorPower,0.001f,0.f,100.f,"%.3f");

	ImGui::NewLine();

	ImGui::Separator();

	return;
}


#pragma region Tree
void CPanel_MapObjectList::ImGuiUpdate_Tree_Desc(TREE_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGuiUpdate_Plants_Desc(pDesc);

	return;
}
#pragma endregion

#pragma region Moss
void CPanel_MapObjectList::ImGuiUpdate_Moss_Desc(MOSS_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGuiUpdate_Plants_Desc(pDesc);

	return;
}
#pragma endregion

#pragma region Bush
void CPanel_MapObjectList::ImGuiUpdate_Bush_Desc(BUSH_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGuiUpdate_Plants_Desc(pDesc);

	return;
}
#pragma endregion


#pragma region Grass
void CPanel_MapObjectList::ImGuiUpdate_Grass_Desc(GRASS_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGuiUpdate_Plants_Desc(pDesc);

	ImGui::SeparatorText("Grass Data");

	ImGui::Text(" DT => [ %f ] " , pDesc->fGrassDT );

	ImGui::NewLine();

	ImGui::Text(" Max Height => [ %f ]" , pDesc->fGrassMaxHeight);

	ImGui::NewLine();

	ImGui::Text(" Sway Speed ");
	ImGui::DragFloat("##Grass Sway Speed",&pDesc->fGrassSwaySpeed , 0.01f , 0.f , 1000.f , "%.2f");
	ImGui::NewLine();

	ImGui::Text(" Wave Size ");
	ImGui::DragFloat("##Grass Wave Size", &pDesc->fGrassWaveSize, 0.01f, 0.f, 1000.f, "%.2f");
	ImGui::NewLine();

	ImGui::Separator();

	return;
}
#pragma endregion

#pragma region Vine
void CPanel_MapObjectList::ImGuiUpdate_Vine_Desc(VINE_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGuiUpdate_Plants_Desc(pDesc);


	return;
}

void CPanel_MapObjectList::ImGuiUpdate_Water_Desc(WATER_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGui::SeparatorText(" Plants Description ");

	ImGui::NewLine();

	ImGui::Text(" if Instance Model) Index 0 Descripton Color Only Apply  ");

	ImGui::NewLine();

	ImGui::ColorEdit4("Water Color", (float*)&pDesc->vMI_TintColor);

	ImGui::NewLine();

	ImGui::SeparatorText("Normal Data");

	ImGui::DragFloat2("Water Speed 1", (float*)&pDesc->vSpeed1,0.01f,0.f,100.f,"%.3f");
	ImGui::DragFloat2("Water Speed 2", (float*)&pDesc->vSpeed2,0.01f, 0.f, 100.f, "%.3f");

	ImGui::NewLine();

	ImGui::DragFloat2("UV Power", (float*)&pDesc->vWaterUVPower,0.f,0.1f,1000.f, "%.3f");

	ImGui::NewLine();

	ImGui::SeparatorText("Noise Data");

	ImGui::DragFloat2("Distortion UV Power ", (float*)&pDesc->vDistortionUVPower, 0.01f , 0.f, 1000.f, "%.3f");
	ImGui::DragFloat2("Distortion Speed ", (float*)&pDesc->vDistortionSpeed, 0.01f, 0.f, 100.f, "%.3f");
	ImGui::DragFloat("Distortion Power", (float*)&pDesc->fDistortionPower, 0.001f, 0.f, 10.f, "%.3f");

	ImGui::Separator();

	ImGui::NewLine();

	ImGui::SeparatorText("Light Data");

	ImGui::DragFloat2("Sparkle UV Power ", (float*)&pDesc->vSparkleUVPower, 0.01f, 0.f, 1000.f, "%.3f");
	ImGui::DragFloat("Sparkle Power", (float*)&pDesc->fSparklePower, 0.001f, 0.f, 10.f, "%.3f");

	ImGui::NewLine();

	ImGui::Separator();

	string strTextureSlotName{};
	string strTextureName{};
	for (_uint i = 0; i < ENUM_TO_UINT(EWaterTextureType::END); ++i)
	{
		ImGui::PushID(i);
		strTextureSlotName = WaterTextureType_ToString(static_cast<EWaterTextureType>(i));
		auto& pTextureBase = pDesc->arrayTextureBase[i];
		strTextureName = pTextureBase == nullptr ? "None" : Engine_Utils::ToString(pTextureBase->Get_Name());

		ImGui::SeparatorText(strTextureSlotName.c_str());
		ImGui::Text("Texture Name		=> [ %s ] " , strTextureName.c_str());

		ID3D11ShaderResourceView* pSRV = pTextureBase == nullptr ? m_pMapToolManager->m_pDefaultWhiteSRV : pTextureBase->Get_SRV();

		if (ImGui::ImageButton("Texture", ImTextureRef(pSRV),ImVec2(32,32)))
		{
			// 타겟 주소를 넘겨주는 아주 훌륭한 로직!
			m_pMapToolManager->m_ppTargetSlot = &pDesc->arrayTextureBase[i];
			m_pMapToolManager->m_isTexArraySelect = false;
			m_pMapToolManager->m_isTex_DH_ArraySelect = false;
			m_pMapToolManager->m_isTex_NBR_ArraySelect = false;

			// 여기서 팝업 상태를 켠다
			ImGui::OpenPopup("Texture_Select_Modal");
		}

		ImGui::Separator();

		m_pMapToolManager->Select_MapTexture();

		ImGui::PopID();
	}
}

void CPanel_MapObjectList::ImGuiUpdate_Env_Desc(ENV_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	// 1. 전체 원본 태그 목록 (MapToolManager)
	auto& masterTags = m_pMapToolManager->m_vecEnvEffectTags;
	// 2. 현재 pDesc가 가진 상세 정보 리스트 (Tag + Desc)
	auto& targetInfos = pDesc->vecEnvEffectInfo;

	ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "=== [Environment Effect Settings] ===");

	// --- [섹션 1: 신규 이펙트 추가] ---
	static int iSelectedMasterIdx = 0;
	const char* preview = masterTags.empty() ? "No Tags" : masterTags[iSelectedMasterIdx].c_str();

	ImGui::PushItemWidth(-80.f);
	if (ImGui::BeginCombo("##AddCombo", preview))
	{
		for (int n = 0; n < (int)masterTags.size(); n++)
		{
			if (ImGui::Selectable(masterTags[n].c_str(), iSelectedMasterIdx == n))
				iSelectedMasterIdx = n;
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("Add", ImVec2(-FLT_MIN, 0)))
	{
		if (!masterTags.empty())
		{
			// 데이터 구조체 생성 및 추가
			ENV_EFFECT_INFO tNewInfo{};
			tNewInfo.strTags = masterTags[iSelectedMasterIdx];
			// 기본값 설정 (필요시)
			tNewInfo.tDesc.VFX_Scale = { 1.f, 1.f, 1.f };

			targetInfos.push_back(tNewInfo);

			// 실제 맵 객체에도 즉시 생성 요청
			if (m_pSelectMapObject)
				m_pSelectMapObject->Add_EnvEffect(tNewInfo.strTags);
		}
	}

	ImGui::Separator();

	// --- [섹션 2: 이펙트 인스턴스 리스트 및 오프셋 수정] ---
	if (m_pSelectMapObject)
	{
		auto pEnvList = m_pSelectMapObject->Get_EffectEnvData(); // vector<pair<CEffect_Env*, EFFECT_ENV_DESC>>*

		// 데이터와 실제 객체 리스트 개수 체크 (싱크 확인용)
		size_t iCount = targetInfos.size();

		for (size_t i = 0; i < iCount; ++i)
		{
			ImGui::PushID((int)i);

			// 헤더 이름 결정 (객체가 있으면 객체 이름, 없으면 태그 이름)
			string strDisplayName = targetInfos[i].strTags;
			if (pEnvList && i < pEnvList->size())
				strDisplayName = (*pEnvList)[i].first->Get_Name();

			char szLabel[MAX_PATH];
			sprintf_s(szLabel, "[%d] %s", (int)i, strDisplayName.c_str());

			if (ImGui::CollapsingHeader(szLabel, ImGuiTreeNodeFlags_DefaultOpen))
			{
				// pDesc(데이터)에 있는 Desc를 수정
				bool bChanged = false;
				EFFECT_ENV_DESC& tDataDesc = targetInfos[i].tDesc;

				if (ImGui::DragFloat3("Local Pos", (float*)&tDataDesc.VFX_Target_Position, 0.05f)) bChanged = true;
				if (ImGui::DragFloat3("Local Rot", (float*)&tDataDesc.VFX_Rotation, 0.5f)) bChanged = true;
				if (ImGui::DragFloat3("Local Scale", (float*)&tDataDesc.VFX_Scale, 0.01f)) bChanged = true;

				// 값이 변경되었다면 실제 렌더링 중인 객체에도 전달
				if (bChanged && pEnvList && i < pEnvList->size())
				{
					m_pSelectMapObject->Set_EnvEffectDesc((_uint)i, tDataDesc);
				}

				// 삭제 버튼
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
				if (ImGui::Button("Delete Instance", ImVec2(-FLT_MIN, 0)))
				{
					// 1. 실제 객체 삭제
					m_pSelectMapObject->Delete_EnvEffect((_uint)i);
					// 2. JSON 저장용 데이터 삭제
					targetInfos.erase(targetInfos.begin() + i);

					ImGui::PopStyleColor();
					ImGui::PopID();
					break;
				}
				ImGui::PopStyleColor();
			}
			ImGui::PopID();
		}
	}
	else
	{
		ImGui::TextDisabled("Select a MapObject to edit effects.");
	}
}

void CPanel_MapObjectList::ImGuiUpdate_Fog_Desc(FOG_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	// 1. 공통 속성 (컬러)
	ImGui::SeparatorText(" Fog Global Settings ");
	ImGui::NewLine();
	ImGui::ColorEdit4(" Tint Color ", (float*)&pDesc->vMI_TintColor);
	ImGui::NewLine();
	ImGui::DragFloat(" Distortion Power ",&pDesc->fDistortionPower,0.001f,0.f,10000.f,"%.3f");
	ImGui::NewLine();
	// 2. 텍스처 배열 순회
	string strTextureSlotName{};
	string strTextureName{};

	for (_uint i = 0; i < ENUM_TO_UINT(EFogTextureType::END); ++i)
	{
		// ID가 겹치지 않게 무조건 PushID로 감싼다!
		ImGui::PushID(i);

		CTextureBase* pTextureBase = pDesc->arrayTextureBase[i];
		strTextureSlotName = FogTextureType_ToString(static_cast<EFogTextureType>(i));
		strTextureName = (pTextureBase == nullptr) ? "None" : Engine_Utils::ToString(pTextureBase->Get_Name());

		ImGui::SeparatorText(strTextureSlotName.c_str());

		// ----------------------------------------------------
		// [왼쪽 영역] 텍스처 이미지 버튼
		// ----------------------------------------------------
		ImGui::BeginGroup();
		ID3D11ShaderResourceView* pSRV = (pTextureBase == nullptr) ? m_pMapToolManager->m_pDefaultWhiteSRV : pTextureBase->Get_SRV();

		// 이미지 크기를 64x64 정도로 좀 키우면 클릭하기도 편하고 보기 좋아!
		if (ImGui::ImageButton("TextureBtn", ImTextureRef(pSRV), ImVec2(64, 64)))
		{
			m_pMapToolManager->m_ppTargetSlot = &pDesc->arrayTextureBase[i];
			m_pMapToolManager->m_isTexArraySelect = false;
			m_pMapToolManager->m_isTex_DH_ArraySelect = false;
			m_pMapToolManager->m_isTex_NBR_ArraySelect = false;

			ImGui::OpenPopup("Texture_Select_Modal");
		}
		ImGui::EndGroup();

		// 줄바꿈 하지 말고 바로 옆에 붙여라!
		ImGui::SameLine();

		// ----------------------------------------------------
		// [오른쪽 영역] 텍스처 정보 및 조절 슬라이더
		// ----------------------------------------------------
		ImGui::BeginGroup();
		ImGui::Text("Name : %s", strTextureName.c_str());

		// 핵심! 인덱스 [i]를 넣어서 각 텍스처마다 개별적인 Speed와 Power를 조작하게 만듦!
		// DragFloat2 앞의 글자 길이 맞추려고 띄어쓰기 살짝 넣으면 더 이쁨
		ImGui::DragFloat2("UV Speed", (float*)&pDesc->vUV[i].x, 0.001f, -100.f, 100.f, "%.3f");
		ImGui::DragFloat2("UV Power", (float*)&pDesc->vUV[i].z, 0.01f, 0.f, 1000.f, "%.2f");
		ImGui::EndGroup();

		// 다음 텍스처 슬롯과의 구분을 위해 약간의 여백 추가
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		// 팝업 모달 (기존 로직 유지)
		m_pMapToolManager->Select_MapTexture();

		ImGui::PopID();
	}

}

#pragma endregion

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

	_bool isChange{false};
	if (ImGui::BeginCombo("Batch Object Type List##Batch Object Desc", m_strBuffer.c_str()))
	{
		for (_uint i = 0; i < ENUM_TO_UINT(DTO::EMakeObjectType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(m_szMakeObjectTypeName[i], &isSelected))
			{
				pDesc->Change_BatchObjecType(DTO::EMakeObjectType(i));
				m_pSelectMapObject->Ready_Batch_Object();
				isChange = true;
				break;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();

	DTO::EMakeObjectType eType = pDesc->eBatchObjectType;


	if (isChange == false)
	{
		switch (eType)
		{
		case DTO::EMakeObjectType::Battle_Field:		ImGuiUpdate_Battle_Field_Desc(static_cast<BATTLE_FIELD_DESC*>(pDesc->pBatchObjectDesc));	break;
		case DTO::EMakeObjectType::PointLight:			ImGuiUpdate_PointLight_Desc(static_cast<POINTLIHGT_DESC*>(pDesc->pBatchObjectDesc));	break;
		default:																															return;
		}
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

void CPanel_MapObjectList::ImGuiUpdate_PointLight_Desc(POINTLIHGT_DESC* pDesc)
{
	if (pDesc == nullptr) return;
	
	ImGui::SeparatorText("Point Light Settings");

	// 1. 색상 관련 설정 (Diffuse, Ambient, Specular)
	// Vector4의 x, y, z, w를 float*로 캐스팅하여 사용합니다.
	ImGui::ColorEdit4("Diffuse", (float*)&pDesc->tLightDesc.vDiffuse);
	ImGui::ColorEdit4("Ambient", (float*)&pDesc->tLightDesc.vAmbient);
	ImGui::ColorEdit4("Specular", (float*)&pDesc->tLightDesc.vSpecular);

	ImGui::Separator();

	// Flicker 중일 때는 BaseRange를 건드려야 원본이 유지됩니다.
	if (ImGui::DragFloat("Base Range", &pDesc->fBaseRange, 0.1f, 0.0f, 500.0f))
	{
		// 깜빡이지 않을 때는 즉시 적용되도록 처리
		if (!pDesc->isFlicker)
		{
			pDesc->tLightDesc.fRange = pDesc->fBaseRange;
		}
	}

	ImGui::SeparatorText("Flicker Options");

	// 3. 깜빡임(Flicker) 관련 설정
	ImGui::Checkbox("Enable Flicker", (bool*)&pDesc->isFlicker);

	if (pDesc->isFlicker)
	{
		ImGui::Indent(); // 옵션 구분을 위해 들여쓰기
		ImGui::DragFloat("Speed", &pDesc->fFlickerSpeed, 0.001f,0.f,1000.f,"%.3f");
		ImGui::DragFloat("Min Ratio", &pDesc->fFlickerMin,0.0001f, 0.f,1.f,"%.4f");

		// 현재 적용 중인 실시간 Range 확인 (Read Only)
		ImGui::Text("Current Range: %.2f", pDesc->pDebugLight->Get_LightDesc().fRange);
		ImGui::Unindent();
	}
	return;
}

#pragma endregion

#pragma region Trigger Box

void CPanel_MapObjectList::ImGuiUpdate_TriggerBox(TRIGGERBOX_DESC* pDesc)
{
	ImGui::SeparatorText(" 'Trigger Box Desc' ");

	if (ImGui::DragFloat3("Extents##TriggerBox_Extents", &pDesc->vExtents.x, 0.1f, 0.1f, 100.f, "%.2f"))
	{
		if (m_pSelectMapObject == nullptr) return;
		m_pSelectMapObject->Update_Collider();
	}

	Vec3 vDegree = Vec3( XMConvertToDegrees(pDesc->vRotation.x), XMConvertToDegrees(pDesc->vRotation.y), XMConvertToDegrees(pDesc->vRotation.z));
	if (ImGui::DragFloat3("Rotation##TriggerBox_Rotation", &vDegree.x, 0.1f,-360.f,360.f, "%.2f"))
	{
		pDesc->vRotation = Vec3(XMConvertToRadians(vDegree.x), XMConvertToRadians(vDegree.y), XMConvertToRadians(vDegree.z));
		if (m_pSelectMapObject == nullptr) return;
		m_pSelectMapObject->Update_Collider();
	}

	ImGui::Separator();

	ImGui::Checkbox("Is Quest Object", &pDesc->bHasQuest);

	if (pDesc->bHasQuest)
	{
		ImGui::Indent();
		ImGui::SeparatorText(" Quest List Configuration ");

		if (ImGui::Button(" + Add New Quest "))
		{
			pDesc->tQuestObjectDesc.push_back(DTO::QUEST_CHAPTERDESC());
		}

		ImGui::Spacing();

		for (int i = 0; i < pDesc->tQuestObjectDesc.size(); )
		{
			ImGui::PushID(i);

			string strNodeName = "Quest Index [" + std::to_string(i) + "] - Chap ID: "
				+ std::to_string(pDesc->tQuestObjectDesc[i].tQuestDesc.iId);

			bool bNodeOpen = ImGui::TreeNode((void*)(intptr_t)i, strNodeName.c_str());

			ImGui::SameLine(ImGui::GetWindowWidth() - 80.f);
			if (ImGui::Button("Delete"))
			{
				pDesc->tQuestObjectDesc.erase(pDesc->tQuestObjectDesc.begin() + i);
				if (bNodeOpen) ImGui::TreePop();
				ImGui::PopID();
				continue;
			}

			if (bNodeOpen)
			{
				ImGuiUpdate_Quest(&pDesc->tQuestObjectDesc[i]);
				ImGui::TreePop();
			}

			ImGui::PopID();
			++i;
		}
		ImGui::Unindent();
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
void CPanel_MapObjectList::ImGuiUpdate_TriggerBox_GlobalEvent_BroadCaster(TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC* pDesc)
{
	if (pDesc == nullptr) return;
	ImGuiUpdate_TriggerBox(pDesc);


	ImGui::SeparatorText(" Global Event BroadCater ");

	ImGui::SeparatorText(" Global Event BroadCaster ");

	// 1. 이벤트 추가 버튼
	if (ImGui::Button("Add Event"))
	{
		// 추가할 때 기본값으로 "NONE"을 넣어줍니다.
		pDesc->vecGlobalEventBroadCasetNames.push_back("NONE");
	}

	ImGui::Separator();

	// 2. Vector 순회하며 콤보박스 그리기
	for (int i = 0; i < pDesc->vecGlobalEventBroadCasetNames.size(); ++i)
	{
		ImGui::PushID(i);

		int iBuffer = static_cast<int>(Global_Broadcast_Type_ToEnum(pDesc->vecGlobalEventBroadCasetNames[i]));

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40.f);

		if (ImGui::Combo("##EventCombo", &iBuffer, g_szGlobalBroadCastType, (int)EGlobal_Broadcast_Type::END))
		{
			pDesc->vecGlobalEventBroadCasetNames[i] = Global_Broadcast_Type_ToString(static_cast<EGlobal_Broadcast_Type>(iBuffer));
		}

		ImGui::SameLine();

		// 3. 요소 삭제 버튼 (X)
		if (ImGui::Button("X"))
		{
			auto iter = pDesc->vecGlobalEventBroadCasetNames.begin() + i;
			pDesc->vecGlobalEventBroadCasetNames.erase(iter);

			ImGui::PopID();
			--i;
			continue;
		}

		// ID 팝
		ImGui::PopID();
	}

	ImGui::Separator();
}
void CPanel_MapObjectList::ImGuiUpdate_TriggerBox_TutorialUIEvent(TRIGGERBOX_TUTORIALUIEVENT_DESC* pDesc)
{
	if (pDesc == nullptr) return;

	ImGuiUpdate_TriggerBox(pDesc);

	ImGui::Text("Event Name => [ %s ]", pDesc->strEventName.c_str());

	EUITutorialPopUpTypeID eID = UITutorialPopUpTypeID_ToEnum(pDesc->strEventName);
	_int				   iID = static_cast<_int>(eID);

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40.f);

	if (ImGui::Combo("##EventCombo", &iID, g_szTutorialUIEvent, (int)EUITutorialPopUpTypeID::END))
	{
		pDesc->strEventName = UITutorialPopUpTypeID_ToString(static_cast<EUITutorialPopUpTypeID>(iID));
	}


}
void CPanel_MapObjectList::ImGuiUpdate_Quest(DTO::QUEST_CHAPTERDESC* pDesc)
{
	ImGui::Indent();
	ImGui::SeparatorText(" Quest Configuration ");

	DTO::QUEST_CHAPTERDESC& chapterDesc = *pDesc;
	DTO::QUESTDESC& questDesc = chapterDesc.tQuestDesc;

	const char* eventTypes[] = { "MONSTER_KILL", "NPC_TALK", "AREA_ENTER", "AREA_EXIT", "OBJECT_INTERACT" };
	ImGui::Combo("Event Type", (int*)&chapterDesc.eEvent, eventTypes, IM_ARRAYSIZE(eventTypes));

	const char* layerTypes[] = { "SCENARIO", "CHAPTER" };
	ImGui::Combo("Quest Layer", (int*)&questDesc.eType, layerTypes, IM_ARRAYSIZE(layerTypes));

	const char* stateTypes[] = { "LOCKED", "AVAILABLE", "IN_PROGRESS", "COMPLETE" };
	ImGui::Combo("Initial State", (int*)&questDesc.eState, stateTypes, IM_ARRAYSIZE(stateTypes));

	ImGui::InputInt("Chapter ID", &questDesc.iId);
	ImGui::InputInt("Parent (Scenario) ID", &questDesc.iParentId);
	ImGui::InputInt("Prev ID", &questDesc.iPrevId);
	ImGui::InputInt("Next ID", &questDesc.iNextId);
	ImGui::InputInt("Target Count", &chapterDesc.iCount);

	ImGui::SeparatorText(" Target Types ");

	static int addTargetEnum = 0;
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputInt("##AddTargetEnum", &addTargetEnum);
	ImGui::SameLine();
	if (ImGui::Button("Add Target"))
	{
		chapterDesc.eTargetType.insert(static_cast<OBJECT_ENUM_TAG::Enum>(addTargetEnum));
	}

	if (ImGui::BeginListBox("##TargetList", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (auto it = chapterDesc.eTargetType.begin(); it != chapterDesc.eTargetType.end(); )
		{
			ImGui::Text("Enum ID: %d", *it);
			ImGui::SameLine(ImGui::GetWindowWidth() - 50.f);

			ImGui::PushID(*it);
			if (ImGui::Button("Del"))
				it = chapterDesc.eTargetType.erase(it);
			else
				++it;
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}

	ImGui::SeparatorText(" Quest Texts ");

	auto ImGuiInputWString = [](const char* label, std::wstring& wstrTarget)
		{
			std::string tempStr(wstrTarget.begin(), wstrTarget.end());
			char buffer[256];
			strcpy_s(buffer, tempStr.c_str());

			if (ImGui::InputText(label, buffer, sizeof(buffer)))
			{
				std::string newStr(buffer);
				wstrTarget = std::wstring(newStr.begin(), newStr.end());
			}
		};

	ImGuiInputWString("Title", questDesc.wstrTitle);
	ImGuiInputWString("SubTitle", questDesc.wstrSubTitle);
	ImGuiInputWString("Explain", questDesc.wstrExplain);
	ImGuiInputWString("Description", questDesc.wstrDescription);

	ImGui::InputInt("Enter Dialogue ID", &chapterDesc.tQuestDesc.iEnterDialogueId);
	ImGui::InputInt("Exit Dialogue ID", &chapterDesc.tQuestDesc.iExitDialogueId);
	ImGui::InputInt("Interact Dialogue ID", &chapterDesc.tQuestDesc.iInteractDialogueId);

	ImGui::Unindent();
}
#pragma endregion



#pragma endregion

#pragma endregion

#pragma endregion