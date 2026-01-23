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
	NONANIM = 0,
	NONANIM_MORETHANONE,
	ANIM,
	END
};

class CImporter;

class ENGINE_DLL CConverter final : public CBase
{
	using Super = CBase;
private:
	CConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const Matrix &matPreTransform);
	virtual ~CConverter() = default;
public:
	HRESULT Initialize(const _tchar* wszAssetParentFolderName, const _char* szSolutionFullPath);
	HRESULT ReadAndExport();
	void Clear();
private:
	HRESULT ReadAndExport_NoAnimation();
	HRESULT ReadAndExport_MoreThanOne();
	HRESULT ReadAndExport_Animation();
	void Read_Bones(aiNode* pNode, _int iIndex, _int iParent);
	void Read_MaterialData();
	void Read_Meshes();
	HRESULT Create_AiScene(const string &strPath, _uint iFlag  = 0);
	HRESULT Read_AnimationData();
	HRESULT Export_ModelData();
	HRESULT Export_MaterialData();
	HRESULT Export_AnimationData(const string& strAnimationName, size_t iIndex);
	string Write_Texture(const _char* szSaveFolder, const _char* szFile);
private:
	HRESULT Check_Folder();
	HRESULT Check_Type();
	HRESULT Is_SubPath(const path& baseAbsPath, const path& targetAbsPath);
	void Create_Folder(path destPath);
	size_t Get_FileCount(const wstring wstrFolderPath);
	size_t Get_FolderCount(const wstring wstrFolderPath);
	void Read_Default_AffectBoneData(_uint iVertexCount, _uint iAffectBoneCount, const aiMesh* pAiMesh, AS_MESH* pCurrentMesh);
	_int Get_BoneIndex(const _char* szName);
private:
	size_t m_iFileCount = { 0 };
	size_t m_iFolderCount = { 0 };
	ConvertType m_eType = { ConvertType::END };
	wstring m_wstrFullPath = { L"" };
	wstring m_wstrAssetName = { L"" };
	path m_AssetParentPath;
	path m_RelativeFolderPath;
	CImporter* m_pImporter = { nullptr };
	const aiScene* m_pScene = { nullptr };

	Matrix m_matPreTransform = {};
	vector<AS_BONE*> m_pBones;
	vector<AS_MESH*> m_pMeshes;
	vector<AS_MATERIAL*> m_pMaterials;
	vector<AS_ANIMATION*> m_pAnimations;
	vector<path> m_vecAssetPaths;
	vector<wstring> m_vecAnimNames;
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
public:
	static CConverter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* szSolutionFullPath, const _tchar* wszAssetParentFolderName, const Matrix& matPreTransform);
	virtual void Free();
};

NS_END