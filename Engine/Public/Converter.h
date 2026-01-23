#pragma once
#include "Base.h"
#include "AsTypes.h"

NS_BEGIN(Engine)

// Animation파일 양식
// Tag_AnimationName.fbx
typedef struct tagAnimationFilenameParts
{
	std::filesystem::path filePath;
	wstring wstrTag = L"";
	wstring wstrAnimationName = L"";

	tagAnimationFilenameParts() {}
	tagAnimationFilenameParts(const path& path_in) : filePath(path_in) {}
}ANIMPATHPART;

enum class ConvertType : unsigned int
{
	NORMAL = 0,
	ANIM,
	MULTINORMALS,
	END
};

class CImporter;

class ENGINE_DLL CConverter final : public CBase
{
	using Super = CBase;
private:
	CConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const Matrix &matPreTransform, _bool bCustom);
	virtual ~CConverter() = default;
public:
	HRESULT Initialize(const _tchar* wszAssetParentFolderName);
	HRESULT ReadAndExportFile();
	void Clear();
	void Clear_For_Custom();
	vector<AS_BONE*> Get_MasterBone()
	{
		vector<AS_BONE*> vecReturn;
		vecReturn.reserve(m_pBones.size());
		for (auto& pBone : m_pBones)
			vecReturn.push_back((pBone->Clone()));
		return vecReturn;
	};
	void Set_MatserBone(const vector<AS_BONE*>& vecMasterBones)
	{
		m_pMasterBones = vecMasterBones;
	}
private:
	HRESULT Read_Folder();
	HRESULT ReadAndExport_Normal();
	HRESULT ReadAndExport_MapDatas();
	HRESULT ReadAndExport_Animations();
	void Read_Bones(aiNode* pNode, _int iIndex, _int iParent);
	void Read_MaterialData();
	void Read_Meshes();
	void Read_AnimationData();
	HRESULT Export_ModelData();
	HRESULT Export_MaterialData();
	HRESULT Export_AnimationData(_uint iIndex);
	string Write_Texture(const _char* szSaveFolder, const _char* szFile);
private:
	size_t Get_FileCount(const wstring wstrFolderPath);
	size_t Get_FolderCount(const wstring wstrFolderPath);
	void Read_Default_AffectBoneData(_uint iVertexCount, _uint iAffectBoneCount, const aiMesh* pAiMesh, AS_MESH* pCurrentMesh);
	void Read_ForMasterBone_AffectBoneData(_uint iVertexCount, _uint iAffectBoneCount, const aiMesh* pAiMesh, AS_MESH* pCurrentMesh);
	_int Get_BoneIndex(const _char* szName);
	_int Get_BoneIndexFormMatserBone(const _char* szName);
private:
	_bool m_bCustom = { false };
	ConvertType m_eType = { ConvertType::END };
	EModelType m_eModelType = { EModelType::NONANIM };
	wstring m_wstrAssetName = { L"" };
	path m_AssetParentPath;
	path m_RelativeFolderPath;
	CImporter* m_pImporter = { nullptr };
	const aiScene* m_pScene = { nullptr };

	Matrix m_matPreTransform = {};
	vector<AS_BONE*> m_pMasterBones;
	vector<AS_BONE*> m_pBones;
	vector<AS_MESH*> m_pMeshes;
	vector<AS_MATERIAL*> m_pMaterials;
	vector<AS_ANIMATION*> m_pAnimations;
	vector<ANIMPATHPART> m_vecAnimPaths;
	vector<path> m_vecMapAssetPaths;
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
public:
	static CConverter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* wszAssetParentFolderName, const Matrix& matPreTransform, _bool bCustom = false);
	virtual void Free();
};

NS_END