#include "pch.h"
#include "Panel_MapObjectList.h"
#include "StaticModel.h"
#include "InstanceModel.h"
#include "Engine_Utils.h"
#include "ImGui_Layout_Transform.h"
#include "Level_Map.h"
#include "CameraMan.h"
#include "Camera.h"
#include "Panel_FileExplore.h"
#include "Panel_MapTool.h"
#include "Model.h"
#include <fstream>
#include "InstanceMesh.h"
#include "AsTypes.h"

USING(Tool)

CPanel_MapObjectList::CPanel_MapObjectList(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext), m_pGameInstance(CGameInstance::GetInstance()), m_pTransformLayout(nullptr), m_pCamera(nullptr)
	, m_pCameraCom(nullptr), m_wszMapObjectLayerTag{}, m_szFindName{}, m_iSelectInstanceID{} 
	, m_arrayOriginMtlUsingTexturesName{}, m_isShowOriginMtlInfo{ false }, m_iSelectOriginMtlTexture{0}
{

	m_pTransformLayout = CImGui_Layout_Transform::Create("Layout_Transform", m_pDevice, m_pDeviceContext);

	Safe_AddRef(m_pGameInstance);

	m_pCamera = static_cast<Engine::CCameraMan*>(m_pGameInstance->Get_MainCamera());

	m_pCameraCom = m_pCamera->Get_Component<CCamera>();

	m_arrayMapObjectList.fill(nullptr);
	m_arrayOriginMtlUsingTexturesName.fill("");
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
	lstrcpyW(m_wszMapObjectLayerTag[ENUM_TO_UINT(EMapObject_Type::STATICMODEL)],g_wszStaticModelLayer);
	lstrcpyW(m_wszMapObjectLayerTag[ENUM_TO_UINT(EMapObject_Type::INSTANCEMODEL)],g_wszInstanceModelLayer);
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

	m_pSelectMapObject = static_cast<CMapObject*>(static_cast<CLevel_Map*>(m_pOwnerLevel)->Get_SelectToolObject());

}


HRESULT CPanel_MapObjectList::Update_MapObjectList()
{
	for (_uint i = 0; i < ENUM_TO_UINT(EMapObject_Type::END); ++i)
	{
		m_arrayMapObjectList[i] = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP), m_wszMapObjectLayerTag[i]);
	}

	return S_OK;
}


void CPanel_MapObjectList::Reset_SelectValue()
{
	m_iSelectOriginMtlTexture = 0;
	m_iSelectInstanceID = 0;
	m_pSelectMapObject = nullptr;


	m_iSelectOverrideMtlTextureID = 0;
	m_iSelectOverrideMtlID = -1;

	m_isShowOriginMtlInfo = false;
	m_arrayOriginMtlUsingTexturesName.fill("");


	m_strOriginMtlPath.clear();
	m_strOriginMtlName.clear();
}


HRESULT CPanel_MapObjectList::Render_MapObjectList()
{
	ImGui::Begin(m_strLabel.c_str());

	if(FAILED(Update_MapObjectList()))
		return E_FAIL;


	ImGui::SeparatorText(" Function ");

	if (ImGui::Button("All Layer Clear"))
	{
		m_pSelectMapObject = nullptr;
		static_cast<CLevel_Map*>(m_pOwnerLevel)->Set_SelectToolObjectNull();
		for (_uint i = 0; i < static_cast<_uint>(EMapObject_Type::END); ++i)
			m_pGameInstance->Clear_Layer(ENUM_TO_UINT(ELevelType::MAP) , m_wszMapObjectLayerTag[i]);
	}

	ImGui::Separator();

	if (ImGui::BeginCombo(" Layer Select ", Engine_Utils::ToString(m_wszMapObjectLayerTag[m_iSelectLayerTag]).c_str()))
	{
		for (_uint i = 0; i < static_cast<_uint>(EMapObject_Type::END); ++i)
		{
			bool isSelect = i == m_iSelectLayerTag;
			if (ImGui::Selectable(Engine_Utils::ToString(m_wszMapObjectLayerTag[i]).c_str(), isSelect))
				m_iSelectLayerTag = i;
			if (isSelect)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Select Layer Tag Clear"))
	{
		m_pSelectMapObject = nullptr;
		m_pGameInstance->Clear_Layer(ENUM_TO_UINT(ELevelType::MAP), m_wszMapObjectLayerTag[m_iSelectLayerTag]);
	}


	ImGui::Separator();


	ImGui::NewLine();
	
	ImGui::InputText("Search", m_szFindName, MAX_PATH);
	
	ImGui::NewLine();

	ImGui::Separator();


	if (ImGui::CollapsingHeader("Map Object List"))
	{
		_uint i = 0;
		for (auto& MapObjectList : m_arrayMapObjectList)
		{
			list<CGameObject*>* pListMapObjectLayer = MapObjectList;
			string strListName = Engine_Utils::ToString(wstring(m_wszMapObjectLayerTag[i]));
			if (ImGui::TreeNode(strListName.c_str()))
			{

				if (!pListMapObjectLayer || pListMapObjectLayer->empty())
				{
					ImGui::Text(" Empty Layer");
				}
				else
				{
					UINT32 iIndex = 0;
					for (auto& MapObject : *pListMapObjectLayer)
					{
						if (MapObject)
						{
							CMapObject* pMapObject = static_cast<CStaticModel*>(MapObject);
							string strModelName = pMapObject->Get_Name();

							if (m_pSelectMapObject && pMapObject == m_pSelectMapObject)
							{
								if (pMapObject->Get_MapObjectType() == EMapObject_Type::INSTANCEMODEL)
									strModelName += " =>  [ " + std::to_string(static_cast<CInstanceModel*>(pMapObject)->Get_InstanceCount()) + " ] ";

								ImGui::TextColored(ImVec4(1.0, 0.f, 0.f, 1.f), strModelName.c_str());
							}
							else
							{
								if (strlen(m_szFindName) > 0)
								{
									if (strModelName.find(m_szFindName) == string::npos)
										continue;
									else
									{
										if (pMapObject->Get_MapObjectType() == EMapObject_Type::INSTANCEMODEL)
											strModelName += " =>  [ " + std::to_string(static_cast<CInstanceModel*>(pMapObject)->Get_InstanceCount()) + " ] ";

										ImGui::Text(strModelName.c_str());
									}
								}
								else
								{
									if (pMapObject->Get_MapObjectType() == EMapObject_Type::INSTANCEMODEL)
										strModelName += " =>  [ " + std::to_string(static_cast<CInstanceModel*>(pMapObject)->Get_InstanceCount()) + " ] ";

									ImGui::Text(strModelName.c_str());
								}
							}
							if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
							{
								Reset_SelectValue();
								static_cast<CLevel_Map*>(m_pOwnerLevel)->On_ChangeSelectedObject(MapObject);
							}
							else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
								m_pCamera->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, pMapObject->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS));
						}
					}
				}
				ImGui::TreePop();
			}
			++i;
		}
	}

	ImGui::End();

	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_SelectInfo()
{
	ImGui::Begin(" Select Info ");

	if (m_pSelectMapObject == nullptr)
	{
		ImGui::Text(" Select Map Object is Empty ");
	}
	else
	{
		EMapObject_Type eMapObjectType = static_cast<CMapObject*>(m_pSelectMapObject)->Get_MapObjectType();

		ImGui::NewLine();

		ImGui::Text( " Map Object Type : [ %s ] " , MapObjectType_TypeToString(eMapObjectType).c_str());
		ImGui::Text(" Map Object Name : [ %s ]", m_pSelectMapObject->Get_Name().c_str());

		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button(" Delete "))
		{
			m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), m_pSelectMapObject->Get_LayerTag(), m_pSelectMapObject);
			static_cast<CLevel_Map*>(m_pOwnerLevel)->Set_SelectToolObjectNull();
			ImGui::End();
			return S_OK;
		}
		ImGui::SameLine();
		if (ImGui::Button(" Cancel Select "))
		{
			static_cast<CLevel_Map*>(m_pOwnerLevel)->Set_SelectToolObjectNull();
			ImGui::End();
			return S_OK;
		}

		ImGui::Separator();

		switch (eMapObjectType)
		{
		case Tool::EMapObject_Type::STATICMODEL:
			Render_StaticModel();
			break;
		case Tool::EMapObject_Type::INSTANCEMODEL:
			Render_InstanceModel();
			break;
		case Tool::EMapObject_Type::END:
			break;
		default:
			break;
		}
	}
	ImGui::End();

	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_StaticModel()
{
	CStaticModel* pStaticModel = static_cast<CStaticModel*>(m_pSelectMapObject);

	STATICMODEL_DATA tStaticModelData = pStaticModel->Get_Data();

	if (ImGui::BeginTabBar(" StaticModel "))
	{
		if (ImGui::BeginTabItem(" Model Info "))
		{
			Render_ModelInfo(tStaticModelData.tUsingModelInfo, pStaticModel->Get_Component<CModel>());
			
			ImGui::EndTabItem();
		}

#pragma region Transform
		if (ImGui::BeginTabItem(" Transform "))
		{
			ImGui::SeparatorText("Reset / Resister");

			if (ImGui::Button(" Reset "))
			{
				pStaticModel->Reset_OriginTransform(0);
			}
			ImGui::SameLine();
			if (ImGui::Button(" Register "))
			{
				pStaticModel->Override_OriginTransform(0);
			}
#pragma region Origin SRT
			if (ImGui::TreeNode(" Origin SRT "))
			{
				Vec3 vOriginScale{}, vOriginPosition{};
				Quat vOriginQuat{};
				pStaticModel->Get_OriginTransform(0).Decompose(vOriginScale, vOriginQuat, vOriginPosition);

				ImGui::Text(" Scale	=> X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]", vOriginScale.x, vOriginScale.y, vOriginScale.z);
				ImGui::Text(" Degree	=> X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]  W : [%.2f] ", vOriginQuat.x, vOriginQuat.y, vOriginQuat.z , vOriginQuat.w);
				ImGui::Text(" Position => X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]", vOriginPosition.x, vOriginPosition.y, vOriginPosition.z);

				ImGui::TreePop();
			}
#pragma endregion

			ImGui::Separator();

			ImGui::NewLine();


			if (ImGui::BeginTabBar("Current SRT Info"))
			{

				if (ImGui::BeginTabItem("Gizmo Transform"))
				{
					m_pTransformLayout->Render(m_pSelectMapObject);

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(" Transform "))
				{

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::EndTabItem();

#pragma endregion
		}

		ImGui::EndTabBar();
	}


	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_InstanceModel()
{
	CInstanceModel* pInstanceModel = static_cast<CInstanceModel*>(m_pSelectMapObject);

	INSTANCEMODEL_DATA& tInstanceModelData = pInstanceModel->Get_Data();

	if (ImGui::BeginTabBar(" InstanceModel "))
	{
		if (ImGui::BeginTabItem(" Model Info "))
		{
			Render_ModelInfo(tInstanceModelData.tUsingModelInfo , pInstanceModel->Get_Component<CModel>());

			ImGui::EndTabItem();
		}

#pragma region Transform
		if (ImGui::BeginTabItem(" Transform "))
		{
			_int iCountInstance = pInstanceModel->Get_InstanceCount();
			m_iSelectInstanceID = pInstanceModel->Get_iSelectInstanceID();
			if (m_iSelectInstanceID > iCountInstance - 1 || m_iSelectInstanceID < 0 )
				m_iSelectInstanceID = 0;

			ImGui::SliderInt("Instance", &m_iSelectInstanceID, 0, iCountInstance - 1, "Instance %d");
			pInstanceModel->Set_SelctInstanceID(m_iSelectInstanceID);

			ImGui::SeparatorText("Reset / Resister");

			if (ImGui::Button(" Reset "))
			{
				pInstanceModel->Reset_OriginTransform(m_iSelectInstanceID);
			}
			ImGui::SameLine();
			if (ImGui::Button(" Register "))
			{
				pInstanceModel->Override_OriginTransform(m_iSelectInstanceID);
			}
			if (ImGui::TreeNode(" Origin SRT "))
			{
				Vec3 vOriginScale{}, vOriginPosition{};
				Quat vOriginQuat{};
				pInstanceModel->Get_OriginTransform(m_iSelectInstanceID).Decompose(vOriginScale, vOriginQuat, vOriginPosition);

				ImGui::Text(" Scale	=> X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]", vOriginScale.x, vOriginScale.y, vOriginScale.z);
				ImGui::Text(" Degree	=> X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]  W : [%.2f] ", vOriginQuat.x, vOriginQuat.y, vOriginQuat.z, vOriginQuat.w);
				ImGui::Text(" Position => X : [ %.2f ]  Y : [ %.2f ]  Z : [ %.2f ]", vOriginPosition.x, vOriginPosition.y, vOriginPosition.z);

				ImGui::TreePop();
			}

			ImGui::Separator();

			ImGui::NewLine();

			if (ImGui::BeginTabBar("Current SRT Info"))
			{
				if (ImGui::BeginTabItem("Gizmo Transform"))
				{
					m_pTransformLayout->Render(m_pSelectMapObject);

					ImGui::EndTabItem();
				}


				if (ImGui::BeginTabItem(" Transform "))
				{
					Vec3& vScale	= tInstanceModelData.vecSRT[m_iSelectInstanceID].vScale;
					Vec3& vPosition = tInstanceModelData.vecSRT[m_iSelectInstanceID].vPosition;
					Quat& vQaut		= tInstanceModelData.vecSRT[m_iSelectInstanceID].vQuat;
					if (ImGui::DragFloat3("Scale##Instance_S", &vScale.x, 0.01f, 0.f, 500.f, "%.2f"))
					{
						tInstanceModelData.vecMatirx[m_iSelectInstanceID]  =  tInstanceModelData.vecSRT[m_iSelectInstanceID].Get_World();
						pInstanceModel->Get_Component<CInstanceMesh>()->Update_Matrix(tInstanceModelData.vecMatirx[m_iSelectInstanceID] , m_iSelectInstanceID);
					}
					if (ImGui::DragFloat4("Quaternion##Instance_R", &vQaut.x , 0.005f , 0.f, 500.f, "%.2f"))
					{
						tInstanceModelData.vecMatirx[m_iSelectInstanceID] = tInstanceModelData.vecSRT[m_iSelectInstanceID].Get_World();
						pInstanceModel->Get_Component<CInstanceMesh>()->Update_Matrix(tInstanceModelData.vecMatirx[m_iSelectInstanceID], m_iSelectInstanceID);
					}
					if (ImGui::DragFloat3("Position##Instance_T", &vPosition.x, 0.1f ,0.f,0.f , "%.2f"))
					{
						tInstanceModelData.vecMatirx[m_iSelectInstanceID] = tInstanceModelData.vecSRT[m_iSelectInstanceID].Get_World();
						pInstanceModel->Get_Component<CInstanceMesh>()->Update_Matrix(tInstanceModelData.vecMatirx[m_iSelectInstanceID], m_iSelectInstanceID);
					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::EndTabItem();

#pragma endregion
		}

		ImGui::EndTabBar();
	}
	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_ModelInfo(USING_MODEL_INFO& tModelInfo , CModel* pModel)
{
	if (pModel == nullptr) return E_FAIL;

	_uint iMtlCount = pModel->Get_MaterialCount();

	ImGui::Text("Model Name : [ %s ]", Engine_Utils::ToString(tModelInfo.wstrName).c_str());

	ImGui::SeparatorText(" Mateiral Info ");

	ImGui::PushID("Origin Matrial");
	for (_uint i = 0; i < iMtlCount; ++i)
	{
		ImGui::PushID(i);
		string mtlName = Engine_Utils::ToString(pModel->Get_MaterialName(i));
		ImGui::Text("[ %d ]  =>  [ %s ]", i, mtlName.c_str());

		if (ImGui::IsItemHovered())
		{
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
				ImGui::GetColorU32(ImGuiCol_HeaderHovered)
			);
		}

		if (ImGui::BeginPopupContextItem(string(mtlName + std::to_string(i)).c_str(), ImGuiPopupFlags_::ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::Selectable("Show Origin Mtl Info"))
			{
				/* Json 파일 읽기 */
				std::ifstream ifs{ tModelInfo.wstrMtl_JsonFile_Path};
				if (ifs.is_open())
				{
					m_arrayOriginMtlUsingTexturesName.fill("");
					json MtlJsons{};
					ifs >> MtlJsons;

					const auto& MtlJson = MtlJsons[i];

					if (MtlJson.contains("FILES"))
					{
						json TexJson = MtlJson["FILES"];

						if (TexJson.contains("DIFFUSE_FILE"))
							TexJson.at("DIFFUSE_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_DIFFUSE - 1]);
						if (TexJson.contains("SPECULAR_FILE"))
							TexJson.at("SPECULAR_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_SPECULAR - 1]);
						if (TexJson.contains("AMBIENT_FILE"))
							TexJson.at("AMBIENT_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_AMBIENT - 1]);
						if (TexJson.contains("EMISSIVE_FILE"))
							TexJson.at("EMISSIVE_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_EMISSIVE - 1]);
						if (TexJson.contains("HEIGHT_FILE"))
							TexJson.at("HEIGHT_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_HEIGHT - 1]);
						if (TexJson.contains("NORMAL_FILE"))
							TexJson.at("NORMAL_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_NORMALS - 1]);
						if (TexJson.contains("SHININESS_FILE"))
							TexJson.at("SHININESS_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_SHININESS - 1]);
						if (TexJson.contains("OPACITY_FILE"))
							TexJson.at("OPACITY_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_OPACITY - 1]);
						if (TexJson.contains("DISPLACEMENT_FILE"))
							TexJson.at("DISPLACEMENT_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_DISPLACEMENT - 1]);
						if (TexJson.contains("LIGHTMAP_FILE"))
							TexJson.at("LIGHTMAP_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_LIGHTMAP - 1]);
						if (TexJson.contains("REFLECTION_FILE"))
							TexJson.at("REFLECTION_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_REFLECTION - 1]);
						if (TexJson.contains("BASECOLOR_FILE"))
							TexJson.at("BASECOLOR_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_BASE_COLOR - 1]);
						if (TexJson.contains("NORMALCAMERA_FILE"))
							TexJson.at("NORMALCAMERA_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_NORMAL_CAMERA - 1]);
						if (TexJson.contains("EMISSIONCOLOR_FILE"))
							TexJson.at("EMISSIONCOLOR_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_EMISSION_COLOR - 1]);
						if (TexJson.contains("METALNESS_FILE"))
							TexJson.at("METALNESS_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_METALNESS - 1]);
						if (TexJson.contains("ROUGHNESS_FILE"))
							TexJson.at("ROUGHNESS_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_DIFFUSE_ROUGHNESS - 1]);
						if (TexJson.contains("AMBIENTOCCLUSION_FILE"))
							TexJson.at("AMBIENTOCCLUSION_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_AMBIENT_OCCLUSION - 1]);
						if (TexJson.contains("UNKNOWN_FILE"))
							TexJson.at("UNKNOWN_FILE").get_to(m_arrayOriginMtlUsingTexturesName[aiTextureType_UNKNOWN - 1]);
					}

					m_iSelectOriginMtlTexture = 0;
					m_isShowOriginMtlInfo = true;
					m_strOriginMtlPath = Engine_Utils::ToString(tModelInfo.wstrMtl_JsonFile_Path);
					m_strOriginMtlName = Engine_Utils::ToString(m_pSelectMapObject->Get_Component<CModel>()->Get_MaterialName(i));

				}
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
	ImGui::PopID();


	ImGui::Separator();

	ImGui::SeparatorText(" Override Material ");

	if (tModelInfo.vecOverrideMaterial.empty())
		ImGui::Text(" Override Material is Empty ");
	else
	{
		ImGui::PushID("Override Mateiral");
		_uint iCountOverrideMaterial = static_cast<_uint>(tModelInfo.vecOverrideMaterial.size());
		for (_uint i = 0; i < iCountOverrideMaterial; ++i)
		{
			ImGui::PushID(i);

			OVERRIDE_MATERIALS tOverrideMaterial = tModelInfo.vecOverrideMaterial[i];
			string strName = Engine_Utils::ToString(tOverrideMaterial.wstrMtl_JsonFile_Name);

			char szText[MAX_PATH]{};
			if(tOverrideMaterial.isNull)
				sprintf_s(szText, "[ %d ] => [ Null ]",i);
			else
				sprintf_s(szText, "[ %d ] => [ %s ]", i , 
					strName.c_str());
			ImGui::Text(szText);

			if (ImGui::IsItemHovered())
			{
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
					ImGui::GetColorU32(ImGuiCol_HeaderHovered)
				);
			}

			if (ImGui::BeginPopupContextItem(string(strName + std::to_string(i)).c_str(), ImGuiPopupFlags_::ImGuiPopupFlags_MouseButtonRight))
			{
				if (!tModelInfo.vecOverrideMaterial[i].isNull)
				{
					if (ImGui::Selectable("Show Override Material Info"))
					{
						m_iSelectOverrideMtlTextureID = 0;
						m_iSelectOverrideMtlID = i;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		ImGui::PopID();

	}


	ImGui::Separator();


	return S_OK;
}



HRESULT CPanel_MapObjectList::Render_SelectOverrideMaterialInfo()
{
	ImGui::Begin(" Override Material Info ");


	if ( m_iSelectOverrideMtlID == -1 || m_pSelectMapObject == nullptr )
	{
		ImGui::Text(" Override Material Is Empty");
		ImGui::End();
		return S_OK;
	}


	OVERRIDE_MATERIALS tOverrideMtl = m_pSelectMapObject->Get_UsingModelInfo().vecOverrideMaterial[m_iSelectOverrideMtlID];


	ImGui::SeparatorText(" Override Material Info ");
	ImGui::NewLine();
	ImGui::Text(" Material Name => [ %s ] ", Engine_Utils::ToString(tOverrideMtl.wstrMtl_JsonFile_Name).c_str());
	
	ImGui::NewLine();

	ImGui::Text(" Material Path => [ %s ] ", Engine_Utils::ToString(tOverrideMtl.wstrMtl_JsonFile_Path).c_str());
	

	if (tOverrideMtl.vecUsingTextureInfo.empty())
	{
		ImGui::NewLine();
		ImGui::TextWrapped(" Using Texture Is Empty Check Json File ");
		ImGui::End();
		return S_OK;
	}

	
	ImGui::BeginChild("Using Texture Info", m_vTextureInfoTableSize , true);

	if (ImGui::BeginTable("TextureInfoTable##Overrdie", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Slot Type##Overrdie", ImGuiTableColumnFlags_WidthFixed, 45.0f);
		ImGui::TableSetupColumn("Texture Name##Overrdie", ImGuiTableColumnFlags_WidthStretch);

		for (_uint i = 0; i < ENUM_TO_UINT(tOverrideMtl.vecUsingTextureInfo.size()); ++i)
		{
			ImGui::PushID(i);
			string strMtlSLotName  = Engine_Utils::ToString(tOverrideMtl.vecUsingTextureInfo[i].first);
			string strUsingTexName = path(tOverrideMtl.vecUsingTextureInfo[i].second).filename().stem().string();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			bool bSelected = (m_iSelectOverrideMtlTextureID == i);
			if (ImGui::Selectable(strMtlSLotName.c_str(), bSelected, ImGuiSelectableFlags_SpanAllColumns))
			{
				if (!strUsingTexName.empty())
					m_iSelectOverrideMtlTextureID = i;
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(strUsingTexName.c_str());

			ImGui::PopID();
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();


	if(m_iSelectOverrideMtlTextureID >= tOverrideMtl.vecUsingTextureInfo.size())
		m_iSelectOverrideMtlTextureID = 0;

	CTextureBase::RESOURCE_BASE_DESC tDesc{};
	tDesc.wstrPath = tOverrideMtl.vecUsingTextureInfo[m_iSelectOverrideMtlTextureID].second;
	tDesc.wstrName = path(tOverrideMtl.vecUsingTextureInfo[m_iSelectOverrideMtlTextureID].second).filename().stem();
	CTextureBase* pTex = m_pGameInstance->GetOrAddTexture(tDesc.wstrName, &tDesc);
	if (pTex)
	{
		ID3D11ShaderResourceView* pSRV = pTex->Get_SRV();
		ImGui::Image((ImTextureID)pSRV, ImVec2(200, 200));
		Safe_Release(pTex);
	}

	ImGui::End();
	return S_OK;
}


HRESULT CPanel_MapObjectList::Render_SelectOriginMaterialInfo()
{
	ImGui::Begin(" Origin Material Info ");

	if ( !m_pSelectMapObject || m_isShowOriginMtlInfo == false)
	{
		ImGui::Text(" Origin Material Is Empty");
		ImGui::End();
		return S_OK;
	}

	ImGui::SeparatorText(" Origin Material Info ");

	ImGui::NewLine();

	ImGui::Text(" Material Name => [ %s ] ", m_strOriginMtlName.c_str() );

	ImGui::Text(" Material Path => [ %s ] ", m_strOriginMtlPath.c_str());


	ImGui::BeginChild("Using Texture Info", m_vTextureInfoTableSize , true);

	if (ImGui::BeginTable("TextureInfoTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Slot Type", ImGuiTableColumnFlags_WidthFixed ,45.0f);
		ImGui::TableSetupColumn("Texture Name", ImGuiTableColumnFlags_WidthStretch);

		for (_uint i = 0; i < ENUM_TO_UINT(m_arrayOriginMtlUsingTexturesName.size()); ++i)
		{
			ImGui::PushID(i);
			string TextureTypeName = Get_MaterialSlotNameAssimp(aiTextureType(i + 1));
			if (TextureTypeName.empty()) { ImGui::PopID(); continue; }
			if (m_arrayOriginMtlUsingTexturesName[i].empty()) { ImGui::PopID(); continue; }

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			bool bSelected = (m_iSelectOriginMtlTexture == i);

			if (ImGui::Selectable(TextureTypeName.c_str(), bSelected, ImGuiSelectableFlags_SpanAllColumns))
			{
				if(!m_arrayOriginMtlUsingTexturesName[i].empty())
					m_iSelectOriginMtlTexture = i;
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(m_arrayOriginMtlUsingTexturesName[i].c_str());

			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	ImGui::EndChild();


	CTextureBase::RESOURCE_BASE_DESC tDesc{};
	if (m_iSelectOriginMtlTexture > m_arrayOriginMtlUsingTexturesName.size())
		m_iSelectOriginMtlTexture = static_cast<_uint>(m_arrayOriginMtlUsingTexturesName.size()) - 1;

	tDesc.wstrName = Engine_Utils::ToWString(m_arrayOriginMtlUsingTexturesName[m_iSelectOriginMtlTexture]);
	CTextureBase* pTex = m_pGameInstance->GetOrAddTexture(tDesc.wstrName, &tDesc);
	if (pTex)
	{
		ID3D11ShaderResourceView* pSRV = pTex->Get_SRV();
		ImGui::Image((ImTextureID)pSRV, ImVec2(200, 200));
		Safe_Release(pTex);
	}


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


	Safe_Release(m_pTransformLayout);
	Safe_Release(m_pGameInstance);
}
