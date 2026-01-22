#include "Converter.h"
#include "Engine_Utils.h"
#include <fstream>
#include <regex>
#include "Bone.h"
#include "FileUtils.h"
#include "Importer.h"

CConverter::CConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float4x4 matPreTransform, _bool bCustom)
	: m_pDevice(pDevice)
	, m_bCustom(bCustom)
	, m_pDeviceContext(pDeviceContext)
	, m_matPreTransform(matPreTransform)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	m_pImporter = CImporter::Create();
}

HRESULT CConverter::Initialize(const _tchar* wszAssetParentFolderName)
{
	if (!wszAssetParentFolderName)
		return E_FAIL;

	_tchar srcBuffer[MAX_PATH] = L"";
	::lstrcatW(srcBuffer, g_wszAssetRelativePath);
	::lstrcatW(srcBuffer, wszAssetParentFolderName);
	m_RelativeFolderPath = wszAssetParentFolderName;
	m_AssetParentPath = std::filesystem::path(srcBuffer);
	m_pMaterials.reserve(30);
	m_pBones.reserve(600);
	m_pMeshes.reserve(30);
	return S_OK;
}

HRESULT CConverter::ReadAndExportFile()
{
	if (!std::filesystem::exists(m_AssetParentPath))
		return E_FAIL;

	if (FAILED(Read_Folder()))
		return E_FAIL;

	switch (m_eType)
	{
	case Engine::ConvertType::NORMAL:
	{
		if (FAILED(ReadAndExport_Normal()))
			return E_FAIL;
	} break;
	case Engine::ConvertType::ANIM:
	{
		if (FAILED(ReadAndExport_Animations()))
			return E_FAIL;
	} break;
	case Engine::ConvertType::MULTINORMALS:
	{
		if (FAILED(ReadAndExport_MapDatas()))
			return E_FAIL;
	} break;
	}

	return S_OK;
}

void CConverter::Read_Bones(aiNode* pNode, _int iIndex, _int iParent)
{
	AS_BONE* pBone = new AS_BONE();
	pBone->iIndex = iIndex;
	pBone->iParent = iParent;
	pBone->strName = pNode->mName.C_Str();
	::memcpy(&pBone->matTransform, &pNode->mTransformation, sizeof(_float4x4));
	::XMStoreFloat4x4(&pBone->matTransform, ::XMMatrixTranspose(::XMLoadFloat4x4(&pBone->matTransform)));
	m_pBones.push_back(pBone);

	// 재귀
	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Read_Bones(pNode->mChildren[i], (_int)m_pBones.size(), iIndex);
}

void CConverter::Read_MaterialData()
{
	if (m_pScene->mNumMaterials <= 0)
		return;

	for (_uint i = 0; i < m_pScene->mNumMaterials; ++i)
	{
		aiMaterial* srcMaterial = m_pScene->mMaterials[i];

		AS_MATERIAL* pMaterial = new AS_MATERIAL;
		pMaterial->strName = srcMaterial->GetName().C_Str();

		// Color
		{
			aiColor3D color;

			// Ambient
			srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
			pMaterial->vAmbient = { color.r, color.g, color.b, 1.f };

			// Diffuse
			srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			pMaterial->vDiffuse = { color.r, color.g, color.b, 1.f };

			// Specular
			srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
			pMaterial->vSpecular = { color.r, color.g, color.b, 1.f };
			srcMaterial->Get(AI_MATKEY_SHININESS, pMaterial->vSpecular.w);

			// Emissive
			srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			pMaterial->vEmissive = { color.r, color.g, color.b, 1.f };
		}

		// filePath
		{
			aiString file;

			// Diffuse Texture
			srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
			pMaterial->strDiffuseFile = file.C_Str();

			aiString file1;
			// Specular Texture
			srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file1);
			pMaterial->strSpecularFile = file1.C_Str();

			aiString file2;
			// Normal Texture
			srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file2);
			pMaterial->strNormalFile = file2.C_Str();
		}

		m_pMaterials.push_back(pMaterial);
	}
}

// 동일한 타입으로 읽어내서 저장하고
// 읽는곳, Loader에서 필요한 데이터만 뽑아서 각 컨테이너에 맞게 조정한다.
// 그래서 ModeType (Animation) 이 포함된 struct에 staticMesh, animMesh 모두 통일된 곳에 저장
void CConverter::Read_Meshes()
{
	if (m_pScene->mNumMeshes <= 0)
		return;

	_uint iMeshCount = m_pScene->mNumMeshes;
	m_pMeshes.resize(iMeshCount);
	// 미리 동적할당 아래에서 임의 접근을 위해
	for (_uint i = 0; i < iMeshCount; ++i)
		m_pMeshes[i] = new AS_MESH;

	_matrix matPreTransform = ::XMLoadFloat4x4(&m_matPreTransform);
	for (_uint m = 0; m < iMeshCount; ++m)
	{
		const aiMesh* pAiMesh = m_pScene->mMeshes[m];
		// 가독성을 위한 참조
		AS_MESH*& pCurrentMesh = m_pMeshes[m];

		pCurrentMesh->strName = pAiMesh->mName.C_Str();

		// 메쉬를 캐싱하고있는 객체 모델에 Material 컨테이너에 저장되고
		// 각 메쉬마다 어떤 Material 입혀줘야하는지에 대한 Index
		pCurrentMesh->iMaterialIndex = pAiMesh->mMaterialIndex;

		_uint iVertexCount = pAiMesh->mNumVertices;
		pCurrentMesh->vecVertices.resize(iVertexCount);

		if (m_eType != ConvertType::ANIM)
		{
			for (_uint v = 0; v < iVertexCount; ++v)
			{
				::memcpy(&pCurrentMesh->vecVertices[v].vPosition, &pAiMesh->mVertices[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vNormal, &pAiMesh->mNormals[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vTangent, &pAiMesh->mTangents[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vBinormal, &pAiMesh->mBitangents[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vUV, &pAiMesh->mTextureCoords[0][v], sizeof(_float2));

				::XMStoreFloat3(&pCurrentMesh->vecVertices[v].vPosition,
					::XMVector3TransformCoord(::XMLoadFloat3(&pCurrentMesh->vecVertices[v].vPosition), matPreTransform));
				::XMStoreFloat3(&pCurrentMesh->vecVertices[v].vNormal,
					::XMVector3TransformNormal(::XMLoadFloat3(&pCurrentMesh->vecVertices[v].vNormal), matPreTransform));
				::XMStoreFloat3(&pCurrentMesh->vecVertices[v].vTangent,
					::XMVector3TransformNormal(::XMLoadFloat3(&pCurrentMesh->vecVertices[v].vTangent), matPreTransform));
				::XMStoreFloat3(&pCurrentMesh->vecVertices[v].vBinormal,
					::XMVector3TransformNormal(::XMLoadFloat3(&pCurrentMesh->vecVertices[v].vBinormal), matPreTransform));
			}
		}
		else
		{
			for (_uint v = 0; v < iVertexCount; ++v)
			{
				::memcpy(&pCurrentMesh->vecVertices[v].vPosition, &pAiMesh->mVertices[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vNormal, &pAiMesh->mNormals[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vTangent, &pAiMesh->mTangents[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vBinormal, &pAiMesh->mBitangents[v], sizeof(_float3));
				::memcpy(&pCurrentMesh->vecVertices[v].vUV, &pAiMesh->mTextureCoords[0][v], sizeof(_float2));
			}
		}

		_uint iIndiciesCount = { 0 };
		pCurrentMesh->vecIndices.resize(pAiMesh->mNumFaces * 3);
		for (_uint i = 0; i < pAiMesh->mNumFaces; ++i)
		{
			pCurrentMesh->vecIndices[iIndiciesCount++] = pAiMesh->mFaces[i].mIndices[0];
			pCurrentMesh->vecIndices[iIndiciesCount++] = pAiMesh->mFaces[i].mIndices[1];
			pCurrentMesh->vecIndices[iIndiciesCount++] = pAiMesh->mFaces[i].mIndices[2];
		}

		// 이 메쉬를 구성하는 정점들에게 영향을 주는 뼈의 갯수
		_uint iAffectBoneCount = pAiMesh->mNumBones;

		// 만약 BoneCount가 0이라면 ? 아예 없는게 맞거나 있는데 애니메이션에 영향을 안받는 Mesh일수도 있음
		// ex. 검이 포함되어있는 모델인데 특정 bone matrix에 부착되는경우
		// 이럴 경우를 대비해서 0개일때 Mesh의 이름과 일치하는 Bone이 있는지 먼저 체크
		if (iAffectBoneCount <= 0)
		{
			// 찾았는데 없을 때 돌아가기
			_int iSrcBoneIndex = Get_BoneIndex(pAiMesh->mName.C_Str());
			if (iSrcBoneIndex == -1)
				continue;

			// 있다면? 위와같은 상황이니 아래와 같이 임의의 값을 넣어준다.
			pCurrentMesh->iAffectBoneCount = 1;
			pCurrentMesh->vecOffsetMatrices.resize(1);
			pCurrentMesh->vecAffectBoneIndices.resize(1);
			::XMStoreFloat4x4(&pCurrentMesh->vecOffsetMatrices[0], ::XMMatrixIdentity());
			pCurrentMesh->vecAffectBoneIndices[0] = iSrcBoneIndex;
			continue;
		}

		if (m_pMasterBones.size() <= 0)
		{
			Read_Default_AffectBoneData(iVertexCount, iAffectBoneCount, pAiMesh, pCurrentMesh);
		}
		else
			Read_ForMasterBone_AffectBoneData(iVertexCount, iAffectBoneCount, pAiMesh, pCurrentMesh);
	}
}

void CConverter::Read_AnimationData()
{
	if (m_pScene->mNumAnimations <= 0)
		return;

	m_pAnimations.reserve(m_pScene->mNumAnimations);
	for (_uint a = 0; a < m_pScene->mNumAnimations; ++a)
	{
		const aiAnimation* pAiAnimation = m_pScene->mAnimations[a];

		AS_ANIMATION* pAnimation = new AS_ANIMATION();

		pAnimation->strName = pAiAnimation->mName.C_Str();
		pAnimation->fTickPerSecond = (_float)pAiAnimation->mTicksPerSecond;
		pAnimation->fDuration = (_float)pAiAnimation->mDuration;

		// 각 뼈가 시간에 따라 어떻게 움직일지에 대한 정보
		// Channel 은 뼈!
		pAnimation->vecChannels.resize(pAiAnimation->mNumChannels, nullptr);
		for (auto& pChannel : pAnimation->vecChannels)
			pChannel = new AS_CHANNEL;

		for (_uint c = 0; c < pAiAnimation->mNumChannels; ++c)
		{
			const aiNodeAnim* pAiChannel = pAiAnimation->mChannels[c];
			AS_CHANNEL*& pChannel = pAnimation->vecChannels[c];
			pChannel->strBoneName = pAiChannel->mNodeName.C_Str();
			pChannel->iBoneIndex = Get_BoneIndex(pAiChannel->mNodeName.C_Str());
			// 최대 KeyFrame 추출
			pChannel->iKeyFrameCount = (std::max)(pAiChannel->mNumPositionKeys, pAiChannel->mNumScalingKeys);
			pChannel->iKeyFrameCount = (std::max)(pChannel->iKeyFrameCount, pAiChannel->mNumRotationKeys);

			_float3		vScale{ 1.f, 1.f, 1.f };
			_float4		vQuaternion{ 0.f, 0.f, 0.f, 1.f };
			_float3		vTranslation{ 0.f, 0.f, 0.f };
			pChannel->vecKeyFrames.resize(pChannel->iKeyFrameCount);
			for (_uint k = 0; k < pChannel->iKeyFrameCount; ++k)
			{
				AS_KEYFRAME KeyFrame = {};

				if (pAiChannel->mNumScalingKeys > k)
				{
					::memcpy(&vScale, &pAiChannel->mScalingKeys[k].mValue, sizeof(_float3));
					KeyFrame.fTrackPosition = (_float)pAiChannel->mScalingKeys[k].mTime;
				}

				if (pAiChannel->mNumRotationKeys > k)
				{
					vQuaternion.x = pAiChannel->mRotationKeys[k].mValue.x;
					vQuaternion.y = pAiChannel->mRotationKeys[k].mValue.y;
					vQuaternion.z = pAiChannel->mRotationKeys[k].mValue.z;
					vQuaternion.w = pAiChannel->mRotationKeys[k].mValue.w;

					KeyFrame.fTrackPosition = (_float)pAiChannel->mRotationKeys[k].mTime;
				}

				if (pAiChannel->mNumPositionKeys > k)
				{
					::memcpy(&vTranslation, &pAiChannel->mPositionKeys[k].mValue, sizeof(_float3));
					KeyFrame.fTrackPosition = (_float)pAiChannel->mPositionKeys[k].mTime;
				}

				KeyFrame.vScale = vScale;
				KeyFrame.vQuaternion = vQuaternion;
				KeyFrame.vTranslation = vTranslation;

				pChannel->vecKeyFrames[k] = KeyFrame;
			}
		}
		m_pAnimations.push_back(pAnimation);
	}
}


HRESULT CConverter::Export_ModelData()
{
	std::filesystem::path ModelfinalPath = { g_wszModelRelativePath };
	ModelfinalPath /= m_RelativeFolderPath;
	ModelfinalPath /= L"Model";

	// BoneData
	if(m_pBones.size() > 0)
	{
		std::filesystem::path BoneFinalPath = { ModelfinalPath };
		BoneFinalPath /= m_wstrAssetName;
		BoneFinalPath.replace_extension(g_wszBoneExtension);

		// 폴더 생성
		if (!std::filesystem::exists(ModelfinalPath))
		{
			if (!std::filesystem::exists(ModelfinalPath.parent_path()))
			{
				std::filesystem::create_directory(ModelfinalPath.parent_path());
			}
			std::filesystem::create_directory(ModelfinalPath);
		}

		CFileUtils* pFileUtil = CFileUtils::Create();
		if (FAILED(pFileUtil->Open(BoneFinalPath, FileMode::WRITE)))
		{
			MSG_BOX("CConverter::Export_ModelData, Failed");
			Safe_Release(pFileUtil);
			return E_FAIL;
		}

		pFileUtil->Write<_uint>((_uint)m_pBones.size());
		for (AS_BONE* pElement : m_pBones)
		{
			pFileUtil->Write<_uint>(pElement->iIndex);
			pFileUtil->Write<string>(pElement->strName);
			pFileUtil->Write<_uint>(pElement->iParent);
			pFileUtil->Write<_float4x4>(pElement->matTransform);
		}
		Safe_Release(pFileUtil);

		{
			FILE* pFile = {};
			string path = "../../Resources/Data/ModelData/" + m_pMeshes[0]->strName + "_Bones.csv";
			::fopen_s(&pFile, path.c_str(), "w");

			for (AS_BONE* pBone : m_pBones)
			{
				string name = pBone->strName;
				::fprintf(pFile, "%d, %s, %f, %f, %f,\n", pBone->iIndex, pBone->strName.c_str(), pBone->matTransform._41, pBone->matTransform._42, pBone->matTransform._43);
			}

			::fprintf(pFile, "\n");
			::fprintf(pFile, "Pos.x, Pos.y, Pos.x, indices.x, indices.y, indices.z, indices.w, wieghts.x, wieghts.y, wieghts.z, wieghts.w\n");
			for (AS_MESH* pMesh : m_pMeshes)
			{
				string name = pMesh->strName;
				::printf("%s\n", name.c_str());
				for (size_t i = 0; i < pMesh->vecVertices.size(); ++i)
				{
					_float3 p = pMesh->vecVertices[i].vPosition;
					XMUINT4 indices = pMesh->vecVertices[i].vBlendIndices;
					_float4 weights = pMesh->vecVertices[i].vBlendWeights;

					::fprintf(pFile, "%f, %f, %f,", p.x, p.y, p.z);
					::fprintf(pFile, "%d, %d, %d, %d,", indices.x, indices.y, indices.z, indices.w);
					::fprintf(pFile, "%f, %f, %f, %f\n", weights.x, weights.y, weights.z, weights.w);
				}
			}

			::fclose(pFile);
		}
	}

	// MeshData
	{
		std::filesystem::path MeshFinalPath = { ModelfinalPath };
		MeshFinalPath /= m_wstrAssetName;
		MeshFinalPath.replace_extension(g_wszMeshExtension);

		// 폴더 생성
		if (!std::filesystem::exists(ModelfinalPath))
		{
			if (!std::filesystem::exists(ModelfinalPath.parent_path()))
			{
				std::filesystem::create_directory(ModelfinalPath.parent_path());
			}
			std::filesystem::create_directory(ModelfinalPath);
		}

		CFileUtils* pFileUtil = CFileUtils::Create();
		if (FAILED(pFileUtil->Open(MeshFinalPath, FileMode::WRITE)))
		{
			MSG_BOX("CConverter::Export_ModelData, Failed");
			Safe_Release(pFileUtil);
			return E_FAIL;
		}

		pFileUtil->Write<_uint>((_uint)m_pMeshes.size());
		for (AS_MESH* pElement : m_pMeshes)
		{
			pFileUtil->Write<string>(pElement->strName);
			pFileUtil->Write<_uint>(pElement->iMaterialIndex);

			// Vertex Data
			pFileUtil->Write<_uint>((_uint)pElement->vecVertices.size());
			if (!pElement->vecVertices.empty())
				pFileUtil->Write(&pElement->vecVertices[0], sizeof(VTXANIMMESH) * (_uint)pElement->vecVertices.size());

			// Index Data
			pFileUtil->Write<_uint>((_uint)pElement->vecIndices.size());
			if (!pElement->vecIndices.empty())
				pFileUtil->Write(&pElement->vecIndices[0], sizeof(_uint) * (_uint)pElement->vecIndices.size());

			// AffectBoenData
			pFileUtil->Write<_uint>((_uint)pElement->vecAffectBoneIndices.size());
			if (!pElement->vecAffectBoneIndices.empty())
				pFileUtil->Write(&pElement->vecAffectBoneIndices[0], sizeof(_uint) * (_uint)pElement->vecAffectBoneIndices.size());

			// OffsetMatrix
			pFileUtil->Write<_uint>((_uint)pElement->vecOffsetMatrices.size());
			if (!pElement->vecOffsetMatrices.empty())
				pFileUtil->Write(&pElement->vecOffsetMatrices[0], sizeof(_float4x4) * (_uint)pElement->vecOffsetMatrices.size());
		}
		
		Safe_Release(pFileUtil);
	}

	return S_OK;
}

HRESULT CConverter::Export_MaterialData()
{
	std::filesystem::path MaterialfinalPath = { g_wszModelRelativePath };
	MaterialfinalPath /= m_RelativeFolderPath;
	MaterialfinalPath /= L"Material";

	// 폴더 생성
	if (!std::filesystem::exists(MaterialfinalPath))
	{
		if (!std::filesystem::exists(MaterialfinalPath.parent_path()))
		{
			std::filesystem::create_directory(MaterialfinalPath.parent_path());
		}
		std::filesystem::create_directory(MaterialfinalPath);
	}

	string strFolder = MaterialfinalPath.string();

	MaterialfinalPath /= m_wstrAssetName;
	MaterialfinalPath.replace_extension(g_wszMaterialExtension);

	std::ofstream ofs(MaterialfinalPath, std::ios::out | std::ios::binary);
	if (!ofs.is_open())
	{
		MSG_BOX("CConverter::Export_MaterialData, Failed");
		return E_FAIL;
	}

	json jArray = json::array();
	for (AS_MATERIAL* pElement : m_pMaterials)
	{
		pElement->strDiffuseFile = Write_Texture(strFolder.c_str(), pElement->strDiffuseFile.c_str());
		pElement->strNormalFile = Write_Texture(strFolder.c_str(), pElement->strNormalFile.c_str());
		pElement->strSpecularFile = Write_Texture(strFolder.c_str(), pElement->strSpecularFile.c_str());
		jArray.push_back(*pElement);
	}

	// setw(i) 출력될 값의 최소폭을 i 만큼 지정
	// Json 이므로 4칸 들여쓰기로 출력
	ofs << std::setw(4) << jArray << std::endl;
	ofs.close();
	return S_OK;
}

HRESULT CConverter::Export_AnimationData(_uint iIndex)
{
	std::filesystem::path AnimationfinalPath = { g_wszModelRelativePath };
	AnimationfinalPath /= m_RelativeFolderPath;
	AnimationfinalPath /= L"Animation";

	// 폴더 생성
	if (!std::filesystem::exists(AnimationfinalPath))
	{
		if (!std::filesystem::exists(AnimationfinalPath.parent_path()))
		{
			std::filesystem::create_directory(AnimationfinalPath.parent_path());
		}

		std::filesystem::create_directory(AnimationfinalPath);
	}

	AnimationfinalPath /= m_vecAnimPaths[iIndex].wstrAnimationName;
	AnimationfinalPath.replace_extension(g_wszAnimationExtension);

	CFileUtils* pFileUtil = CFileUtils::Create();
	pFileUtil->Open(AnimationfinalPath, FileMode::WRITE);

	pFileUtil->Write<_uint>((_uint)m_pAnimations.size());
	for (_uint i = 0; i < (_uint)m_pAnimations.size(); ++i)
	{
		AS_ANIMATION* pAnimation = m_pAnimations[i];

		pFileUtil->Write<string>(pAnimation->strName);
		pFileUtil->Write<_float>(pAnimation->fDuration);
		pFileUtil->Write<_float>(pAnimation->fTickPerSecond);

		pFileUtil->Write<_uint>((_uint)pAnimation->vecChannels.size());
		for (AS_CHANNEL* pChannel : pAnimation->vecChannels)
		{
			pFileUtil->Write<string>(pChannel->strBoneName);
			pFileUtil->Write<_int>(pChannel->iBoneIndex);

			pFileUtil->Write<_uint>((_uint)pChannel->vecKeyFrames.size());
			for (AS_KEYFRAME& KeyFrame : pChannel->vecKeyFrames)
			{
				pFileUtil->Write<_float>(KeyFrame.fTrackPosition);
				pFileUtil->Write<_float3>(KeyFrame.vScale);
				pFileUtil->Write<_float4>(KeyFrame.vQuaternion);
				pFileUtil->Write<_float3>(KeyFrame.vTranslation);
			}
		}
	}

	Safe_Release(pFileUtil);
	return S_OK;
}

string CConverter::Write_Texture(const _char *szSaveFolder, const _char* szFile)
{
	string strFileName = std::filesystem::path(szFile).filename().string();
	string strFolderName = std::filesystem::path(szSaveFolder).filename().string();

	// fbx 파일에 텍스쳐가 바인딩된 경우
	const aiTexture* pSrcTexture = m_pScene->GetEmbeddedTexture(strFileName.c_str());
	if(pSrcTexture)
	{
		string strPath = (std::filesystem::path(szSaveFolder) / strFileName).string();

		if (pSrcTexture->mHeight == 0)
		{
			CFileUtils* pFileUtil = CFileUtils::Create();

			if (FAILED(pFileUtil->Open(Engine_Utils::ToWString(strPath), FileMode::WRITE)))
			{
				Safe_Release(pFileUtil);
				return "FAILED, FileUtil::Open";
			}
			if (FAILED(pFileUtil->Write(pSrcTexture->pcData, pSrcTexture->mWidth)))
			{
				Safe_Release(pFileUtil);
				return "FAILED, FileUtil::Open";
			}

			Safe_Release(pFileUtil);
		}
		else
		{
			D3D11_TEXTURE2D_DESC desc = {};
			::ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = pSrcTexture->mWidth;
			desc.Height = pSrcTexture->mHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA subResources = {};
			::ZeroMemory(&subResources, sizeof(D3D11_SUBRESOURCE_DATA));
			subResources.pSysMem = pSrcTexture->pcData;
			subResources.SysMemPitch = sizeof(aiTexel) * pSrcTexture->mWidth;

			ID3D11Texture2D* pTexture = { nullptr };
			if (FAILED(m_pDevice->CreateTexture2D(&desc, &subResources, &pTexture)))
				return string("FAILED, CreateTexture2D");

			ScratchImage img = {};
			if (FAILED(::CaptureTexture(m_pDevice, m_pDeviceContext, pTexture, img)))
				return string("FAILED, CaptureTexture");

			if (FAILED(::SaveToDDSFile(*img.GetImages(), DDS_FLAGS_NONE, Engine_Utils::ToWString(strFileName).c_str())))
				return string("FAILED, SaveToDDSFile");

			Safe_Release(pTexture);
		}
	}
	else
	{
		string strOrigin = (std::filesystem::path(g_wszAssetRelativePath) / strFolderName / szFile).string();
		Engine_Utils::Replace(strOrigin, "\\", "/");

		string strPath = (std::filesystem::path(szSaveFolder) / strFileName).string();
		Engine_Utils::Replace(strPath, "\\", "/");

		::CopyFileA(strOrigin.c_str(), strPath.c_str(), false);
	}

	return strFileName;
}

_int CConverter::Get_BoneIndex(const _char* szName)
{
	for (AS_BONE* pBone : m_pBones)
	{
		if (pBone->strName == szName)
			return pBone->iIndex;
	}

	return -1;
}

HRESULT CConverter::Read_Folder()
{
	if (std::filesystem::exists(m_AssetParentPath) == false)
	{
		MSG_BOX("CConverter::Read_Folder, AssetParentPath is invalid");
		return E_FAIL;
	}

	size_t iFileCount = Get_FileCount(m_AssetParentPath);
	size_t iFolderCount = Get_FolderCount(m_AssetParentPath);

	if (iFileCount == 1)
	{
		for (const auto& entry : std::filesystem::directory_iterator(m_AssetParentPath))
		{
			if (entry.is_regular_file())
			{
				m_wstrAssetName = entry.path().filename().lexically_normal().stem();
				break;
			}
		}

		// .fbx파일 하나에 폴더가 있으면 Animation폴더로 간주
		if (iFolderCount >= 1)
		{
			m_eType = ConvertType::ANIM;
		}
		// .fbx파일 하나만 있으면 단일 모델로 간주
		else
		{
			m_eType = ConvertType::NORMAL;
		}
	}
	// .fbx 1개 초과면 MapData로 간주
	else if (iFileCount > 1)
	{
		m_vecMapAssetPaths.resize(iFileCount);
		m_eType = ConvertType::MULTINORMALS;
	}
	else
	{
		MSG_BOX("CConverter::ReadAndExportFile, Folder rule violation");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CConverter::ReadAndExport_Normal()
{
	std::filesystem::path finalPath = m_AssetParentPath / m_wstrAssetName;
	finalPath.replace_extension(g_wszModelExtension);

	m_pScene = m_pImporter->Get_Assimp_Importer()->ReadFile(
		Engine_Utils::ToString(finalPath.c_str()),
		aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast
	);

	if (!m_pScene)
		return E_FAIL;

	m_pBones.reserve(m_pScene->mNumSkeletons);
	Read_Bones(m_pScene->mRootNode, -1, -1);
	Read_Meshes();
	Read_MaterialData();

	if (FAILED(Export_ModelData()))
		return E_FAIL;

	if (FAILED(Export_MaterialData()))
		return E_FAIL;

	return S_OK;
}

HRESULT CConverter::ReadAndExport_MapDatas()
{
	size_t iCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(m_AssetParentPath))
	{
		if (entry.is_regular_file())
		{
			m_vecMapAssetPaths[iCount] = entry.path();
			++iCount;
		}
	}

	for (size_t i = 0; i < m_vecMapAssetPaths.size(); ++i)
	{
		Clear();
		m_pScene = m_pImporter->Get_Assimp_Importer()->ReadFile(
			Engine_Utils::ToString(m_vecMapAssetPaths[i].c_str()),
			aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast
		);

		if (!m_pScene)
			return E_FAIL;

		m_wstrAssetName = m_vecMapAssetPaths[i].filename().lexically_normal().stem();

		//m_pBones.reserve(m_pScene->mNumSkeletons);
		//Read_Bones(m_pScene->mRootNode, -1, -1);
		Read_Meshes();
		Read_MaterialData();

		if (FAILED(Export_ModelData()))
			return E_FAIL;

		if (FAILED(Export_MaterialData()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CConverter::ReadAndExport_Animations()
{
	std::filesystem::path AnimFolder = m_AssetParentPath / L"Animations";
	if (std::filesystem::exists(AnimFolder) == false)
	{
		MSG_BOX("CConverter::Read_AnimationFolder, Animation folder is invalid");
		return E_FAIL;
	}

	size_t iFileCount = Get_FileCount(AnimFolder);
	if (iFileCount <= 0)
	{
		MSG_BOX("CConverter::Read_AnimationFolder, Animation file is invalid");
		return E_FAIL;
	}

	// 메모리 미리 확보
	m_vecAnimPaths.resize(iFileCount);

	// 경로 순회하면서 넣기
	_uint iCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(AnimFolder))
	{
		if (entry.is_regular_file())
		{
			m_vecAnimPaths[iCount] = ANIMPATHPART(entry.path());
			++iCount;
		}
	}

	// 룰 지정 alpha_alpha01
	std::wregex rule{ L"^([[:alpha:]]+)_([[:alnum:]]+)$" };
	const std::wsregex_iterator wstrItr_End;

	// 정규표현식 적용을 후 Tag와 AnimationName 추출을 위한 람다식
	for_each(m_vecAnimPaths.begin(), m_vecAnimPaths.end(),
		[&wstrItr_End, &rule](ANIMPATHPART& element)->void
		{
			std::wstring tmp{ element.filePath.stem() };
			if (std::wsmatch match; std::regex_match(tmp, match, rule))
			{
				// 0은 문자열 전체
				element.wstrTag = match[1];
				element.wstrAnimationName = match[2];
			}
		});

	// 포함된 모델 데이터 먼저 읽기
	if (FAILED(ReadAndExport_Normal()))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_vecAnimPaths.size(); ++i)
	{
		Clear(); // Bone을 제외한 모든 데이터 정리
		m_pScene = m_pImporter->Get_Assimp_Importer()->ReadFile(
			Engine_Utils::ToString(m_vecAnimPaths[i].filePath.c_str()),
			aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast
		);

		if (!m_pScene)
			return E_FAIL;

		Read_AnimationData();
		if (FAILED(Export_AnimationData(i)))
			return E_FAIL;
	}

	return S_OK;
}



_int CConverter::Get_BoneIndexFormMatserBone(const _char* szName)
{
	for (AS_BONE* pBone : m_pMasterBones)
	{
		if (pBone->strName == szName)
			return pBone->iIndex;
	}

	return -1;
}

size_t CConverter::Get_FileCount(const wstring wstrFolderPath)
{
	size_t iFileCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(wstrFolderPath))
	{
		if (entry.is_regular_file())
		{
			++iFileCount;
		}
	}

	return iFileCount;
}

size_t CConverter::Get_FolderCount(const wstring wstrFolderPath)
{
	size_t iFolderCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(wstrFolderPath))
	{
		if (entry.is_directory())
		{
			++iFolderCount;
		}
	}

	return iFolderCount;
}

// Bone 빼고 다 정리
void CConverter::Clear()
{
	m_pImporter->Get_Assimp_Importer()->FreeScene();

	for (AS_MATERIAL* pElement : m_pMaterials)
	{
		Safe_Delete(pElement);
	}

	for (AS_MESH* pElement : m_pMeshes)
	{
		Safe_Delete(pElement);
	}

	for (AS_ANIMATION* pElement : m_pAnimations)
	{
		for (AS_CHANNEL* pData : pElement->vecChannels)
		{
			Safe_Delete(pData);
		}
		pElement->vecChannels.clear();
		Safe_Delete(pElement);
	}

	m_pMaterials.clear();
	m_pMeshes.clear();
	m_pAnimations.clear();
}

void CConverter::Clear_For_Custom()
{
	Clear();
	for (AS_BONE* pElement : m_pBones)
		Safe_Delete(pElement);
	m_pBones.clear();
	m_vecAnimPaths.clear();
	m_vecMapAssetPaths.clear();
}

CConverter* CConverter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* wszAssetParentFolderPath, _float4x4 matPreTransform, _bool bCustom)
{
	CConverter* pInstance = new CConverter(pDevice, pDeviceContext, matPreTransform, bCustom);

	if (FAILED(pInstance->Initialize(wszAssetParentFolderPath)))
	{
		MSG_BOX("CConverter::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConverter::Read_Default_AffectBoneData(_uint iVertexCount, _uint iAffectBoneCount, const aiMesh* pAiMesh, AS_MESH* pCurrentMesh)
{
	// Weight 유틸용 벡터
	vector<AS_BONEWEIGHTS> tempVertexBoneWeights;
	tempVertexBoneWeights.resize(iVertexCount);

	pCurrentMesh->iAffectBoneCount = iAffectBoneCount;
	// 메시에 영향을 주는 뼈들의 인덱스를 모아놓는다.
	// 뼈들의 인덱스 ? 모델 객체가 들고 있는 뼈컨테이너의 특정 인덱스!
	pCurrentMesh->vecAffectBoneIndices.resize(iAffectBoneCount);

	// 특정 뼈를 정점에게 적용할 때?
	// 추가적인 보정 (Offset) 을 거쳐서 정점에게 상태를 전달 할 수있도록 한다.
	// 디자이너들이 정해놓은것이기때문에 우리가 직접 건드릴일은 적을 것
	pCurrentMesh->vecOffsetMatrices.resize(iAffectBoneCount);

	// Initialize, Indentity Matrix
	for (_uint i = 0; i < iAffectBoneCount; ++i)
		::XMStoreFloat4x4(&pCurrentMesh->vecOffsetMatrices[i], ::XMMatrixIdentity());

	for (_uint b = 0; b < iAffectBoneCount; ++b)
	{
		const aiBone* pAiBone = pAiMesh->mBones[b];
		_uint iAffectBoneIndex = Get_BoneIndex(pAiBone->mName.C_Str());
		if (iAffectBoneIndex == -1)
		{
			MSG_BOX("CConverter::Read_Meshes, iAffectBoneIndex is Invalid");
			return;
		}
		pCurrentMesh->vecAffectBoneIndices[b] = iAffectBoneIndex;

		::memcpy(&pCurrentMesh->vecOffsetMatrices[b], &pAiBone->mOffsetMatrix, sizeof(_float4x4));
		::XMStoreFloat4x4(&pCurrentMesh->vecOffsetMatrices[b],
			::XMMatrixTranspose(::XMLoadFloat4x4(&pCurrentMesh->vecOffsetMatrices[b])));

		// b번째 뼈는 몇개의 정점에게 영향을 주는가?
		for (_uint w = 0; w < pAiBone->mNumWeights; ++w)
		{
			const aiVertexWeight& AiWeight = pAiBone->mWeights[w];
			_uint iVertexIndex = AiWeight.mVertexId;
			_float fWeight = AiWeight.mWeight;

			// Wieght가 4개 넘어갈경우 큰값 상위 4개만 저장
			tempVertexBoneWeights[iVertexIndex].Add_Weights(b, fWeight);
		}
	}

	// 최종 결과 계산, Weight Normalize 추후 Shader에서 정렬할 필요 없음
	for (_uint v = 0; v < tempVertexBoneWeights.size(); ++v)
	{
		AS_BLENDWEIGHT blendWeight = tempVertexBoneWeights[v].Get_BlendWeight();
		pCurrentMesh->vecVertices[v].vBlendIndices = blendWeight.vIndices;
		pCurrentMesh->vecVertices[v].vBlendWeights = blendWeight.vWeights;
	}
}

void CConverter::Read_ForMasterBone_AffectBoneData(_uint iVertexCount, _uint iAffectBoneCount, const aiMesh* pAiMesh, AS_MESH* pCurrentMesh)
{
	// Weight 유틸용 벡터
	vector<AS_BONEWEIGHTS> tempVertexBoneWeights;
	tempVertexBoneWeights.resize(iVertexCount);

	// MasterBone에 맞추어 수정
	pCurrentMesh->iAffectBoneCount = iAffectBoneCount;
	pCurrentMesh->vecAffectBoneIndices.reserve(iAffectBoneCount);
	pCurrentMesh->vecOffsetMatrices.reserve(iAffectBoneCount);

	// Initialize, Indentity Matrix
	// 해당 Mesh의 Affect카운트를 일단 전부 순회
	// 필요없는 Bone의 경우 기록을 하지않지만, 스키닝데이터가 존재한다면 현재로선 매핑 불가능함
	for (_uint b = 0; b < iAffectBoneCount; ++b)
	{
		// 메시에 영향을 주는 Bone들을 순회
		const aiBone* pAiBone = pAiMesh->mBones[b];
		// Index정보를 마스터Bone 기준으로 들고온다.
		_int iAffectBoneIndex = Get_BoneIndexFormMatserBone(pAiBone->mName.C_Str());
		// b번째 뼈는 몇개의 정점에게 영향을 주는가?
		for (_uint w = 0; w < pAiBone->mNumWeights; ++w)
		{
			const aiVertexWeight& AiWeight = pAiBone->mWeights[w];
			_uint iVertexIndex = AiWeight.mVertexId;
			_float fWeight = AiWeight.mWeight;
			// Master본과 일치하지않는데, Weight값 즉 스키닝 데이터가 있다면 매핑이 불가능
			if (iAffectBoneIndex == -1 && fWeight > 0.0001f)
			{
				MSG_BOX("안돼!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				return;
			}
			else if(iAffectBoneIndex == -1)
				break;
			// Masetr본과 일치하지않지만, Wieght값은 

			// Wieght가 4개 넘어갈경우 큰값 상위 4개만 저장
			tempVertexBoneWeights[iVertexIndex].Add_Weights(iAffectBoneIndex, fWeight);
		}

		if (iAffectBoneIndex == -1)
			continue;

		_float4x4 matSrc = {};
		::memcpy(&matSrc, &pAiBone->mOffsetMatrix, sizeof(_float4x4));
		::XMStoreFloat4x4(&matSrc, ::XMMatrixTranspose(::XMLoadFloat4x4(&matSrc)));
		pCurrentMesh->vecAffectBoneIndices.push_back(iAffectBoneIndex);
		pCurrentMesh->vecOffsetMatrices.push_back(matSrc);
	}


	// 최종 결과 계산, Weight Normalize 추후 Shader에서 정렬할 필요 없음
	for (_uint v = 0; v < tempVertexBoneWeights.size(); ++v)
	{
		AS_BLENDWEIGHT blendWeight = tempVertexBoneWeights[v].Get_BlendWeight();
		pCurrentMesh->vecVertices[v].vBlendIndices = blendWeight.vIndices;
		pCurrentMesh->vecVertices[v].vBlendWeights = blendWeight.vWeights;
	}

}

void CConverter::Free()
{
	for (AS_MATERIAL* pElement : m_pMaterials)
	{
		Safe_Delete(pElement);
	}

	for (AS_MESH* pElement : m_pMeshes)
	{
		Safe_Delete(pElement);
	}

	for (AS_BONE* pElement : m_pMasterBones)
	{
		Safe_Delete(pElement);
	}

	for (AS_BONE* pElement : m_pBones)
	{
		Safe_Delete(pElement);
	}

	for (AS_ANIMATION* pElement : m_pAnimations)
	{
		for (AS_CHANNEL* pData : pElement->vecChannels)
		{
			Safe_Delete(pData);
		}
		pElement->vecChannels.clear();
		Safe_Delete(pElement);
	}

	m_pMasterBones.clear();
	m_pMaterials.clear();
	m_pMeshes.clear();
	m_pBones.clear();
	m_pAnimations.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pImporter);
	Super::Free();
}


#pragma region LegacyCSV
/*
{
	FILE* pFile = {};
	string path = "../../Resources/Data/ModelData/" + m_pMeshes[0]->strName + "_Vertices.csv";
	::fopen_s(&pFile, path.c_str(), "w");

	for (AS_BONE* pBone : m_pBones)
	{
		string name = pBone->strName;
		::fprintf(pFile, "%d, %s, %f, %f, %f,\n", pBone->iIndex, pBone->strName.c_str(), pBone->matTransform._41, pBone->matTransform._42, pBone->matTransform._43);
	}

	::fprintf(pFile, "\n");
	::fprintf(pFile, "Pos.x, Pos.y, Pos.x, indices.x, indices.y, indices.z, indices.w, wieghts.x, wieghts.y, wieghts.z, wieghts.w\n");
	for (AS_MESH* pMesh : m_pMeshes)
	{
		string name = pMesh->strName;
		::printf("%s\n", name.c_str());
		for (size_t i = 0; i < pMesh->vecVertices.size(); ++i)
		{
			_float3 p = pMesh->vecVertices[i].vPosition;
			_float4 indices = pMesh->vecVertices[i].vBlendIndices;
			_float4 weights = pMesh->vecVertices[i].vBlendWeights;

			::fprintf(pFile, "%f, %f, %f,", p.x, p.y, p.z);
			::fprintf(pFile, "%f, %f, %f, %f,", indices.x, indices.y, indices.z, indices.w);
			::fprintf(pFile, "%f, %f, %f, %f\n", weights.x, weights.y, weights.z, weights.w);
		}
	}

	::fclose(pFile);
}
*/
#pragma endregion