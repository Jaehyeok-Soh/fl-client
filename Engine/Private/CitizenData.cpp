#include "Engine_pch.h"
#include "CitizenData.h"
#include <fstream>

#include "Shader.h"
#include "Model.h"
#include "ModelAnimation.h"
#include <tchar.h>
#include "GameInstance.h"

NS_BEGIN(DTO)


#pragma region NPC Data
void CITIZEN_DATA::from_Json(const json& LoadJson)
{
	// "Atlas Datas" 키가 존재하고, 그 값이 배열인지 확인
	if (LoadJson.contains("Atlas Datas") && LoadJson["Atlas Datas"].is_array())
	{
		const auto& AltasDataArray_Json = LoadJson["Atlas Datas"];

		// 실제 가지고 있는 array 크기와 Json 배열 크기 중 작은 값을 기준으로 루프 
		// (혹시 Json에 데이터가 더 많아도 배열 범위 초과 에러 방지)
		size_t loopCount = std::min(arrayNpcAtlasData.size(), AltasDataArray_Json.size());

		for (size_t i = 0; i < loopCount; ++i)
		{
			// 각 인덱스에 맞는 구조체의 from_Json 호출
			arrayNpcAtlasData[i].from_Json(AltasDataArray_Json[i]);
		}
	}
	if (LoadJson.contains("Cloth RGBA Color"))
	{
		auto& RGBColor_LoadJson = LoadJson["Cloth RGBA Color"];

		Engine_Utils::read_vec4_xyzw(RGBColor_LoadJson["R"], this->tClothRGBColor.vColorR);
		Engine_Utils::read_vec4_xyzw(RGBColor_LoadJson["G"], this->tClothRGBColor.vColorG);
		Engine_Utils::read_vec4_xyzw(RGBColor_LoadJson["B"], this->tClothRGBColor.vColorB);

		this->isUseClothColorMapping = true;
	}
	else
		this->isUseClothColorMapping = false;

	if (LoadJson.contains("Parts Datas"))
	{
		auto& PartDatas_LoadJson = LoadJson["Parts Datas"];
		
		for (_uint i = 0; i < ENUM_TO_UINT(CITIZEN_PARTTYPE::END); ++i)
		{
			string strPartName = CitizenPartType_ToString(CITIZEN_PARTTYPE(i));
			if (PartDatas_LoadJson.contains(strPartName))
			{
				this->arrayPartDatas[i].from_Json(PartDatas_LoadJson[strPartName]);
			}
		}
	}

	if (LoadJson.contains("Loop Animation Name"))
	{
		this->strLoopAnimationName = LoadJson["Loop Animation Name"];
	}
	if (LoadJson.contains("Model Name"))
	{
		this->strModelName = LoadJson["Model Name"];
	}
}

void CITIZEN_DATA::to_Json(json& SaveJson)
{

	auto& AltasDataArray_Json = SaveJson["Atlas Datas"];
	for (auto& AtlasData : arrayNpcAtlasData)
	{
		json AtlasData_json = json::object();
		AtlasData.to_Json(AtlasData_json);
		AltasDataArray_Json.push_back(AtlasData_json);
	}

	SaveJson["Model Name"] = this->strModelName;

	if (this->isUseClothColorMapping)
	{
		auto& RGBColor_SaveJson = SaveJson["Cloth RGBA Color"];

		Engine_Utils::write_vec4_xyzw(RGBColor_SaveJson["R"], this->tClothRGBColor.vColorR);
		Engine_Utils::write_vec4_xyzw(RGBColor_SaveJson["G"], this->tClothRGBColor.vColorG);
		Engine_Utils::write_vec4_xyzw(RGBColor_SaveJson["B"], this->tClothRGBColor.vColorB);

	}

	auto& Parts_SaveJson = SaveJson["Parts Datas"];
	for (_uint i = 0; i < ENUM_TO_UINT(CITIZEN_PARTTYPE::END); ++i)
	{
		string strPartName = CitizenPartType_ToString(CITIZEN_PARTTYPE(i));
		auto& Part_SaveJson = Parts_SaveJson[strPartName];
		this->arrayPartDatas[i].to_Json(Part_SaveJson);
	}

	SaveJson["Loop Animation Name"] = this->strLoopAnimationName;
}
#pragma endregion


#pragma region NPC 아틸라스 정보

void CITIZEN_ATLAS_DATA::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Use Atlas"))
		this->isUseAtlas = LoadJson["Use Atlas"];

	if (LoadJson.contains("Max Colum"))
		this->iMaxColumn = LoadJson["Max Colum"];

	if (LoadJson.contains("Max Row"))
		this->iMaxRow = LoadJson["Max Row"];

	if (LoadJson.contains("Select Row"))
		this->iSelectRow = LoadJson["Select Row"];

	if (LoadJson.contains("Select Column"))
		this->iSelectColumn = LoadJson["Select Column"];
}

void CITIZEN_ATLAS_DATA::to_Json(json& SaveJson)
{
	SaveJson["Use Atlas"] = this->isUseAtlas;
	SaveJson["Max Colum"] = this->iMaxColumn;
	SaveJson["Max Row"] = this->iMaxRow;
	SaveJson["Select Row"] = this->iSelectRow;
	SaveJson["Select Column"] = this->iSelectColumn;
}
#pragma endregion


#pragma region PartData

void CITIZEN_PART_DATA::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Color"))
	{
		Engine_Utils::read_vec4_xyzw(LoadJson["Color"], this->vColor);
	}

	if (LoadJson.contains("Name"))
	{
		this->strName = LoadJson["Name"];
	}
}

void CITIZEN_PART_DATA::to_Json(json& SaveJson)
{
	Engine_Utils::write_vec4_xyzw(SaveJson["Color"],this->vColor);
	SaveJson["Name"] = this->strName;
}

#pragma endregion


void CB_CitizentFaceData::SetFaceUV(CITIZEN_ATLAS_TYPE eType, const struct CITIZEN_ATLAS_DATA* pData)
{

	if (!pData || !pData->isUseAtlas) {
		tCitizenFaceUV[ENUM_TO_UINT(eType)] = { {0.f, 0.f}, {1.f, 1.f} };
		return;
	}

	float fScaleX = 1.0f / (float)pData->iMaxColumn;
	float fScaleY = 1.0f / (float)pData->iMaxRow;

	auto& target = tCitizenFaceUV[ENUM_TO_UINT(eType)];
	target.vUVScale = { fScaleX, fScaleY };
	target.vUVOffset = { fScaleX * pData->iSelectColumn , fScaleY * pData->iSelectRow};
}

HRESULT CitizenWayPointOriginData::Load_CitizenWayPointDatas(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext)
{
	std::ifstream ifs{ wstrCitizenWaypointDatasPath };

	if (!ifs.is_open())
		return S_OK;

	if (ifs.peek() == std::ifstream::traits_type::eof())
		return S_OK;

	json LoadJson;
	ifs >> LoadJson;

	mapCitizenWapointDatas.clear();

	for (const auto& item : LoadJson.items())
	{
		string strKey = item.key();


		const auto& PathListJson = item.value();

		for (const auto& PathJson : PathListJson)
		{
			Citizen_WayPoint_Data tData(pDeivce, pContext);

			tData.Load_Json(PathJson);

			mapCitizenWapointDatas[strKey].push_back(tData);
		}
	}

	ifs.close();

	return S_OK;
}

HRESULT CitizenWayPointOriginData::Load_CitizenWayPointDatas(const string& strLevelName, _uint iIndex , Citizen_WayPoint_Data& tOutData)
{
	if (strLevelName.empty()) return E_FAIL;

	auto iter = mapCitizenWapointDatas.find(strLevelName);

	// 해당 레벨이 없거나, 인덱스가 범위를 벗어나면 실패
	if (iter == mapCitizenWapointDatas.end()) return E_FAIL;

	if (iIndex < 0 || iIndex >= (int)iter->second.size()) return E_FAIL;

	// 데이터 복사
	tOutData = iter->second[iIndex];

	return S_OK;
}

HRESULT CitizenWayPointOriginData::Save_CitizenWayPointDatas(const string& strLevelName, const Citizen_WayPoint_Data& tData , _int iIndex)
{
	if (strLevelName.empty()) return E_FAIL;

	// 1. 해당 레벨의 벡터 주소를 가져옵니다. (없으면 새로 생성됨)
	auto& vecDatas = mapCitizenWapointDatas[strLevelName];

	// 2. 인덱스에 따른 처리
	if (iIndex >= 0 && iIndex < (int)vecDatas.size())
	{
		// [수정] 기존 데이터 덮어쓰기
		vecDatas[iIndex] = tData;
	}
	else
	{
		// [추가] 인덱스가 -1이거나 범위를 벗어나면 새로 추가
		vecDatas.push_back(tData);
	}

	return S_OK;
}

HRESULT CitizenWayPointOriginData::Save_CitizenWayPointDatas()
{
	std::ofstream ofs{ wstrCitizenWaypointDatasPath };

	if (!ofs.is_open())
		return E_FAIL;

	json SaveRootJson = json::object();

	for (auto& [strLevelName, vecPaths] : mapCitizenWapointDatas)
	{
		json LevelPathsArray = json::array();

		for (auto& tData : vecPaths)
		{
			json PathJson = json::object();

			tData.Save_Json(PathJson);

			LevelPathsArray.push_back(PathJson);
		}
		SaveRootJson[strLevelName] = LevelPathsArray;
	}

	ofs << SaveRootJson.dump(4);
	ofs.close();

	return S_OK;
}


void CitizenWayPointOriginData::Render_CitizenWayPointRenderDebug(const string& strLevelName, _uint iIndex , CShader* pShader, CModel* pModel , _uint iPassIndex)
{
	auto iter = mapCitizenWapointDatas.find(strLevelName);

	if (!pShader && pModel) return;

	iter->second[iIndex].Render_Debug(pShader, pModel, iPassIndex);

	return;
}

const Citizen_WayPoint_Data* CitizenWayPointOriginData::Get_RandomWayPointOrignData(const string& strLevelName)
{
	auto iter = mapCitizenWapointDatas.find(strLevelName);

	if (iter == mapCitizenWapointDatas.end())
		return nullptr;

	const auto& vecPaths = iter->second;
	int iPathCount = (int)vecPaths.size();

	if (iPathCount == 0)
		return nullptr;

	int iRandomIndex = rand() % iPathCount;

	return &vecPaths[iRandomIndex];
}


void Citizen_WayPoint_Data::Render_Debug(CShader* pShader, CModel* pModel , _uint iPassIndex)
{
	if (this->vecPosition.empty()) return;

	if (!pShader)	return;
	if (!pModel)	return;

	auto pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Setup_ViewProj_ToCBuffer();

	_uint i = 0;
	for (auto& Pos : this->vecPosition)
	{
		/* World Matrix Binding */
		Matrix MatrixPosition =  Matrix::CreateTranslation(Pos);
		pShader->Bind_TransformData(  i == 0 ? Matrix::CreateFromYawPitchRoll( XMConvertToRadians(this->vStartPitchYawRoll.y) , 
			XMConvertToRadians(this->vStartPitchYawRoll.x) , XMConvertToRadians(this->vStartPitchYawRoll.z))* MatrixPosition :  MatrixPosition );

		_uint iMeshCount = pModel->Get_MeshCount();

		/* Pass 값 세팅 */
		pShader->Set_Pass(iPassIndex);

		for (_uint i = 0; i < iMeshCount; ++i)
		{
			pModel->Bind_Material(pShader, i);
			pModel->Bind_MaterialInstance(pShader, i);
			pShader->Apply();
			pModel->Render(i);
		}

		i++;
	}

	/* 2개 이상일때 */
	if (this->pBatch && this->pEffect && vecPosition.size() >= 2)
	{

		this->pContext->GSSetShader(nullptr, nullptr, 0);
		this->pContext->RSSetState(nullptr);
		this->pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		this->pEffect->Apply(this->pContext);
		this->pContext->IASetInputLayout(this->pInputLayout);
		this->pBatch->Begin();

		for (size_t i = 0; i < vecPosition.size() - 1; ++i)
		{
			pEffect->SetView(pGameInstance->Get_ViewMatrix());
			pEffect->SetProjection(pGameInstance->Get_ProjMatrix());
			pEffect->SetWorld(Matrix::Identity); // 경로는 월드좌표 기준이므로 Identity

			Vec3 vPos1 = vecPosition[i];
			Vec3 vPos2 = vecPosition[i + 1];

			// 색상은 노란색(Yellow) 등 취향껏 설정
			VertexPositionColor v1(vPos1, DirectX::Colors::Magenta);
			VertexPositionColor v2(vPos2, DirectX::Colors::Magenta);

			pBatch->DrawLine(v1, v2);
		}

		pBatch->End();
	}

}

void Citizen_WayPoint_Data::Load_Json(const json& LoadJson)
{
	if (LoadJson.contains("Duration"))
		this->fDuration = LoadJson["Duration"];

	if (LoadJson.contains("Start PitchYawRoll"))
		Engine_Utils::read_vec3_xyz(LoadJson["Start PitchYawRoll"], this->vStartPitchYawRoll);

	if (LoadJson.contains("Position Datas"))
	{
		auto& PositionDatas_LoadJson = LoadJson["Position Datas"];
		for (auto& DataJson : PositionDatas_LoadJson)
		{
			if (DataJson.is_null())
				continue;
			Vec3 vPos{};
			Engine_Utils::read_vec3_xyz(DataJson,vPos);
 			this->vecPosition.push_back(vPos);						/* Position 값이 추가 */
		}
	}
}

void Citizen_WayPoint_Data::Save_Json(json& SaveJson)
{
	SaveJson["Duration"] = this->fDuration;

	Engine_Utils::write_vec3_PitchYawRoll(SaveJson["Start PitchYawRoll"] , this->vStartPitchYawRoll );


	for (auto& Pos : this->vecPosition)
	{
		json PositionJson = json::object();
		Engine_Utils::write_vec3_xyz(PositionJson,Pos);
		SaveJson["Position Datas"].push_back(PositionJson);
	}
}



HRESULT CitizenPresetData::Add_ModelPrototype(_uint iAddPrototypeLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (vecDatas.empty())
		return S_OK;


	auto* pGameInstance = CGameInstance::GetInstance();

	static Matrix BodyPreMatrix = Matrix::CreateScale(0.01f, 0.01f, 0.01f) * Matrix::CreateRotationX(XMConvertToRadians(90.f));

	static const wstring& wstrCitizenTag = L"NPC_Citizen/";
	static const wstring& wstrCitizenPartTag = L"NPC_Citizen_Parts/";
	static const wstring& wstrPrototypeModelTag = L"Prototype_Component_Model_";

	for (auto& Data : vecDatas)
	{
		if(Data.strModelName.empty())
			continue;

		const wstring& wstrBodyModelFolderName = Engine_Utils::ToWString(Data.strModelName);

		CModel::MODEL_ORIGIN_DESC tBodyOriginDesc{};
		tBodyOriginDesc.iPrototypeLevelIndex = iAddPrototypeLevel;
		tBodyOriginDesc.eType = EModelType::ANIM;
		tBodyOriginDesc.FStageBone = CModel::STAGEING_BONE::SB_ALLBONE;
		tBodyOriginDesc.pMatPreTransform = &BodyPreMatrix;
		tBodyOriginDesc.wstrModelFolderName = wstrCitizenTag + wstrBodyModelFolderName;


		CModel::DATA_ANIMCHANNEL tAnimChannelData{};
		tAnimChannelData.bMixAni = false;
		tAnimChannelData.bRootAni = false;
		tAnimChannelData.iRootBoneIndex = 3;

		tBodyOriginDesc.pAniChannelData = &tAnimChannelData;

		if (nullptr == pGameInstance->Find_Prototype(iAddPrototypeLevel, wstrPrototypeModelTag + wstrBodyModelFolderName))
		{
			/* 찾았는데 없다면 */
			CModel* pPrototype_Model = CModel::Create(pDevice, pContext, &tBodyOriginDesc);
			if (pPrototype_Model == nullptr)
				return E_FAIL;

			if (FAILED(pGameInstance->Add_Prototype(iAddPrototypeLevel, wstrPrototypeModelTag + wstrBodyModelFolderName, pPrototype_Model)))
				return E_FAIL;
			/* Run Walk Anin Index 추가 */
			DTO::CitizenWalkRunAnimIndexData::Add_CitizenWalkRunAnimIndex(Data.strModelName , pPrototype_Model);
			Data.tWalkRunAnimIndex = DTO::CitizenWalkRunAnimIndexData::Get_CitizenWalkRunAnimIndex(Data.strModelName);
		}

		for (auto& Parts : Data.arrayPartDatas)
		{
			if (Parts.strName.empty())
				continue;

			const wstring& wstrPartModelFolderName = Engine_Utils::ToWString(Parts.strName);
			Matrix PartPreMatrix = Matrix::CreateTranslation(Get_CitizenPartsOffset_ByFolderPath(wstrPartModelFolderName));

			CModel::MODEL_ORIGIN_DESC tPartOriginDesc{};
			tPartOriginDesc.eType = EModelType::STATIC;
			tPartOriginDesc.wstrModelFolderName = wstrCitizenPartTag + wstrPartModelFolderName;
			tPartOriginDesc.iPrototypeLevelIndex = iAddPrototypeLevel;
			tPartOriginDesc.pMatPreTransform = &PartPreMatrix;


			if (nullptr == pGameInstance->Find_Prototype(iAddPrototypeLevel, wstrPrototypeModelTag + wstrPartModelFolderName))
			{
				/* 찾았는데 없다면 */
				if (FAILED(pGameInstance->Add_Prototype(iAddPrototypeLevel, wstrPrototypeModelTag + wstrPartModelFolderName, CModel::Create(pDevice, pContext, &tPartOriginDesc))))
					return E_FAIL;
			}
		}
	}

	return S_OK;
}

CITIZEN_DATA CitizenPresetData::Get_Preset(_uint iIndex)
{
	if (iIndex >= vecDatas.size()) return CITIZEN_DATA();
	return  vecDatas[iIndex];
}

CITIZEN_DATA& CitizenPresetData::Get_Preset_ForTool(_uint iIndex)
{
	return vecDatas[iIndex];
}

HRESULT CitizenPresetData::Load_CitizenPresetData()
{

	std::ifstream ifs{ wstrCitizenPresetDatasPath };

	// 파일이 없거나 비어있으면 그냥 패스
	if (!ifs.is_open() || ifs.peek() == std::ifstream::traits_type::eof())
		return S_OK;

	json LoadJson;
	ifs >> LoadJson;

	vecDatas.clear(); // 기존 메모리 싹 비우고

	// "Presets" 라는 키 배열이 있는지 확인
	if (LoadJson.contains("Presets") && LoadJson["Presets"].is_array())
	{
		for (const auto& presetJson : LoadJson["Presets"])
		{
			CITIZEN_DATA tData;
			tData.from_Json(presetJson); // 만들어두신 파싱 함수 꿀빨기
			vecDatas.push_back(tData);
		}
	}

	ifs.close();

	return S_OK;
}

HRESULT CitizenPresetData::Save_CitizenPresetData()
{
	std::filesystem::path filePath(wstrCitizenPresetDatasPath);
	std::filesystem::path dirPath = filePath.parent_path();

	if (!dirPath.empty() && !std::filesystem::exists(dirPath))
	{
		std::filesystem::create_directories(dirPath);
	}

	std::ofstream ofs{ wstrCitizenPresetDatasPath };

	if (!ofs.is_open())
		return E_FAIL;


	json SaveRootJson = json::object();
	json PresetsArray = json::array();

	// 벡터에 있는 모든 프리셋을 JSON 배열로 변환
	for (auto& tData : vecDatas)
	{
		json presetJson = json::object();
		tData.to_Json(presetJson); // 만들어두신 저장 함수 꿀빨기 2
		PresetsArray.push_back(presetJson);
	}

	SaveRootJson["Presets"] = PresetsArray;

	ofs << SaveRootJson.dump(4);
	ofs.close();

	return S_OK;
}

HRESULT CitizenPresetData::Update_CitizenPresetData(const CITIZEN_DATA& tData, _int iIndex)
{
	// 인덱스가 유효하면 덮어쓰기 (수정)
	if (iIndex >= 0 && iIndex < (int)vecDatas.size())
	{
		vecDatas[iIndex] = tData;
	}
	// 인덱스가 -1이거나 범위를 벗어나면 맨 뒤에 새로 추가
	else
	{
		vecDatas.push_back(tData);
	}

	// Save_CitizenPresetDatas(); 

	return S_OK;
}

HRESULT CitizenPresetData::Delete_CitizenPresetData(_int iIndex)
{
	if (iIndex < 0 || iIndex >= (int)vecDatas.size())
		return E_FAIL; // 엉뚱한 거 지우려고 하면 컷

	vecDatas.erase(vecDatas.begin() + iIndex);


	return S_OK;
}



void CitizenWalkRunAnimIndexData::Add_CitizenWalkRunAnimIndex(const std::string& strFolderName, CModel* pPrototypeModel)
{
	if (pPrototypeModel == nullptr) return;

	const wstring& wstrNPCTag = L"NPC_";

	_uint iHash = Engine_Utils::ToHash(strFolderName.c_str());

	/* 이미 찾은 애니매이션 데이터 */
	if (mapCitizenWalkRunAnimIndex.find(iHash) != mapCitizenWalkRunAnimIndex.end())
		return;

	vector<class CModelAnimation*>& vecAnim =  pPrototypeModel->Get_Animations();


	// 저장할 인덱스 변수 초기화 (-1이나 특정 최대값으로 초기화하는 것이 안전)
	_uint iWalkIndex = 0;
	_uint iRunIndex = 0;

	// 2. 애니메이션 인덱스를 알아야 하므로 일반 for문 사용
	for (_uint i = 0; i < vecAnim.size(); ++i)
	{
		auto pAnim = vecAnim[i];
		if (!pAnim) continue;

		// 이름 가져오기 (TCHAR* 형태라고 가정)
		const _tchar* szAnimName = pAnim->Get_Name();
		if (!szAnimName) continue;

		// 3. TCHAR 문자열 검색 (_tcsstr 사용)
		// szAnimName 안에 TEXT("_Walk_Loop")가 포함되어 있다면 nullptr이 아닌 주소값을 반환합니다.
		if (_tcsstr(szAnimName, TEXT("_Walk_Loop")) != nullptr)
		{
			iWalkIndex = i;
		}
		else if (_tcsstr(szAnimName, TEXT("_Run_Loop")) != nullptr)
		{
			iRunIndex = i;
		}
	}

	mapCitizenWalkRunAnimIndex[iHash] = {iWalkIndex , iRunIndex};
}

const CitizenWalkRunAnimIndex& CitizenWalkRunAnimIndexData::Get_CitizenWalkRunAnimIndex(const std::string& strFolderName)
{
	return mapCitizenWalkRunAnimIndex[Engine_Utils::ToHash(strFolderName.c_str())];
}

NS_END
