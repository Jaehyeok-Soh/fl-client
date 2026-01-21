#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CModel;
class CBone;
class CModelAnimation;
class CChannel;
class CMesh;
class CMaterial;
class CGameInstance;

typedef struct tagBatchData MODEL_BATCH_DATA;

class ENGINE_DLL CModelLoader final : public CBase
{
	using Super = CBase;
private:
	CModelLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _bool bCustom);
	virtual ~CModelLoader() = default;

	HRESULT Initialize(const _tchar* wszModelFolderName);
public:
	HRESULT Read_Material(vector<CMaterial*>* vecMaterials);
	HRESULT Read_Model(EModelType eType, vector<CBone*>* vecBones, vector<CMesh*>* vecMeshes);
	HRESULT Read_Animation(vector<CModelAnimation*>* vecAnimations);
private:
	HRESULT Create_Channel(class CFileUtils* pFileUtil, _uint iChannelCount,vector<CChannel*>* vecChannels);
private:
	std::filesystem::path m_ModelPath;
	wstring m_wstrModelName = L"";
	_bool m_bCustom = { false };
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
public:
	static CModelLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* wszModelFolderName, _bool bCustom = false);
	virtual void Free() override;
};

NS_END