#include "Engine_pch.h"
#include "ModelLoader.h"
#include "ModelAnimation.h"
#include "FileUtils.h"
#include "AsTypes.h"
#include "TextureBase.h"
#include "Engine_Utils.h"
#include <fstream>
#include <regex>
#include "Channel.h"
#include "Bone.h"
#include "Mesh.h"
//#include "Model.h"
#include "Material.h"
#include "GameInstance.h"

CModelLoader::CModelLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _bool bCustom)
    : m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_bCustom(bCustom)
	, m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

CModelLoader* CModelLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* wszModelFolderName, _bool bCustom)
{
    CModelLoader* pInstance = new CModelLoader(pDevice, pDeviceContext, bCustom);

    if (FAILED(pInstance->Initialize(wszModelFolderName)))
    {
        MSG_BOX("CModelLoader::Create, Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

HRESULT CModelLoader::Initialize(const _tchar* wszModelFolderName)
{
	if (!wszModelFolderName)
		return E_FAIL;

	std::filesystem::path in(wszModelFolderName);
	// "..", "." 정리
	in = in.lexically_normal();

	// 경로가 슬래시로 끝나면 filename()이 비어 있을 수 있으니 보정
	if (in.filename().native().empty())
		in = in.parent_path();

	m_ModelPath = g_wszModelRelativePath;
	// 확장자가 포함된 파일의 경우
	if (in.has_extension())
	{
		m_wstrModelName = in.stem().native();
		m_ModelPath /= in.parent_path().parent_path();
	}
	// 순수 폴더 경로일 경우
	else
	{
		m_wstrModelName = in.filename().native();
		m_ModelPath /= wszModelFolderName;
	}
    return S_OK;
}

HRESULT CModelLoader::Read_Material(vector<CMaterial*>* vecMaterials)
{
	if (!std::filesystem::exists(m_ModelPath / L"Material"))
	{
		MSG_BOX("CModelLoader::Read_Material, Folder is invalid");
		return E_FAIL;
	}
	
	if (!vecMaterials)
		return E_FAIL;

	std::filesystem::path finalPath = m_ModelPath / L"Material" / m_wstrModelName;
	finalPath.replace_extension(g_wszMaterialExtension);

	std::ifstream ifs(finalPath, std::ios::in | std::ios::binary);

	if (!ifs.is_open())
	{
		MSG_BOX("CModel::Read_Material, File open failed");
		return E_FAIL;
	}

	std::filesystem::path parentPath = finalPath.parent_path();

	json jArray;
	ifs >> jArray;
	vecMaterials->reserve(jArray.size());
	for (const auto& jObj : jArray)
	{
 		AS_MATERIAL asMaterial = jObj.get<AS_MATERIAL>();
		CMaterial::MATERIAL_DESC MaterialDesc = {};
		CMaterial* pMaterial = nullptr;
		MaterialDesc.wstrName = Engine_Utils::ToWString(asMaterial.strName);
		if (pMaterial = m_pGameInstance->Get_Resource<CMaterial>(MaterialDesc.wstrName))
		{
			vecMaterials->push_back(pMaterial);
			continue;
		}

		{
			MaterialDesc.spanTags = std::span<string>(asMaterial.tags.vecFiles.data(), asMaterial.tags.vecFiles.size());
			for (_uint i = 0; i < ENUM_TO_UINT(EMaterialTextureType::MAX_COUNT); ++i)
			{
				if (MaterialDesc.spanTags[i].empty() == false)
				{
					wstring wstrTag = Engine_Utils::ToWString(MaterialDesc.spanTags[i]);

					CTextureBase::RESOURCE_BASE_DESC srcDesc = {};
					srcDesc.wstrName = wstrTag;
					srcDesc.wstrPath = parentPath / wstrTag;
					CTextureBase* pReturn = m_pGameInstance->GetOrAddTexture(srcDesc.wstrName, &srcDesc);
					Safe_Release(pReturn);
				}
			}
		}

		//MaterialDesc.vDiffuse = asMaterial.vDiffuse;
		//MaterialDesc.vAmbient = asMaterial.vAmbient;
		//MaterialDesc.vSpecular = asMaterial.vSpecular;
		//MaterialDesc.vEmissive = asMaterial.vEmissive;

		pMaterial = CMaterial::Create(m_pDevice, m_pDeviceContext, &MaterialDesc);
		if (FAILED(m_pGameInstance->Add_Resource(MaterialDesc.wstrName, pMaterial)))
		{
			MSG_BOX("CModel::Read_Material, Add resource failed");
			ifs.close();
			return E_FAIL;
		}
		Safe_AddRef(pMaterial);
		vecMaterials->push_back(pMaterial);
	}

	ifs.close();
	return S_OK;
}

HRESULT CModelLoader::Read_Model(EModelType eType, vector<CBone*>* vecBones, vector<CMesh*>* vecMeshes, CModel::DATA_ANIMCHANNEL* pData)
{
	if (!std::filesystem::exists(m_ModelPath / L"Model"))
	{
		MSG_BOX("CModelLoader::Read_Model, Folder is invalid");
		return E_FAIL;
	}

	if (vecBones)
	{
		CFileUtils* pFileUtil = CFileUtils::Create();
		std::filesystem::path bonePath = m_ModelPath / L"Model" / m_wstrModelName;
		bonePath.replace_extension(g_wszBoneExtension);

		if (FAILED(pFileUtil->Open(bonePath, FileMode::READ)))
		{
			MSG_BOX("CModel::Read_Model, Bonefile open failed");
			return E_FAIL;
		}

		_uint iBoneCount = pFileUtil->Read<_uint>();
		vecBones->reserve(iBoneCount);
		CBone::BONE_DESC boneDesc = {};
		for (_uint i = 0; i < iBoneCount; ++i)
		{
			boneDesc.iIndex = pFileUtil->Read<_uint>();
			boneDesc.strName = pFileUtil->Read<string>();
			boneDesc.iParentIndex = pFileUtil->Read<_uint>();
			boneDesc.matTransform = pFileUtil->Read<Matrix>();

			if (pData)
			{
				boneDesc.iRootMotionBoneIndex = pData->iRootBoneIndex;
			}

				CBone * pNewbone = CBone::Create(&boneDesc);
			if (!pNewbone)
			{
				Safe_Release(pFileUtil);
				return E_FAIL;
			}
			vecBones->push_back(pNewbone);
		}

		if (vecBones->size() != iBoneCount)
		{
			MSG_BOX("CModelLoader::Read_Model, Bonecounting is not match");
			Safe_Release(pFileUtil);
			return E_FAIL;
		}

		Safe_Release(pFileUtil);
	}

	if (vecMeshes)
	{
		CFileUtils* pFileUtil = CFileUtils::Create();
		std::filesystem::path meshPath = m_ModelPath / L"Model" / m_wstrModelName;
		meshPath.replace_extension(g_wszMeshExtension);

		// TODO - Mesh 중복 체크?
		wstring wstrMeshTag = L"Prototype_Component_Mesh_";
		wstrMeshTag += m_wstrModelName;

		if (FAILED(pFileUtil->Open(meshPath, FileMode::READ)))
		{
			MSG_BOX("CModel::Read_Model, Meshfile open failed");
			return E_FAIL;
		}

		_uint iMeshCount = pFileUtil->Read<_uint>();
		vecMeshes->reserve(iMeshCount);
		for (_uint i = 0; i < iMeshCount; ++i)
		{
			CMesh::MESH_ORIGIN_DESC meshDesc = {};
			meshDesc.strName = pFileUtil->Read<string>();
			meshDesc.iMaterialIndex = pFileUtil->Read<_uint>();

			// Vertices
			meshDesc.iVertexCount = pFileUtil->Read<_uint>();
			vector<VTXANIMMESH> vecImportVertices(meshDesc.iVertexCount);
			if (meshDesc.iVertexCount > 0)
			{
				void* pVertices = vecImportVertices.data();
				pFileUtil->Read(&pVertices, sizeof(VTXANIMMESH) * meshDesc.iVertexCount);
				meshDesc.spanVertex = std::span<VTXANIMMESH>(vecImportVertices.data(), vecImportVertices.size());
			}

			// Indices
			meshDesc.iIndexCount = pFileUtil->Read<_uint>();
			vector<_uint> vecImportIndices(meshDesc.iIndexCount);
			if (meshDesc.iIndexCount > 0)
			{
				void* pIndices = vecImportIndices.data();
				pFileUtil->Read(&pIndices, sizeof(_uint) * meshDesc.iIndexCount);
				meshDesc.spanIndex = std::span<_uint>(vecImportIndices.data(), vecImportIndices.size());
			}

			// AffectBones
			meshDesc.iAffectBoneCount = pFileUtil->Read<_uint>();
			vector<_uint> vecImportAffectBoneIndices(meshDesc.iAffectBoneCount);
			if (meshDesc.iAffectBoneCount > 0)
			{
				void* pAffectBoneIndices = vecImportAffectBoneIndices.data();
				pFileUtil->Read(&pAffectBoneIndices, sizeof(_uint) * meshDesc.iAffectBoneCount);
				meshDesc.spanAffectBoneIndex = std::span<_uint>(vecImportAffectBoneIndices.data(), vecImportAffectBoneIndices.size());
			}

			// OffsetMatrix
			meshDesc.iOffsetMatricesCount = pFileUtil->Read<_uint>();
			vector<Matrix> vecImportOffsetMatrices(meshDesc.iOffsetMatricesCount);
			if (meshDesc.iOffsetMatricesCount > 0)
			{
				void* pOffsetMatrices(vecImportOffsetMatrices.data());
				pFileUtil->Read(&pOffsetMatrices, sizeof(Matrix) * meshDesc.iOffsetMatricesCount);
				meshDesc.spanOffsetMatrices = std::span<Matrix>(vecImportOffsetMatrices.data(), vecImportOffsetMatrices.size());
			}

			// MinMax
			meshDesc.iMinMaxCount = pFileUtil->Read<_uint>();
			vector<Vec3> vecImportMinMax(meshDesc.iMinMaxCount);
			if (meshDesc.iMinMaxCount > 0)
			{
				void* pMinMax(vecImportMinMax.data());
				pFileUtil->Read(&pMinMax, sizeof(Vec3)* meshDesc.iMinMaxCount);
				meshDesc.spanMinMax = std::span<Vec3>(vecImportMinMax.data(), vecImportMinMax.size());
			}


			if (m_wstrModelName.ends_with(L"_COL") || m_wstrModelName.starts_with(L"COL_"))
				meshDesc.bSaveNormal = true;

			meshDesc.eModelType = eType;
			CMesh* pNewMesh = CMesh::Create(m_pDevice, m_pDeviceContext, &meshDesc);
			if (!pNewMesh)
			{
				Safe_Release(pFileUtil);
				return E_FAIL;
			}

			vecMeshes->push_back(pNewMesh);
		}

		if (vecMeshes->size() != iMeshCount)
		{
			MSG_BOX("CModelLoader::Read_Model, Meshcounting is not match");
			Safe_Release(pFileUtil);
			return E_FAIL;
		}

		Safe_Release(pFileUtil);
	}

	return S_OK;
}

HRESULT CModelLoader::Read_Animation(vector<CModelAnimation*>* vecAnimations, CModel::DATA_ANIMCHANNEL* pData)
{
	std::filesystem::path AnimationFolerPath = m_ModelPath / L"Animation";
	if (!std::filesystem::exists(AnimationFolerPath))
	{
		MSG_BOX("CModelLoader::Read_Animation, Folder is invalid");
		return E_FAIL;
	}

	if (!vecAnimations)
		return E_FAIL;

	std::filesystem::directory_iterator dirItr_Begin{ AnimationFolerPath };
	std::filesystem::directory_iterator dirItr_End{};

	size_t iFileCount = std::count_if(dirItr_Begin, dirItr_End,
		[](const std::filesystem::directory_entry& entry)->bool
		{
			return entry.is_regular_file();
		});

	vecAnimations->reserve(iFileCount);
	dirItr_Begin = std::filesystem::directory_iterator{ AnimationFolerPath };

	std::filesystem::path AnimationClipPath;
	// 폴더 내 파일 순회하면서 clip 파일 읽기
	for (auto dirItr = dirItr_Begin; dirItr != dirItr_End; ++dirItr)
	{
		AnimationClipPath = dirItr->path();
		CModelAnimation::MODELANIM_DESC desc = {};
		CModelAnimation* pNewAnimation = { nullptr };
		CFileUtils* pFileUtil = CFileUtils::Create();
		if (FAILED(pFileUtil->Open(AnimationClipPath, FileMode::READ)))
		{
			MSG_BOX("CModel::Read_Animation, Clipfile open failed");
			return E_FAIL;
		}

		wstring wstrAnimationame = AnimationClipPath.stem();
		wstring wstrResourceTag = L"Animation_" + m_wstrModelName + L"_" + wstrAnimationame;
		if (pNewAnimation = m_pGameInstance->Get_Resource<CModelAnimation>(wstrResourceTag))
		{
			vecAnimations->push_back(pNewAnimation);
			Safe_Release(pFileUtil);
			continue;
		}

		desc.wstrName = wstrAnimationame;
		desc.wstrPath = AnimationClipPath;
		const string aiAnimationName = pFileUtil->Read<string>();
		desc.fDuration = pFileUtil->Read<_float>();
		desc.fTickPerSecond = pFileUtil->Read<_float>();
		desc.iChannelCount = pFileUtil->Read<_uint>();
		if (pData)
		{
			desc.bRootAni	= pData->bRootAni;
			desc.bMixAni	= pData->bMixAni;
		}

		vector<CChannel*> vecChannels;
		vecChannels.reserve(desc.iChannelCount);
		if (FAILED(Create_Channel(pFileUtil, desc.iChannelCount, &vecChannels, pData)))
		{
			Safe_Release(pFileUtil);
			return E_FAIL;
		}
		std::span<CChannel*> spanChannels(vecChannels.data(), vecChannels.size());
		desc.spanChannels = spanChannels;
		pNewAnimation = CModelAnimation::Create(m_pDevice, m_pDeviceContext, &desc);
		if (!pNewAnimation)
		{
			Safe_Release(pFileUtil);
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_Resource(wstrResourceTag, pNewAnimation)))
		{
			Safe_Release(pFileUtil);
			return E_FAIL;
		}


		Safe_AddRef(pNewAnimation);
		vecAnimations->push_back(pNewAnimation);
		Safe_Release(pFileUtil);
	}

	return S_OK;
}

HRESULT CModelLoader::Create_Channel(CFileUtils* pFileUtil, _uint iChannelCount, vector<CChannel*>* vecChannels, CModel::DATA_ANIMCHANNEL* pData)
{
	if (!pFileUtil || !vecChannels || iChannelCount <= 0)
		return E_FAIL;

	for (size_t i = 0; i < iChannelCount; ++i)
	{
		CChannel::CHANNEL_DESC desc = {};
		desc.strName = pFileUtil->Read<string>();
		desc.iBoneIndex = pFileUtil->Read<_int>();
		_uint iKeyFramesCount = pFileUtil->Read<_uint>();
		if (iKeyFramesCount <= 0)
			return E_FAIL;
		vector<KEYFRAME> vecKeyframes;
		vecKeyframes.reserve(iKeyFramesCount);
		for (_uint i = 0; i < iKeyFramesCount; ++i)
		{
			KEYFRAME keyframe = {};
			keyframe.fTrackPosition = pFileUtil->Read<_float>();
			keyframe.vScale = pFileUtil->Read<Vec3>();
			keyframe.vQuaterion = pFileUtil->Read<Vec4>();
			keyframe.vTranslation = pFileUtil->Read<Vec3>();
			vecKeyframes.push_back(keyframe);
		}
		std::span<KEYFRAME> spanKeyframes(vecKeyframes.data(), vecKeyframes.size());
		desc.spanKeyframes = spanKeyframes;

		if (pData)
		{
			desc.iRootBoneIndex = pData->iRootBoneIndex;
		}

		CChannel* pNewChannel = CChannel::Create(desc);
		if (!pNewChannel)
			return E_FAIL;
		vecChannels->push_back(pNewChannel);
	}

	return S_OK;
}

void CModelLoader::Free()
{
	Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
    Super::Free();
}
