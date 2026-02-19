#include "pch.h"
#include "MapToolManager.h"
#include "MapObject.h"
#include "ImGui_ToolManager.h"
#include "Picking_ToolManager.h"
#include "DebugLine.h"
#include "Model.h"
#include "DataStruct_Map.h"
#include "Level_Map.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Shader.h"

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
{
	Safe_AddRef(m_pGameInstance);
	m_arrayMapObjectCloneFactory.fill(nullptr);
}

HRESULT CMapToolManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice  = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);


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


	return S_OK;
}


HRESULT CMapToolManager::Bind_MapTexture()
{
	/* SRV 바인딩 */
	if (m_pMesh_Shader == nullptr) return E_FAIL;

	/* Base , RGB , RGBA 텍스처를 바인딩해준다 */
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
	/* Texture 먼저 Binding */
	ID3DX11EffectShaderResourceVariable* pEffectSRV{ nullptr };

	/* Effect SRVs */
	pEffectSRV = m_pMesh_Shader->Get_Variable(g_szMix_RGBA_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Mix_RGBA_Texture is Can't Find ");
		return E_FAIL;
	}

	/* SRV를 모아기 */
	array<ID3D11ShaderResourceView*, MAX_RGBA_TEXTURE_COUNT> arraySRVs{};
	arraySRVs.fill(nullptr);

	for (_int i = 0; i < m_tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count ; ++i)
		arraySRVs[i] = m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i] == nullptr ? m_pDefaultBlackSRV
		: m_tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i]->Get_SRV();

	/* 모은 SRV 던져주기 */
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
		MSG_BOX("CB_MIX_RGBA_INFO 바인딩 실패 문자열 검색 확인");
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

		/* 텍스처들을 엎어온다... */
		CTextureBase* pTexBase = m_pGameInstance->GetOrAddTexture(L"Texture_" + tTexDesc.wstrName, &tTexDesc );
		if (pTexBase == nullptr)
			return E_FAIL;

		/* 구역이름으로 나눠주기 */
		FullPath._Remove_filename_and_separator();
		wstring wstrFloderName = FullPath.filename();
		m_umapMapTextures[wstrFloderName].push_back(pTexBase);
	}

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
	m_funcMapObjectCloneFactory =
		[=](void* pArg)->CGameObject* { return m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_GameObject_MapObject",
			ENUM_TO_UINT(ELevelType::MAP), g_wszMapObjectLayer, pArg); };

	return S_OK;
}

HRESULT CMapToolManager::Bind_SplatingTextureInfo()
{
	/* Binding Texture */
	if (FAILED(Bind_MapTexture()))
		return E_FAIL;

	/* 현재 사용하는 RGBA Map 개수 Binding */
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
	/* 마우스가 ImGUi 창 위에 있거나 조작중일떄는 입력방지 */
	ImGuiIO& io = ImGui::GetIO();
	//if (io.WantCaptureMouse)
	//	return;

	/* 휠 */
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
	/* Picking이면 피킹된 위치 파악 */
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
	/* 이미 한번 모델이 생성된 객체 */

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
			/* SRT 하나만 Push 하고 하나씩 생성 */
			tDesc.vecSRTs.clear();
			tDesc.vecClientMakePathDesc.clear();

			tDesc.vecSRTs.push_back(vecSRTData[i]);

			CLIENT_MAKEPATH_DESC_BASE* pClientMakePathDesc{ nullptr };
			if (!vecClientMakePathDesc.empty())
			{
				/* 포인터로 넣어주긴하지만 Clone 할 때 내부적으로 New 복사생성 호출해서 복사해간다 */
				tDesc.vecClientMakePathDesc.push_back(vecClientMakePathDesc[i]);

			}

			pResult = Make_MapObject(&tDesc);

			if (pResult == nullptr)
			{
				/* 실패했을시 누수처리 */
				Safe_Delete(pClientMakePathDesc);
				return E_FAIL;
			}
		}
		pChangeMapObject->Set_Dead(pChangeMapObject->Get_LayerTag());
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
				m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), m_pPreviewMapobject->Get_LayerTag(), m_pPreviewMapobject);
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
			m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), m_pPreviewMapobject->Get_LayerTag(), m_pPreviewMapobject);
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

	/* 미리 한곳으로 합치기 */
	vector<CMapObject*> vecMapObject{};
	/* Section 별로 Instance를 하고싶은데 */
	//vecMapObject.reserve(iAllSize);
	//if(pStaticModelList) vecMapObject.insert(vecMapObject.end() , pStaticModelList->begin(), pStaticModelList->end());
	//if(pInstanceModelList) vecMapObject.insert(vecMapObject.end() , pInstanceModelList->begin(), pInstanceModelList->end());

	/* 사용하는 모델주소가 같은 StaticModel을 모아둘 장소 */
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

	/* 그리고 한개짜리인 */



	return S_OK;
}



HRESULT CMapToolManager::Make_DefaultTexture()
{
	// 공통 텍스처 설정 (1x1 픽셀, 32비트 RGBA)
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

/* 2d Texture Array를 SRV로 반환해서 내뱉어주는 함수  */
HRESULT CMapToolManager::Slice_DH_Texture()
{
	if (m_tTextureSplattingInfo.pMix_DH_Tile_Texture == nullptr) return S_OK;

	for (auto& pSRV : m_tTextureSplattingInfo.vecDHTextureArraySlices)
		pSRV->Release();
	m_tTextureSplattingInfo.vecDHTextureArraySlices.clear();

	ID3D11ShaderResourceView* pOriginalSRV = m_tTextureSplattingInfo.pMix_DH_Tile_Texture->Get_SRV();
	ID3D11Resource* pRes = nullptr;
	pOriginalSRV->GetResource(&pRes);

	ID3D11Texture2D* pTex2D = (ID3D11Texture2D*)pRes;
	D3D11_TEXTURE2D_DESC tTexDesc;
	pTex2D->GetDesc(&tTexDesc); 

	m_tTextureSplattingInfo.vecDHTextureArraySlices.reserve(tTexDesc.ArraySize);
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
			pRes->Release();
			return E_FAIL;
		}

		m_tTextureSplattingInfo.vecDHTextureArraySlices.push_back(pSliceSRV);
	}

	pRes->Release();
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

	/* 배치모드가 싱글이면 한번생성하고 끝  */
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
				MSG_BOX(" Brush로 그릴 수 있는 그룹 최대 인스턴생 개수에 도달했습니다 새로 생성 OR 인스턴싱 그룹 Max Count를 조절해주세요 ");
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


	/* 안에 들어있는 Texture 정리 */
	UnRegister_MapTexture();


	m_pLevelMap = nullptr;


	for (auto& Factory : m_arrayMapObjectCloneFactory)
		Factory = nullptr;
}
