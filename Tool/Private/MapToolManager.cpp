#include "pch.h"
#include "MapToolManager.h"
#include "MapObject.h"
#include <fstream>
#include "ImGui_ToolManager.h"
#include "Picking_ToolManager.h"
#include "DebugLine.h"
#include "Model.h"
#include "Level_Map.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Shader.h"
#include "LevelData.h"

IMPLEMENT_SINGLETON(CMapToolManager)

CMapToolManager::CMapToolManager()
	: m_pGameInstance					{ CGameInstance::GetInstance() }
	, m_pImGui_ToolManager				{ CImGui_ToolManager::GetInstance() }
	, m_pPreviewMapobject				{ nullptr }
	, m_pMesh_Shader					{nullptr}
	, m_tTextureSplattingInfo			{}
	, m_arrayMapObjectCloneFactory		{}
	, m_umapMapTextures					{}
	, m_pDefaultBlackSRV				{nullptr}
	, m_pDefaultWhiteSRV				{nullptr}
	, m_mapTextureSplatingInfoDatas		{}
	, m_pLevelData						{nullptr}
	, m_pCamCinematicSequenceRenderModel{nullptr}
	, m_pCamCinematicSequenceRenderShader{nullptr}
{
	Safe_AddRef(m_pGameInstance);
	m_arrayMapObjectCloneFactory.fill(nullptr);
}

EClientMakePath CMapToolManager::Get_ClientMakePath_ByFilePath(const wstring& wstrFilePullPath)
{
	if (wstrFilePullPath.empty())
		return m_eMakeMapObjectClientMakePath;

	if (wstrFilePullPath.find(L"Bush") != std::wstring::npos)
		return EClientMakePath::Bush;
	if (wstrFilePullPath.find(L"Grass") != std::wstring::npos)
		return EClientMakePath::Grass;
	else if (wstrFilePullPath.find(L"Moss") != std::wstring::npos)
		return EClientMakePath::Moss;
	else if (wstrFilePullPath.find(L"Tree") != std::wstring::npos)
		return EClientMakePath::Tree;
	else if (wstrFilePullPath.find(L"Vine") != std::wstring::npos)
		return EClientMakePath::Vine;
	else if (wstrFilePullPath.find(L"Rock") != std::wstring::npos)
		return EClientMakePath::Rock;
	else if (wstrFilePullPath.find(L"Water") != std::wstring::npos)
		return EClientMakePath::Water;
	else if (wstrFilePullPath.find(L"Land") != std::wstring::npos)
		return EClientMakePath::LandScape;

	return m_eMakeMapObjectClientMakePath;
}

HRESULT CMapToolManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (!m_pDevice || !pContext)
	{
		m_pDevice = pDevice;
		m_pContext = pContext;

		Safe_AddRef(m_pDevice);
		Safe_AddRef(m_pContext);
	}

	m_fMouseWheelSpeed = 0.001f;
	m_fMouseRange = 1.f;


	m_pMesh_Shader = 
		static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_Tool"));


	if (FAILED(Register_MapObjectCloneFactory()))
		return E_FAIL;

	if (FAILED(Make_DefaultTexture()))
		return E_FAIL;

	if (FAILED(Bind_MapTexture()))
		return E_FAIL;

	if (FAILED(Reset_Camera_Cinematic_Sequence()))
		return E_FAIL;

	if (FAILED(Update_Camera_Cinematic_Sequence_Names()))
		return E_FAIL;

	return S_OK;
}


HRESULT CMapToolManager::Bind_MapTexture()
{
	/* SRV ¹ÙÀÎµù */
	if (m_pMesh_Shader == nullptr) return E_FAIL;

	/* Base , RGB , RGBA ÅØ½ºÃ³¸¦ ¹ÙÀÎµùÇØÁØ´Ù */
	ID3DX11EffectShaderResourceVariable* pEffectSRV{ nullptr };

	/* Base Texture */
	pEffectSRV = m_pMesh_Shader->Get_Variable(g_szBase_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Base_Texture is Can't Find ");
		return E_FAIL;
	}
	pEffectSRV->SetResource(m_tTextureSplattingInfo.pBase_Texture == nullptr ? m_pDefaultBlackSRV : m_tTextureSplattingInfo.pBase_Texture->Get_SRV());

	/* DH Tile Texture  */
	pEffectSRV = m_pMesh_Shader->Get_Variable(g_szMix_DH_Tile_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Mix_DH_Tile_Texture is Can't Find ");
		return E_FAIL;
	}
	pEffectSRV->SetResource(m_tTextureSplattingInfo.pMix_DH_Tile_Texture == nullptr ? m_pDefaultBlackSRV : m_tTextureSplattingInfo.pMix_DH_Tile_Texture->Get_SRV());


	/* NBR Tile Texture */
	pEffectSRV = m_pMesh_Shader->Get_Variable(g_szMix_NBR_Tile_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Mix_NBR_Tile_Texture is Can't Find ");
		return E_FAIL;
	}
	pEffectSRV->SetResource(m_tTextureSplattingInfo.pMix_NBR_Tile_Texture == nullptr ? m_pDefaultBlackSRV : m_tTextureSplattingInfo.pMix_NBR_Tile_Texture->Get_SRV());

	return S_OK;
}

HRESULT CMapToolManager::Bind_Mix_RGBA_Info()
{
	if (FAILED(Bind_Mix_RGBA_Texture()))
		return E_FAIL;

	if (FAILED(Bind_Mix_RGBA_Data_And_Count()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapToolManager::Bind_Mix_RGBA_Texture()
{
	/* Texture ¸ÕÀú Binding */
	ID3DX11EffectShaderResourceVariable* pEffectSRV{ nullptr };

	/* Effect SRVs */
	pEffectSRV = m_pMesh_Shader->Get_Variable(g_szMix_RGBA_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Mix_RGBA_Texture is Can't Find ");
		return E_FAIL;
	}

	/* SRV¸¦ ¸ð¾Æ±â */
	array<ID3D11ShaderResourceView*, MAX_RGBA_TEXTURE_COUNT> arraySRVs{};
	arraySRVs.fill(nullptr);

	for (_int i = 0; i < m_tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count ; ++i)
		arraySRVs[i] = m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i] == nullptr ? m_pDefaultBlackSRV
		: m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i]->Get_SRV();

	/* ¸ðÀº SRV ´øÁ®ÁÖ±â */
	if (FAILED(pEffectSRV->SetResourceArray(arraySRVs.data(), 0, MAX_RGBA_TEXTURE_COUNT)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapToolManager::Bind_Mix_RGBA_Data_And_Count()
{
	CB_MIX_RGBA_INFO	tCB{};

	memcpy(tCB.g_MIX_RGBA_DATA , m_tTextureSplattingInfo.tMix_RGBA_Info.vecMix_RGBA_Data.data() , sizeof(MIX_RGBA_DATA) * m_tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count );
	tCB.g_iUse_Mix_RGBA_Count = m_tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count;

	ID3DX11EffectConstantBuffer* pCB = m_pMesh_Shader->Get_ConstantBuffer("CB_MIX_RGBA_INFO");
	if (!pCB->IsValid())
	{
		MSG_BOX("CB_MIX_RGBA_INFO ¹ÙÀÎµù ½ÇÆÐ ¹®ÀÚ¿­ °Ë»ö È®ÀÎ");
		return E_FAIL;
	}

	if(FAILED(pCB->SetRawValue( &tCB , 0 , sizeof(CB_MIX_RGBA_INFO))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapToolManager::Register_MapTexture()
{
	CTextureBase::RESOURCE_BASE_DESC tTexDesc{};

	for (auto& Path : std::filesystem::recursive_directory_iterator(g_wszMapTexture_Path))
	{
		if (!std::filesystem::is_regular_file(Path))
			continue;

		wstring wstrExt = path(Path).extension();

		if (wstrExt != L".png" && wstrExt != L".dds")
			continue;
		path FullPath = path(Path);
		tTexDesc.wstrPath = path(FullPath);
		tTexDesc.wstrName = path(FullPath).filename().stem();

		/* ÅØ½ºÃ³µéÀ» ¾þ¾î¿Â´Ù... */
		CTextureBase* pTexBase = m_pGameInstance->GetOrAddTexture(L"Texture_" + tTexDesc.wstrName, &tTexDesc);
		if (pTexBase == nullptr)
			return E_FAIL;

		/* ±¸¿ªÀÌ¸§À¸·Î ³ª´²ÁÖ±â */
		FullPath._Remove_filename_and_separator();
		wstring wstrFloderName = FullPath.filename();
		m_umapMapTextures[wstrFloderName].push_back(pTexBase);
	}

	return S_OK;
}

HRESULT CMapToolManager::Release_SplatingTextureData()
{
	for (auto& Pair : m_mapTextureSplatingInfoDatas)
		Pair.second.Free();
	m_mapTextureSplatingInfoDatas.clear();

	return S_OK;
}

HRESULT CMapToolManager::Delete_TextureSplatingInfoData(const wstring& wstrDeleteName)
{
	if (m_mapTextureSplatingInfoDatas.find(wstrDeleteName) == m_mapTextureSplatingInfoDatas.end())
		return E_FAIL;


	/* Delete ÇÏ±â Delete ÇÏ±âÀü ¾ÈÀüÇÏ°Ô Free() È£­ŒÇØÁÖ±â */
	m_mapTextureSplatingInfoDatas.erase(wstrDeleteName);


	return S_OK;
}

HRESULT CMapToolManager::Load_TextureSplatingInfoData()
{
	/* Texture SplatingÀ» ÀúÀå½ÃÅ² DataµéÀ» LoadÇØÁØ´Ù */

	std::ifstream ifs(TextureSplatingInfoDataPath);
	
	if (ifs.is_open() == false) return E_FAIL;

	//if (ifs.peek() == std::ifstream::traits_type::eof())
	//{
	//	return S_OK; // ÅÖ ºñ¾îÀÖÀ¸´Ï ·ÎµåÇÒ °Íµµ ¾ø´Ù! ¾ÈÀüÇÏ°Ô ¸®ÅÏ.
	//}

	nlohmann::json LoadJson{};
	try
	{
		ifs >> LoadJson;
	}
	catch (nlohmann::json::parse_error& e)
	{
		// JSON ¹®¹ýÀÌ Æ²·È°Å³ª ÀÎÄÚµù ¹®Á¦ÀÏ ¶§
		OutputDebugStringA(e.what());
		OutputDebugStringA("\nJSON ÆÄ½Ì ¿¡·¯ ¹ß»ý!\n");
		return E_FAIL;
	}



	if (LoadJson.empty())
		return S_OK;

	/* ÀúÀåÇÒ Key °ªÀ»ÅëÇØ ÀúÀå½ÃÄÑÁØ´Ù */

	m_mapTextureSplatingInfoDatas.clear();


	for (const auto& item : LoadJson.items())
	{
		TEXTURE_SPLATTING_INFO tInfo{};

		wstring wstrKey = Engine_Utils::ToWString(item.key());
		tInfo.Load_Json(item.value());
		m_mapTextureSplatingInfoDatas.emplace(wstrKey , tInfo);
	}

	return S_OK;
}

HRESULT CMapToolManager::Load_TextureSplatingInfoData(const wstring& wstrLoadName)
{
	if (m_mapTextureSplatingInfoDatas.empty()) return S_OK;


	if (m_mapTextureSplatingInfoDatas.find(wstrLoadName) == m_mapTextureSplatingInfoDatas.end())
		return E_FAIL;



	/* ÇöÀç Àû¿ëµÇ°íÀÖ´Â Info¸¦ »èÁ¦ */
	m_tTextureSplattingInfo.Free();

	/* ´ëÀÔ º¹»ç»ý¼º */
	m_tTextureSplattingInfo = m_mapTextureSplatingInfoDatas.at(wstrLoadName);

	/* Data¿¡¼­ LoadÇØ¿Â µ¥ÀÌÅÍ¿¡¼­ DH , NBRÀ» Slice·Î Àß¶óÁØ´Ù */
	Slice_DH_Texture();
	Slice_NBR_Texture();

	/* Load ÀüºÎ ÇØÁÖ°í Binding ÇÔ¼ö ºÎ¸£±â */

	if (FAILED(Bind_MapTexture()))
		return E_FAIL;
	if (FAILED(Bind_Mix_RGBA_Info()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapToolManager::Save_TextureSplatingInfoData()
{
	/* Json ÆÄÀÏ Save */
	std::ofstream ofs(TextureSplatingInfoDataPath);

	if (ofs.is_open() == false) return E_FAIL;

	nlohmann::json SaveJson{};


	for (auto& Pair : m_mapTextureSplatingInfoDatas)
	{
		string strKey = Engine_Utils::ToString(Pair.first);
		Pair.second.Save_Json(SaveJson[strKey]);
	}

	ofs << SaveJson.dump(4);
	
	ofs.close();

	return S_OK;
}

HRESULT CMapToolManager::Save_TextureSplatingInfoData(const wstring& wstrSaveName)
{
	_bool isExist = m_mapTextureSplatingInfoDatas.find(wstrSaveName) != m_mapTextureSplatingInfoDatas.end();


	if (isExist)
	{
		int iResult = MessageBox(NULL, L"ÀÌ¹Ì ÀúÀåµÈ ÆÄÀÏÀÌ Á¸ÀçÇÕ´Ï´Ù µ¤¾î¾²½Ã°Ú½À´Ï±î?", L"°æ°í: ±âÁ¸ ÀúÀåÆÄÀÏ »ç¶óÁü", MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND);
		if (iResult)
		{
			m_mapTextureSplatingInfoDatas[wstrSaveName].Free();
		}
		else
			return S_OK;
	}


	/* ÇöÀç ÀúÀåµÈ Texture Splating Info ¸¦ ÀúÀå½ÃÄÑÁØ´Ù */
	m_mapTextureSplatingInfoDatas[wstrSaveName] = m_tTextureSplattingInfo;

	return S_OK;
}

HRESULT CMapToolManager::UnRegister_MapTexture()
{
	m_tTextureSplattingInfo.Free();

	for (auto& Pair : m_umapMapTextures)
	{
		for (auto& Tex : Pair.second)
			Safe_Release(Tex);
	}

	return S_OK;
}

HRESULT CMapToolManager::Register_MapObjectCloneFactory()
{
	// °Å ÀÌÆåÆ®¿¡¼­ Á» ²ÇÃÄ°¡°Ú½À´Ï´Ù.
		// ¾îÂ÷ÇÇ ¹«Á¶°Ç else °É¸±°Ì´Ï´Ù. 
		// Load ´Ü°è¿¡¼­´Â ¾îÂ÷ÇÇ Loader¶ó¼­ 0ÀÌ°Åµç
	if (m_pGameInstance->Get_CurrentLevelIndex() == ENUM_TO_UINT(ELevelType::EFFECT))
	{
		m_funcMapObjectCloneFactory =
			[=](void* pArg)->CGameObject* { return m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_MapObject",
				ENUM_TO_UINT(ELevelType::EFFECT), g_wszMapObjectLayer, pArg); };
	}

	else
	{
		m_funcMapObjectCloneFactory =
			[=](void* pArg)->CGameObject* { return m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_GameObject_MapObject",
				ENUM_TO_UINT(ELevelType::MAP), g_wszMapObjectLayer, pArg); };
	}

	return S_OK;
}

CModel* CMapToolManager::Get_MonsterPreviewModel(DTO::EMakeMonsterType eMakeMonsterType)
{
	CModel* pModel{ nullptr };

	wstring wstrDefualtPath = g_wszPreviewObejctModelPath;

	wstring wstrModelName{};

	switch (eMakeMonsterType)
	{
	case DTO::EMakeMonsterType::Dog:				
		wstrModelName = L"Preveiw_Monster_Dog";
		break;
	case DTO::EMakeMonsterType::Boomer:
		wstrModelName = L"Preveiw_Monster_Boomer";
		break;
	case DTO::EMakeMonsterType::Shooter:
		wstrModelName = L"Preveiw_Monster_Shooter";
		break;
	case DTO::EMakeMonsterType::Xibi:
		wstrModelName = L"Preveiw_Boss_Xibi";
		break;
	default:									return nullptr;
	}

	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	pModel =
		static_cast<CModel*>
		(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_" + wstrModelName, &tModelCopyDesc));

	return pModel;
}

CModel* CMapToolManager::Get_PlayerPreviewModel()
{
	CModel* pModel{ nullptr };

	wstring wstrPlayerModelName = L"Preveiw_Player";

	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	pModel =
		static_cast<CModel*>
		(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_" + wstrPlayerModelName, &tModelCopyDesc));

	return pModel;
}

CModel* CMapToolManager::Get_BatchObjectModel(DTO::EMakeObjectType eType)
{
	CModel* pModel{ nullptr };
	wstring wstrModelName{};

	switch (eType)
	{
	case DTO::EMakeObjectType::Battle_Field:	wstrModelName = L"Cube"; break;
	break;
	default:									return nullptr;
	}


	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	pModel =
		static_cast<CModel*>
		(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_" + wstrModelName, &tModelCopyDesc));

	return pModel;
}

HRESULT CMapToolManager::Ready_LevelData()
{
	m_pLevelData= CLevelData::Create(EToolObjectType::MAPOBJECT, m_pDevice, m_pContext);
	if (m_pLevelData == nullptr) return E_FAIL;

	return S_OK;
}

HRESULT CMapToolManager::Apply_LevelData(const DTO::TLevelData* tData)
{
	if (tData == nullptr) return E_FAIL;

	/* ´Ù¸¥ ÀÌ¸§ÀÌ ÀúÀåµÇ¾îÀÖ´Ù¸é E_FAIL ¹ÝÈ¯µÈ´Ù */

	/* NoneÀÌ ¾Æ´Ï¶ó¸é ¹ÝÈ¯ÇÑ´Ù */
	if(tData->strTextureSplatingInfoName != "None")
		if (FAILED(CMapToolManager::Load_TextureSplatingInfoData(Engine_Utils::ToWString(tData->strTextureSplatingInfoName))))
			return E_FAIL;

	/* None => [Don't Use Texture Splating Info] */
	m_pLevelData->m_strTextureSplatingInfoName	= tData->strTextureSplatingInfoName;

	/* Level Type */
	m_pLevelData->m_eClientLevelType			= StringToClientleveltype(tData->strLevelTypeName);


	return S_OK;
}

HRESULT CMapToolManager::Release_SceneData()
{
	Safe_Release(m_pLevelData);

	return S_OK;
}

HRESULT CMapToolManager::Bind_SplatingTextureInfo()
{
	/* Binding Texture */
	if (FAILED(Bind_MapTexture()))
		return E_FAIL;

	/* ÇöÀç »ç¿ëÇÏ´Â RGBA Map °³¼ö Binding */
	if (FAILED(Bind_Mix_RGBA_Info()))
		return E_FAIL;

	return S_OK;
}



void	CMapToolManager::Update(float DT)
{
	Mouse_Update(DT);
	
	Input_Update(DT);

	Preview_Update(DT);
}

void CMapToolManager::Input_Update(float DT)
{
	/* ¸¶¿ì½º°¡ ImGUi Ã¢ À§¿¡ ÀÖ°Å³ª Á¶ÀÛÁßÀÏ‹š´Â ÀÔ·Â¹æÁö */
	ImGuiIO& io = ImGui::GetIO();
	//if (io.WantCaptureMouse)
	//	return;

	/* ÈÙ */
	_long lMouseWheelDt = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::WHEEL);
	m_fMouseRange += (float)lMouseWheelDt * m_fMouseWheelSpeed;

	if (m_fMouseRange < 1.f)
		m_fMouseRange = 1.f;

	bool isMouseOn{ false };
	isMouseOn = m_eMapTooObjectBatchMode == EMapToolObjectBatchMode::Brush ? m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB) : m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::LB);
	
	if (isMouseOn && m_pPreviewMapobject && m_pImGui_ToolManager->Get_MousePosInViewPort())
	{
		bool isCanBatch = m_tBrushModeOption.isUsePlacementSpacing == false ? true : Vec3(m_vRayWorldPos - m_vLastPlacedPos).Length() > m_tBrushModeOption.fPlacementSpacing ? true : false;
		if (isCanBatch)
		{
			if (FAILED(Batch_Preview()))
			{
				MSG_BOX("Preview is Failed To Batch");
			}
		}
	}


	return;
}

void CMapToolManager::Mouse_Update(float DT)
{
	if (m_eMapToolEmplaceType == EMapTool_EmplaceType::Free)
	{
		m_pImGui_ToolManager->RayUpdate();

		Vec3 vRayPos = m_pGameInstance->Picking_Get_RayPos();
		Vec3 vRayDir = m_pGameInstance->Picking_Get_RayDir();
		vRayDir.Normalize();
		m_vRayWorldPos = vRayPos + vRayDir * m_fMouseRange;
	}
	/* PickingÀÌ¸é ÇÇÅ·µÈ À§Ä¡ ÆÄ¾Ç */
	else if(m_eMapToolEmplaceType == EMapTool_EmplaceType::Picking)
	{
		m_vRayWorldPos =  CPicking_ToolManager::GetInstance()->Get_PickingPos();
	}
}

void CMapToolManager::Preview_Update(float DT)
{
	if (!m_pPreviewMapobject) return;

	if (m_pPreviewMapobject->Get_MapObjectDrawType() == EMapObject_DrawType::Instance)
		m_pPreviewMapobject->Set_Position(m_vRayWorldPos , m_pPreviewMapobject->Get_InstanceCount() - 1 );
	else
		m_pPreviewMapobject->Set_Position(m_vRayWorldPos);

	return;
}

CLIENT_MAKEPATH_DESC_BASE* CMapToolManager::Make_Client_MakePathDesc(EClientMakePath eClientMakePath ,CLIENT_MAKEPATH_DESC_BASE* pPrototype)
{
	return Engine::Create_ClientMakePathDesc(static_cast<DTO::EClientMakePath>(eClientMakePath), pPrototype);
}

_bool CMapToolManager::IsExist_ClientMakePathDesc(EClientMakePath eClientMakePath)
{
	return Engine::IsExist_ClientMakePathDesc(static_cast<DTO::EClientMakePath>(eClientMakePath));
}

HRESULT CMapToolManager::Change_Instance_To_OtherDrawType(CMapObject* pChangeMapObject, EMapObject_DrawType eChangeType)
{
	/* ÀÌ¹Ì ÇÑ¹ø ¸ðµ¨ÀÌ »ý¼ºµÈ °´Ã¼ */

	if (pChangeMapObject == nullptr) return E_FAIL;

	if (eChangeType == EMapObject_DrawType::Default)
	{
		CMapObject::MAPOBJECT_DESC tDesc{};

		tDesc.eClientLevelType = pChangeMapObject->Get_ClientLevelType();
		tDesc.eClientMakePath = pChangeMapObject->Get_ClientMakePath();
		tDesc.eMapObjectDrawType = eChangeType;
		tDesc.eState = pChangeMapObject->Get_MapObjectState();
		tDesc.isUELoaded = pChangeMapObject->Get_IsUELoaded();
		tDesc.isLoaded = pChangeMapObject->Get_IsLoaded();
		tDesc.wstrLayerTag = g_wszMapObjectLayer;
		tDesc.tUsingModelInfo.wstrPath = pChangeMapObject->Get_ModelPath();
		tDesc.tUsingModelInfo.wstrName = Engine_Utils::ToWString(pChangeMapObject->Get_ModelFileName());

		vector<SRT_DATA> vecSRTData = pChangeMapObject->Get_SRTDatas();
		vector<CLIENT_MAKEPATH_DESC_BASE*> vecClientMakePathDesc = pChangeMapObject->Get_ClientMakePathDescs();

		CGameObject* pResult{ nullptr };

		_uint iCount = ENUM_TO_UINT(vecSRTData.size());

		for (_uint i = 0; i < iCount; ++i)
		{
			/* SRT ÇÏ³ª¸¸ Push ÇÏ°í ÇÏ³ª¾¿ »ý¼º */
			tDesc.vecSRTs.clear();
			tDesc.vecClientMakePathDesc.clear();

			tDesc.vecSRTs.push_back(vecSRTData[i]);

			CLIENT_MAKEPATH_DESC_BASE* pClientMakePathDesc{ nullptr };
			if (!vecClientMakePathDesc.empty())
			{
				/* Æ÷ÀÎÅÍ·Î ³Ö¾îÁÖ±äÇÏÁö¸¸ Clone ÇÒ ¶§ ³»ºÎÀûÀ¸·Î New º¹»ç»ý¼º È£ÃâÇØ¼­ º¹»çÇØ°£´Ù */
				tDesc.vecClientMakePathDesc.push_back(vecClientMakePathDesc[i]);

			}

			pResult = Make_MapObject(&tDesc);

			if (pResult == nullptr)
			{
				/* ½ÇÆÐÇßÀ»½Ã ´©¼öÃ³¸® */
				Safe_Delete(pClientMakePathDesc);
				return E_FAIL;
			}
		}
		pChangeMapObject->Set_Dead();
	}
	return S_OK;
}

HRESULT CMapToolManager::Change_Default_To_OtherDrawType(CMapObject* pChangeMapObject, EMapObject_DrawType eChangeType)
{
	if (pChangeMapObject == nullptr) return E_FAIL;

	if (eChangeType == EMapObject_DrawType::Instance)
	{

	}
	return S_OK;

}

void CMapToolManager::Delete_Preview()
{
	if (m_pPreviewMapobject)
	{
		//if (m_pPreviewMapobject->Get_MapObjectState() == CMapObject::EState::Select)
		//{
		//	m_pPreviewMapobject = nullptr;
		//	return;
		//}

		EMapObject_DrawType eDrawType = m_pPreviewMapobject->Get_MapObjectDrawType();
		if (eDrawType == EMapObject_DrawType::Instance)
		{
			_int iIndex = m_pPreviewMapobject->Get_InstanceCount() - 1;
			if (iIndex == 0)
			{
				m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), m_pPreviewMapobject);
			}
			else
			{
				m_pPreviewMapobject->Delete_InstanceData(iIndex);
				m_pPreviewMapobject->Set_MapObjectState(CMapObject::EState::Default);
			}
			m_pPreviewMapobject = nullptr;
		}
		else
		{
			m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), m_pPreviewMapobject);
			m_pPreviewMapobject = nullptr;
		}
	}
}

void CMapToolManager::DrawImGui_Preview()
{
	if (!m_pPreviewMapobject) return;

	m_pPreviewMapobject->Draw_ImGui();
}

HRESULT CMapToolManager::Check_And_Bind_FromUE()
{
	list<CGameObject*>* pUEMapObject = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP) , g_wszMapObjectLayer);

	if (pUEMapObject == nullptr)  return S_OK;

	if (m_pLevelMap)
		m_pLevelMap->On_ChangeSelectedObject(nullptr);

	/* ¹Ì¸® ÇÑ°÷À¸·Î ÇÕÄ¡±â */
	vector<CMapObject*> vecMapObject{};
	/* Section º°·Î Instance¸¦ ÇÏ°í½ÍÀºµ¥ */
	//vecMapObject.reserve(iAllSize);
	//if(pStaticModelList) vecMapObject.insert(vecMapObject.end() , pStaticModelList->begin(), pStaticModelList->end());
	//if(pInstanceModelList) vecMapObject.insert(vecMapObject.end() , pInstanceModelList->begin(), pInstanceModelList->end());

	/* »ç¿ëÇÏ´Â ¸ðµ¨ÁÖ¼Ò°¡ °°Àº StaticModelÀ» ¸ð¾ÆµÑ Àå¼Ò */
	map<PairKey, vector<CMapObject*> > mapSameModels{};

	for (auto& MapObject : vecMapObject)
	{
	//	PairKey Key{};
	//	if (!MapObject)
	//		continue;
	//	CMapObject* pMapObject = static_cast<CMapObject*>(MapObject);
	//	CModel* pModel = pMapObject->Get_Component<CModel>();
	//	if (!pModel)
	//		continue;
	//	Key.first = pMapObject->Get_UsingModelInfo().wstrName;
	//	Key.second =  pMapObject->Get_TotalUseMtlsName();
	//	mapSameModels[Key].push_back(static_cast<CMapObject*>(pMapObject));
	//}

	//for (auto& Key : mapSameModels)
	//{
	//	for (auto& MapObject : Key.second)
	//	{
	//		if (!MapObject) continue;

	//		EMapObject_Type eMapObjectType = MapObject->Get_MapObjectType();
	//		if (eMapObjectType != EMapObject_Type::STATICMODEL && eMapObjectType != EMapObject_Type::INSTANCEMODEL) continue;

	//		vector<SRT_DATA> vecSRTData = MapObject->Get_SRTDatas();
	//		tInstanceModelDesc.tData.vecSRT.insert(tInstanceModelDesc.tData.vecSRT.end() , vecSRTData.begin() , vecSRTData.end());
	//	} 
	//	if (tInstanceModelDesc.tData.vecSRT.size() <= 1)
	//	{
	//		tInstanceModelDesc.tData.vecSRT.clear();
	//		continue;
	//	}
	//	else
	//	{
	//		tInstanceModelDesc.tData.tUsingModelInfo = Key.second.front()->Get_UsingModelInfo();
	//		Make_MapObject(EMapObject_Type::INSTANCEMODEL, &tInstanceModelDesc);
	//		tInstanceModelDesc.tData.vecSRT.clear();
	//		for (auto& MapObject : Key.second)
	//			m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), MapObject->Get_MapObjectType() ==  EMapObject_Type::STATICMODEL ?
	//				g_wszStaticModelLayer : g_wszInstanceModelLayer, MapObject);
	//	}
	}

	/* ±×¸®°í ÇÑ°³Â¥¸®ÀÎ */



	return S_OK;
}


HRESULT CMapToolManager::Make_DefaultTexture()
{
	// °øÅë ÅØ½ºÃ³ ¼³Á¤ (1x1 ÇÈ¼¿, 32ºñÆ® RGBA)
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

	{
		uint32_t pixelBlack = 0xFF000000; // ARGB = 0, 0, 0, 0
		D3D11_SUBRESOURCE_DATA tData = {};
		tData.pSysMem = &pixelBlack;
		tData.SysMemPitch = sizeof(uint32_t);

		ID3D11Texture2D* pTexBlack = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexBlack)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexBlack, nullptr, &m_pDefaultBlackSRV)))
		{
			pTexBlack->Release();
			return E_FAIL;
		}
		Safe_Release(pTexBlack);
	}
	{
		uint32_t pixelWhite = 0xFFFFFFFF; // ARGB = 255, 255, 255, 255
		D3D11_SUBRESOURCE_DATA tData = {};
		tData.pSysMem = &pixelWhite;
		tData.SysMemPitch = sizeof(uint32_t);

		ID3D11Texture2D* pTexWhite = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexWhite)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexWhite, nullptr, &m_pDefaultWhiteSRV)))
		{
			pTexWhite->Release();
			return E_FAIL;
		}
		Safe_Release(pTexWhite);
	}

	return S_OK;
}

/* 2d Texture Array¸¦ SRV·Î ¹ÝÈ¯ÇØ¼­ ³»¹ñ¾îÁÖ´Â ÇÔ¼ö  */
HRESULT CMapToolManager::Slice_DH_Texture()
{
	if (m_tTextureSplattingInfo.pMix_DH_Tile_Texture == nullptr) return S_OK;

	for (auto& pSRV : m_tTextureSplattingInfo.vecDHTextureArraySlices)
		Safe_Release(pSRV);

	m_tTextureSplattingInfo.vecDHTextureArraySlices.clear();

	ID3D11ShaderResourceView* pOriginalSRV = m_tTextureSplattingInfo.pMix_DH_Tile_Texture->Get_SRV();
	ID3D11Resource* pRes = nullptr;
	pOriginalSRV->GetResource(&pRes);

	ID3D11Texture2D* pTex2D = (ID3D11Texture2D*)pRes;
	D3D11_TEXTURE2D_DESC tTexDesc;
	pTex2D->GetDesc(&tTexDesc);

	m_tTextureSplattingInfo.vecDHTextureArraySlices.resize(tTexDesc.ArraySize);
	for (UINT i = 0; i < tTexDesc.ArraySize; ++i)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC tViewDesc;
		ZeroMemory(&tViewDesc, sizeof(tViewDesc));

		tViewDesc.Format = tTexDesc.Format;
		tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		tViewDesc.Texture2DArray.MostDetailedMip = 0;
		tViewDesc.Texture2DArray.MipLevels = 1;
		tViewDesc.Texture2DArray.FirstArraySlice = i;
		tViewDesc.Texture2DArray.ArraySize = 1;

		ID3D11ShaderResourceView* pSliceSRV = nullptr;
		if (FAILED(m_pDevice->CreateShaderResourceView(pTex2D, &tViewDesc, &pSliceSRV)))
		{
			Safe_Release(pRes);
			return E_FAIL;
		}

		m_tTextureSplattingInfo.vecDHTextureArraySlices[i] = pSliceSRV;
	}
	Safe_Release(pRes);


	return S_OK;
}

HRESULT CMapToolManager::Slice_NBR_Texture()
{

	if (m_tTextureSplattingInfo.pMix_NBR_Tile_Texture == nullptr) return S_OK;

	for (auto& pSRV : m_tTextureSplattingInfo.vecNBRTextureArraySlices)
		Safe_Release(pSRV);

	m_tTextureSplattingInfo.vecNBRTextureArraySlices.clear();


	ID3D11ShaderResourceView* pOriginalSRV = m_tTextureSplattingInfo.pMix_NBR_Tile_Texture->Get_SRV();
	ID3D11Resource* pRes = nullptr;
	pOriginalSRV->GetResource(&pRes);

	ID3D11Texture2D* pTex2D = (ID3D11Texture2D*)pRes;
	D3D11_TEXTURE2D_DESC tTexDesc;
	pTex2D->GetDesc(&tTexDesc);

	m_tTextureSplattingInfo.vecNBRTextureArraySlices.resize(tTexDesc.ArraySize);
	for (UINT i = 0; i < tTexDesc.ArraySize; ++i)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC tViewDesc;
		ZeroMemory(&tViewDesc, sizeof(tViewDesc));

		tViewDesc.Format = tTexDesc.Format;
		tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		tViewDesc.Texture2DArray.MostDetailedMip = 0;
		tViewDesc.Texture2DArray.MipLevels = 1;
		tViewDesc.Texture2DArray.FirstArraySlice = i;
		tViewDesc.Texture2DArray.ArraySize = 1;

		ID3D11ShaderResourceView* pSliceSRV = nullptr;
		if (FAILED(m_pDevice->CreateShaderResourceView(pTex2D, &tViewDesc, &pSliceSRV)))
		{
			Safe_Release(pRes);
			return E_FAIL;
		}

		m_tTextureSplattingInfo.vecNBRTextureArraySlices[i] = pSliceSRV;
	}
	Safe_Release(pRes);


	return S_OK;
}

void CMapToolManager::Get_SRT_BrushData(Vec3& vOutScale, Quat& vOutQuat, Vec3& vOutPosition)
{
	vOutPosition = m_vRayWorldPos;

	if (m_tBrushModeOption.isUseBrushScale)
		vOutScale = m_tBrushModeOption.vBrushScale;
	if (m_tBrushModeOption.isUseRandomScale)
	{
		vOutScale.x *= m_pGameInstance->Rand_Float(m_tBrushModeOption.vMinMaxScaleX.x, m_tBrushModeOption.vMinMaxScaleX.y);
		vOutScale.y *= m_pGameInstance->Rand_Float(m_tBrushModeOption.vMinMaxScaleY.x, m_tBrushModeOption.vMinMaxScaleY.y);
		vOutScale.z *= m_pGameInstance->Rand_Float(m_tBrushModeOption.vMinMaxScaleZ.x, m_tBrushModeOption.vMinMaxScaleZ.y);
	}

	if (m_tBrushModeOption.isUseBrushRotation)
	{
		Vec3 vRotRadian = m_tBrushModeOption.vBrushRotation * TO_RAD;
		vOutQuat =  Quat::CreateFromYawPitchRoll(vRotRadian.y , vRotRadian.x , vRotRadian.z);
	}

	if (m_tBrushModeOption.isUseRandomYRotation)
	{
		float RotY = m_pGameInstance->Rand_Float(m_tBrushModeOption.vRandomMinMaxRotaionRange.x  , m_tBrushModeOption.vRandomMinMaxRotaionRange.y);
		float fRandYRad = RotY * TO_RAD;
		Quat vRandomRot = Quat::CreateFromYawPitchRoll(fRandYRad, 0.f, 0.f);
		vOutQuat = vOutQuat * vRandomRot;
	}

	return;
}

void CMapToolManager::Set_BrushRotation(const Quat& vQuat)
{
	Vec3 vRotation = vQuat.ToEuler()* To_DEGREE;
	m_tBrushModeOption.vBrushRotation = vRotation;
	return;
}


const Vec3& CMapToolManager::Get_MousePickingPos() const
{
	return m_vRayWorldPos;
}

HRESULT CMapToolManager::Render()
{
	if (m_pCamCinematicSequence)
		if (FAILED(m_pCamCinematicSequence->Render_Debug(ENUM_TO_UINT(EMapObjectShaderPass::StaticObject), m_pCamCinematicSequenceRenderModel, m_pCamCinematicSequenceRenderShader)))
			return E_FAIL;

	return S_OK;
}

HRESULT CMapToolManager::Export_SaveSceneData(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
	if (m_pGameInstance->Get_CurrentLevelIndex() != ENUM_TO_UINT(ELevelType::MAP)) return E_FAIL;
	if (m_pLevelData == nullptr) return E_FAIL;

	m_pLevelData->Export_Data(eCategory , pDocument);

	return S_OK;
}

HRESULT CMapToolManager::Load_Camera_Cinematic_Sequence(const wstring& wstrFindKey)
{
	m_pGameInstance->GameDataManager_Load_CameraCinematicSequence(wstrFindKey , m_pCamCinematicSequence);
	return S_OK;
}

HRESULT CMapToolManager::Save_Camera_Cinematic_Sequence(const wstring& wstrSaveKey)
{
	m_pGameInstance->GameDataManager_Save_CameraCinematicSequence(wstrSaveKey, m_pCamCinematicSequence);

	Update_Camera_Cinematic_Sequence_Names();
	return S_OK;
}

HRESULT CMapToolManager::Reset_Camera_Cinematic_Sequence()
{
	/* ÀüÃ¼¸®¼Â */
	if (!m_pCamCinematicSequence)
	{
		m_pCamCinematicSequence = new Camera_Cinematic_Sequence(m_pDevice, m_pContext);
		return S_OK;
	}

	m_pCamCinematicSequence->Reset();

	return S_OK;
}

HRESULT CMapToolManager::Update_Camera_Cinematic_Sequence_Names()
{
	m_vecCamCinematicSequenceNames.clear();

	m_vecCamCinematicSequenceNames = m_pGameInstance->GameDataManager_Get_CameraCinematicSequenceNames();
	
	return S_OK;
}

HRESULT CMapToolManager::Ready_CinematicSequenceDebugRender()
{
	Safe_Release(m_pCamCinematicSequenceRenderModel);
	Safe_Release(m_pCamCinematicSequenceRenderShader);



	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	m_pCamCinematicSequenceRenderModel =
		static_cast<CModel*>
		(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::MAP), L"Prototype_Component_Model_DebugCamera",&tModelCopyDesc));

	CShader::SHADER_ORIGIN_DESC tDesc{};
	m_pCamCinematicSequenceRenderShader =
		static_cast<CShader*>
		(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_Tool", &tDesc));

	if (m_pCamCinematicSequenceRenderModel == nullptr) return E_FAIL;
	if (m_pCamCinematicSequenceRenderShader == nullptr) return E_FAIL;

	return S_OK;
}

CMapObject* CMapToolManager::Make_MapObject(void* pArg, _bool isPreview)
{
	if ( m_funcMapObjectCloneFactory == nullptr) return nullptr;

	CGameObject* pCreatObject{ nullptr };

	if (!(pCreatObject = m_funcMapObjectCloneFactory(pArg)))
	{
		Safe_Release(pCreatObject);
		return nullptr;
	}

	if (isPreview)
		Delete_Preview();

	return  isPreview == true ? m_pPreviewMapobject = static_cast<CMapObject*>(pCreatObject) : static_cast<CMapObject*>(pCreatObject);
}


HRESULT CMapToolManager::Batch_Preview()
{
	EMapObject_DrawType ePreviewDrawType	= m_pPreviewMapobject->Get_MapObjectDrawType();
	CMapObject::EState  ePreviewState		= m_pPreviewMapobject->Get_MapObjectState();

	if( ePreviewState == CMapObject::EState::Preview)
		m_pPreviewMapobject->Set_MapObjectState(CMapObject::EState::Default);

	/* ¹èÄ¡¸ðµå°¡ ½Ì±ÛÀÌ¸é ÇÑ¹ø»ý¼ºÇÏ°í ³¡  */
	if (m_eMapTooObjectBatchMode == EMapToolObjectBatchMode::Single)
	{
		_int iIndex = m_pPreviewMapobject->Get_InstanceCount() - 1;
		m_pPreviewMapobject->Set_Position(m_vRayWorldPos, iIndex);
		m_pPreviewMapobject->Override_OriginTransform(iIndex);
		m_pPreviewMapobject = nullptr;
		
		m_vLastPlacedPos = m_vRayWorldPos;
	}
	else
	{

		_uint iCount = m_pPreviewMapobject->Get_InstanceCount();
		_uint iBatchIndex = iCount - 1;

		if (m_tBrushModeOption.isUseGroupCount)
		{
			if (iCount == m_tBrushModeOption.iMaxGroupMaxCount +1)
			{
				MSG_BOX(" Brush·Î ±×¸± ¼ö ÀÖ´Â ±×·ì ÃÖ´ë ÀÎ½ºÅÏ»ý °³¼ö¿¡ µµ´ÞÇß½À´Ï´Ù »õ·Î »ý¼º OR ÀÎ½ºÅÏ½Ì ±×·ì Max Count¸¦ Á¶ÀýÇØÁÖ¼¼¿ä ");
				return S_OK;
			}
		}


		SRT_DATA tSRT{};
		Get_SRT_BrushData(tSRT.vScale , tSRT.vQuat , tSRT.vPosition);

		m_pPreviewMapobject = CMapObject::Clone(m_pPreviewMapobject, tSRT);

		m_vLastPlacedPos = tSRT.vPosition;

		if (m_pPreviewMapobject == nullptr) return E_FAIL;

		m_vLastPlacedPos = tSRT.vPosition;

	}

	return S_OK;
}


void CMapToolManager::Free()
{
	Super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);


	Safe_Release(m_pDefaultWhiteSRV);
	Safe_Release(m_pDefaultBlackSRV);

	Safe_Release(m_pMesh_Shader);


	/* ¾È¿¡ µé¾îÀÖ´Â Texture Á¤¸® */
	m_tTextureSplattingInfo.Free();
	UnRegister_MapTexture();


	Safe_Delete(m_pCamCinematicSequence);

	Safe_Release(m_pCamCinematicSequenceRenderModel);
	Safe_Release(m_pCamCinematicSequenceRenderShader);

	m_pLevelMap = nullptr;


	for (auto& Factory : m_arrayMapObjectCloneFactory)
		Factory = nullptr;

}



