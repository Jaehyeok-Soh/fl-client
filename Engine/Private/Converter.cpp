#include "Engine_pch.h"
#include "Converter.h"
#include "Engine_Utils.h"
#include <fstream>
#include <regex>
#include "Bone.h"
#include "FileUtils.h"
#include "Importer.h"

#define MAX_COUNT_FOR_CREATEFOLDER 5

CConverter::CConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const Matrix& matPreTransform)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_matPreTransform(matPreTransform)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	m_pImporter = CImporter::Create();
}

HRESULT CConverter::Initialize(const _tchar* wszAssetParentFolderName, const _char* szSolutionFullPath)
{
	if (wszAssetParentFolderName == nullptr)
		return E_FAIL;

	path assetsfolderPath = path("Resources") / "Assets";
	path prefixRelativePath = g_wszAssetRelativePath;
	path solutionFullPath = szSolutionFullPath;
	path fullFolderPath = wszAssetParentFolderName;

	solutionFullPath = std::filesystem::weakly_canonical(solutionFullPath);
	fullFolderPath = std::filesystem::weakly_canonical(fullFolderPath);

	if (FAILED(Is_SubPath(solutionFullPath, fullFolderPath)))
		return E_FAIL;
	
	path assetAbs = solutionFullPath / assetsfolderPath;
	
	if (FAILED(Is_SubPath(assetAbs, fullFolderPath)))
		return E_FAIL;

	m_RelativeFolderPath = fullFolderPath.lexically_relative(assetAbs);
	m_AssetParentPath = prefixRelativePath / m_RelativeFolderPath;
	m_pMaterials.reserve(30);
	m_pBones.reserve(600);
	m_pMeshes.reserve(30);
	return S_OK;
}

HRESULT CConverter::ReadAndExport()
{
	if (std::filesystem::exists(m_AssetParentPath) == false)
		return E_FAIL;

	if (FAILED(Check_Folder()))
		return E_FAIL;

	switch (m_eType)
	{
	case ConvertType::NONANIM:
	{
		if (FAILED(ReadAndExport_NoAnimation()))
			return E_FAIL;
	} break;
	case ConvertType::NONANIM_MORETHANONE:
	{
		if (FAILED(ReadAndExport_MoreThanOne()))
			return E_FAIL;
	} break;
	case ConvertType::ANIM:
	{
		if (FAILED(ReadAndExport_Animation()))
			return E_FAIL;
	} break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

void CConverter::Read_Bones(aiNode* pNode, _int iIndex, _int iParent)
{
	AS_BONE* pBone = new AS_BONE();
	pBone->iIndex = iIndex;
	pBone->iParent = iParent;
	pBone->strName = pNode->mName.C_Str();
	::memcpy(&pBone->matTransform, &pNode->mTransformation, sizeof(Matrix));
	pBone->matTransform = pBone->matTransform.Transpose();
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

	Matrix matPreTransform = m_matPreTransform;
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
				::memcpy(&pCurrentMesh->vecVertices[v].vPosition, &pAiMesh->mVertices[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vNormal, &pAiMesh->mNormals[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vTangent, &pAiMesh->mTangents[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vBinormal, &pAiMesh->mBitangents[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vUV, &pAiMesh->mTextureCoords[0][v], sizeof(Vec2));

				pCurrentMesh->vecVertices[v].vPosition = Vec3::Transform(pCurrentMesh->vecVertices[v].vPosition, matPreTransform);
				pCurrentMesh->vecVertices[v].vNormal = Vec3::Transform(pCurrentMesh->vecVertices[v].vNormal, matPreTransform);
				pCurrentMesh->vecVertices[v].vTangent = Vec3::Transform(pCurrentMesh->vecVertices[v].vTangent, matPreTransform);
				pCurrentMesh->vecVertices[v].vBinormal = Vec3::Transform(pCurrentMesh->vecVertices[v].vBinormal, matPreTransform);
			}
		}
		else
		{
			for (_uint v = 0; v < iVertexCount; ++v)
			{
				::memcpy(&pCurrentMesh->vecVertices[v].vPosition, &pAiMesh->mVertices[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vNormal, &pAiMesh->mNormals[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vTangent, &pAiMesh->mTangents[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vBinormal, &pAiMesh->mBitangents[v], sizeof(Vec3));
				::memcpy(&pCurrentMesh->vecVertices[v].vUV, &pAiMesh->mTextureCoords[0][v], sizeof(Vec2));
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
			pCurrentMesh->vecOffsetMatrices[0] = Matrix::Identity;
			pCurrentMesh->vecAffectBoneIndices[0] = iSrcBoneIndex;
			continue;
		}

		Read_Default_AffectBoneData(iVertexCount, iAffectBoneCount, pAiMesh, pCurrentMesh);
	}
}

HRESULT CConverter::Create_AiScene(const string& strPath, _uint iFlag)
{
	iFlag |= aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_EmbedTextures;

	m_pScene = m_pImporter->Get_Assimp_Importer()->ReadFile(strPath, iFlag);

	if (m_pScene == nullptr)
	{
		MSG_BOX("CConverter::Create_AiScene, failed");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CConverter::Read_AnimationData()
{
	m_pAnimations.reserve(m_pScene->mNumAnimations);
	m_vecAnimNames.resize(m_pScene->mNumAnimations);
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

			Vec3		vScale{ 1.f, 1.f, 1.f };
			Vec4		vQuaternion{ 0.f, 0.f, 0.f, 1.f };
			Vec3		vTranslation{ 0.f, 0.f, 0.f };
			pChannel->vecKeyFrames.resize(pChannel->iKeyFrameCount);
			for (_uint k = 0; k < pChannel->iKeyFrameCount; ++k)
			{
				AS_KEYFRAME KeyFrame = {};

				if (pAiChannel->mNumScalingKeys > k)
				{
					::memcpy(&vScale, &pAiChannel->mScalingKeys[k].mValue, sizeof(Vec3));
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
					::memcpy(&vTranslation, &pAiChannel->mPositionKeys[k].mValue, sizeof(Vec3));
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

	return S_OK;
}


HRESULT CConverter::Export_ModelData()
{
	std::filesystem::path ModelfinalPath = { g_wszModelRelativePath };
	ModelfinalPath /= m_RelativeFolderPath;
	ModelfinalPath /= L"Model";

	Create_Folder(ModelfinalPath);

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
			pFileUtil->Write<Matrix>(pElement->matTransform);
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
					Vec3 p = pMesh->vecVertices[i].vPosition;
					XMUINT4 indices = pMesh->vecVertices[i].vBlendIndices;
					Vec4 weights = pMesh->vecVertices[i].vBlendWeights;

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
				pFileUtil->Write(&pElement->vecOffsetMatrices[0], sizeof(Matrix) * (_uint)pElement->vecOffsetMatrices.size());
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

	Create_Folder(MaterialfinalPath);

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

HRESULT CConverter::Export_AnimationData(const string& strAnimationName, size_t iIndex)
{
	std::filesystem::path AnimationfinalPath = { g_wszModelRelativePath };
	AnimationfinalPath /= m_RelativeFolderPath;
	AnimationfinalPath /= L"Animation";

	Create_Folder(AnimationfinalPath);

	AnimationfinalPath /= strAnimationName;
	AnimationfinalPath.replace_extension(g_wszAnimationExtension);

	CFileUtils* pFileUtil = CFileUtils::Create();
	pFileUtil->Open(AnimationfinalPath, FileMode::WRITE);

	AS_ANIMATION* pAnimation = m_pAnimations[iIndex];

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
			pFileUtil->Write<Vec3>(KeyFrame.vScale);
			pFileUtil->Write<Vec4>(KeyFrame.vQuaternion);
			pFileUtil->Write<Vec3>(KeyFrame.vTranslation);
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
		Create_Folder((std::filesystem::path(szSaveFolder)));

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

HRESULT CConverter::ReadAndExport_NoAnimation()
{
	// m_pBones.reserve(m_pScene->mNumSkeletons);
	// Read_Bones(m_pScene->mRootNode, -1, -1);
	Read_Meshes();
	Read_MaterialData();

	if (FAILED(Export_ModelData()))
		return E_FAIL;

	if (FAILED(Export_MaterialData()))
		return E_FAIL;

	return S_OK;
}

HRESULT CConverter::ReadAndExport_MoreThanOne()
{
	for (size_t i = 0; i < m_vecAssetPaths.size(); ++i)
	{
		Clear();
		
		if (FAILED(Create_AiScene(Engine_Utils::ToString(m_vecAssetPaths[i]))))
			return E_FAIL;

		m_wstrAssetName = m_vecAssetPaths[i].filename().lexically_normal().stem();

		Read_Meshes();
		Read_MaterialData();

		if (FAILED(Export_ModelData()))
			return E_FAIL;

		if (FAILED(Export_MaterialData()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CConverter::ReadAndExport_Animation()
{
	m_pBones.reserve(m_pScene->mNumSkeletons);
	Read_Bones(m_pScene->mRootNode, -1, -1);
	Read_Meshes();
	Read_MaterialData();
	
	if (FAILED(Read_AnimationData()))
		return E_FAIL;

	if (FAILED(Export_ModelData()))
		return E_FAIL;

	if (FAILED(Export_MaterialData()))
		return E_FAIL;

	for (size_t i = 0; i < m_pAnimations.size(); ++i)
	{
		if (FAILED(Export_AnimationData(m_pAnimations[i]->strName, i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CConverter::Check_Folder()
{
	m_iFileCount = Get_FileCount(m_AssetParentPath);
	m_iFolderCount = Get_FolderCount(m_AssetParentPath);

	if (m_iFolderCount > 0)
	{
		MSG_BOX("CConverter::Ready_Folder, wrong folder.. has too many folders");
		return E_FAIL;
	}

	if (m_iFileCount <= 0)
	{
		MSG_BOX("CConverter::Ready_Folder, empty folder");
		return E_FAIL;
	}
	else if (m_iFileCount == 1)
	{
		for (const auto& entry : std::filesystem::directory_iterator(m_AssetParentPath))
		{
			if (entry.is_regular_file())
			{
				m_wstrAssetName = entry.path().filename().lexically_normal().stem();
				break;
			}
		}

		path assetFilePath = m_AssetParentPath / m_wstrAssetName;
		assetFilePath.replace_extension(g_wszModelExtension);
		if (FAILED(Create_AiScene(assetFilePath.string())))
			return E_FAIL;

		if (FAILED(Check_Type()))
			return E_FAIL;
	}
	else
	{
		m_eType = ConvertType::NONANIM_MORETHANONE;
		m_vecAssetPaths.resize(m_iFileCount);
	}

	return S_OK;
}

HRESULT CConverter::Check_Type()
{
	m_eType = ConvertType::NONANIM;

	if (m_pScene->HasAnimations())
		m_eType = ConvertType::ANIM;

	if (m_pScene->HasMeshes() == false)
		return E_FAIL;

	return S_OK;
}

HRESULT CConverter::Is_SubPath(const path &baseAbsPath, const path& targetAbsPath)
{
	if (baseAbsPath.empty() || targetAbsPath.empty())
		return E_FAIL;

	if (baseAbsPath.has_root_name() && targetAbsPath.has_root_name() &&
		(baseAbsPath.has_root_name() != targetAbsPath.has_root_name()))
		return E_FAIL;

	path relative = targetAbsPath.lexically_relative(baseAbsPath);
	if (relative.empty())
		return E_FAIL;

	for (const auto& pathPart : relative)
	{
		if (pathPart == "..")
			return E_FAIL;
	}

	return S_OK;
}

void CConverter::Create_Folder(path destPath)
{
	destPath = destPath.lexically_normal();
	if (std::filesystem::exists(destPath))
		return;

	std::filesystem::create_directories(destPath);
}

size_t CConverter::Get_FileCount(const wstring wstrFolderPath)
{
	size_t iFileCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(wstrFolderPath))
	{
		if (entry.is_regular_file())
		{
			m_vecAssetPaths.push_back(entry.path());
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

void CConverter::Clear()
{
	m_pImporter->Get_Assimp_Importer()->FreeScene();

	for (AS_BONE* pElement : m_pBones)
		Safe_Delete(pElement);

	for (AS_MATERIAL* pElement : m_pMaterials)
		Safe_Delete(pElement);

	for (AS_MESH* pElement : m_pMeshes)
		Safe_Delete(pElement);

	for (AS_ANIMATION* pElement : m_pAnimations)
	{
		for (AS_CHANNEL* pData : pElement->vecChannels)
		{
			Safe_Delete(pData);
		}
		pElement->vecChannels.clear();
		Safe_Delete(pElement);
	}

	m_pBones.clear();
	m_pMaterials.clear();
	m_pMeshes.clear();
	m_pAnimations.clear();
}

CConverter* CConverter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* szSolutionFullPath, const _tchar* wszAssetParentFolderName, const Matrix &matPreTransform)
{
	CConverter* pInstance = new CConverter(pDevice, pDeviceContext, matPreTransform);

	if (FAILED(pInstance->Initialize(wszAssetParentFolderName, szSolutionFullPath)))
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
		pCurrentMesh->vecOffsetMatrices[i] = Matrix::Identity;

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

		::memcpy(&pCurrentMesh->vecOffsetMatrices[b], &pAiBone->mOffsetMatrix, sizeof(Matrix));
		pCurrentMesh->vecOffsetMatrices[b] = pCurrentMesh->vecOffsetMatrices[b].Transpose();

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

void CConverter::Free()
{
	Clear();

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