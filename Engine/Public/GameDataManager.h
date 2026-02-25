#pragma once
#include "Base.h"
#include "GameData_Struct.h"
#include <string>

NS_BEGIN(Engine)

class CShader;

class CGameDataManager final : public CBase
{
	using Super = CBase;
private:
	CGameDataManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CGameDataManager() = default;

	HRESULT Initialize();
public:
	static ID3D11ShaderResourceView* Make_ShaderResourceViewColor(_uint A, _uint R, _uint G, _uint B,ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:

#pragma region Texture Splating Info Datas

public:

	/* Engine 단위에서는 단순하게 Data들을 들고만 있는다 Json 파일 관리는 MapToolManager에서*/
	HRESULT		Load_TextureSplatingInfoData();

	/* 이름으로 Binding 하는 함수 */
	HRESULT		Bind_SplatingTextureInfo(CShader* pBindShader , const wstring& wstrTextureSplatingInfoDataName);
	HRESULT		Bind_MapTexture(CShader* pBindShader , const wstring& wstrTextureSplatingInfoDataName);
	HRESULT		Bind_Mix_RGBA_Info(CShader* pBindShader , const wstring& wstrTextureSplatingInfoDataName);
	HRESULT		Bind_Mix_RGBA_Texture(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName);
	HRESULT		Bind_Mix_RGBA_Data_And_Count(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName);
#pragma endregion

#pragma region ATTACK_PRESET
	const DTO::TAttackPreset_Data* Find_AttackPrseet(_uint iPresetKey) const;
	const DTO::TAttackPreset_Data* Find_AttackPresetByTag(const string& strTag) const;
	HRESULT Upsert_AttackPresetData(const DTO::TAttackPreset_Data& inData);
	const unordered_map<_uint, DTO::TAttackPreset_Data>& Get_AttackPresetsData_ForDebug() const { return m_umapAttackPresetDatas; }
#pragma endregion
public:
	void Clear_AttackPreset();
private:
	map<wstring, TEXTURE_SPLATTING_INFO >				m_mapTextureSplatingInfoDatas{};
	const _tchar*										m_wszTextureSplatingInfoDataPath = L"../../Resources/Data/MapData/TextureSplatingInfoData.json";
	ID3D11ShaderResourceView*							m_pDefaultBlack{nullptr};
	ID3D11ShaderResourceView*							m_pDefaultWhite{nullptr};
private:

	ID3D11Device*										m_pDevice{nullptr};
	ID3D11DeviceContext*								m_pDeviceContext{nullptr};
	////////////////////
	/// AttackPreset ///
	////////////////////
	_bool m_bAttackPresetLoaded{ false };
	unordered_map<_uint, DTO::TAttackPreset_Data> m_umapAttackPresetDatas;
	unordered_map<string, _uint> m_umapAttackPresetTagToKey;
	class CGameInstance *								m_pGameInstance = { nullptr };
public:
	static CGameDataManager* Create(ID3D11Device* pDevice , ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END