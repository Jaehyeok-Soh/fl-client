#include "pch.h"
#include "Level_Tutorial_Boss.h"

//=================
// Builder
//=================
#include "Builder_UI.h"
#include "DataStruct_UI.h"

CLevel_Tutorial_Boss::CLevel_Tutorial_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice , pDeviceContext)
{
}

HRESULT CLevel_Tutorial_Boss::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Build_Prototype()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Build_Prototype Create Failed");
		return E_FAIL;
	}

	if (FAILED(Build_Files()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Build_Files Create Failed");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;



	return S_OK;
}

void CLevel_Tutorial_Boss::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);

}

HRESULT CLevel_Tutorial_Boss::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Build_Prototype()
{
	if (FAILED(Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::TUTORIAL_VILLAGE)))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Build_Files()
{
	ELevelType eLevelType = ELevelType::TUTORIAL_VILLAGE;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);
	DTO::ECategory eCategory = DTO::ECategory::UI;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;

	_wstring strUIFolderPath = L"../../Resources/Data/UIData/Static/";
	if (std::filesystem::exists(strUIFolderPath))
	{
		for (auto iter : std::filesystem::directory_iterator(strUIFolderPath))
		{
			if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
				return E_FAIL;

			if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
				return E_FAIL;
		}
	}
	return S_OK;
}

CLevel_Tutorial_Boss* CLevel_Tutorial_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Tutorial_Boss* pInstance = new CLevel_Tutorial_Boss(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tutorial_Boss::Free()
{
	Super::Free();
}