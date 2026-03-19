#include "Engine_pch.h"
#include "Cinematic_Manager.h"

#include <fstream>

#include "GameInstance.h"

CCinematic_Manager::CCinematic_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice{ pDevice }
	, m_pDeviceContext{ pDeviceContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_mapCinematicCameraSequence{}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}


HRESULT CCinematic_Manager::Initialize()
{

	return S_OK;
}


HRESULT CCinematic_Manager::Load_CameraCinematicSequence(const _tchar* wszCameraCinematicDataJsonPath)
{
	std::ifstream ifs(wszCameraCinematicDataJsonPath);

	if (ifs.is_open() == false) return E_FAIL;

	/* json 파일 안에 내용이 있는지 없는지를 검사 */
	if (ifs.peek() == std::ifstream::traits_type::eof())
		return S_OK;

	/* json 파일 안에 내용이 있다면 Load */
	nlohmann::json LoadJson{};
	ifs >> LoadJson;

	/* 혹시 모르니 한번더 검사 */
	if (LoadJson.empty())
		return S_OK;

	m_mapCinematicCameraSequence.clear();

	/* Key 값을 가지고 Load된다 */
	for (const auto& item : LoadJson.items())
	{
		if (item.key().empty())
			continue;
		if (item.value().is_null())
			continue;

		CinematicCameraSequence tData(m_pDevice, m_pDeviceContext);
		wstring wstrKey = Engine_Utils::ToWString(item.key());

		tData.Load_Json(item.value());
		tData.strName = item.key();
		m_mapCinematicCameraSequence.emplace(wstrKey, tData);
	}

	return S_OK;
}

HRESULT CCinematic_Manager::Save_CameraCinematicSequence(const _tchar* wszCameraCinematicDataJsonPath)
{
	if (m_mapCinematicCameraSequence.empty())
		return S_OK;

	std::ofstream ofs(wszCameraCinematicDataJsonPath);

	/* json 파일 안에 내용이 있다면 Load */
	nlohmann::json SaveJson{};

	for (auto& Pair : this->m_mapCinematicCameraSequence)
	{
		auto& Key_SaveJson = SaveJson[Engine_Utils::ToString(Pair.first)];
		Pair.second.Save_Json(Key_SaveJson);
	}

	ofs << SaveJson.dump(4);

	ofs.close();

	return S_OK;
}

HRESULT CCinematic_Manager::Load_CameraCinematicSequence(const wstring& wstrFindKey, OUT CinematicCameraSequence* pOutCamCinematicSequence)
{
	/* 받아갈 데이터를 반드시 집어넣어줘야한다 */
	if (pOutCamCinematicSequence == nullptr)					return E_FAIL;
	auto iter = m_mapCinematicCameraSequence.find(wstrFindKey);
	if (iter == m_mapCinematicCameraSequence.end())				return E_FAIL;

	if (IDOK == MessageBox(NULL, wstring(wstrFindKey + L" 데이터를 복사 하시겠습니까? ").c_str(), L"경고: 파일 원본이 아닌 복사본입니다", MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND))
	{
		*pOutCamCinematicSequence = iter->second;
	}

	return S_OK;
}

HRESULT CCinematic_Manager::Save_CameraCinematicSequence(const wstring& wstrFindKey, const CinematicCameraSequence* pSaveCamCinematicSequence)
{
	/* 받아갈 데이터를 반드시 집어넣어줘야한다 */
	if (pSaveCamCinematicSequence == nullptr)					return E_FAIL;
	auto iter = m_mapCinematicCameraSequence.find(wstrFindKey);

	/* 데이터가 이미 있는 경우 */
	if (iter != m_mapCinematicCameraSequence.end())
	{
		int iResult = MessageBox(NULL, L"이미 저장된 파일이 존재합니다 덮어쓰시겠습니까?", L"경고: 기존 저장파일 사라짐", MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND);
		if (iResult == IDOK)
			m_mapCinematicCameraSequence.erase(wstrFindKey);
		else
			return S_OK;
	}
	/* 데이터가 없는 경우 */
	else
	{
		int iResult = MessageBox(NULL, L"데이터를 저장하시겠습니까?", nullptr, MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND);
		if (iResult != IDOK)
			return S_OK;
	}

	m_mapCinematicCameraSequence.emplace(wstrFindKey, *pSaveCamCinematicSequence);


	return S_OK;
}


HRESULT CCinematic_Manager::Play_CameraCinematic(const wstring& wstrFindKey)
{
	const auto& iter = m_mapCinematicCameraSequence.find(wstrFindKey);
	if (iter == m_mapCinematicCameraSequence.end()) return E_FAIL;

	/* Play Cinematic */
	m_pGameInstance->Play_CameraCinematic(&iter->second);

	return S_OK;
}

vector<string> CCinematic_Manager::Get_CameraCinematicSequenceNames() const
{
	vector<string> vecCamCinematicSequenceNames{};

	_uint iCount = static_cast<_uint>(m_mapCinematicCameraSequence.size());
	vecCamCinematicSequenceNames.reserve(iCount);

	for (auto& CamCinematicSequence : m_mapCinematicCameraSequence)
		vecCamCinematicSequenceNames.push_back(Engine_Utils::ToString(CamCinematicSequence.first));

	return vecCamCinematicSequenceNames;
}


CCinematic_Manager* CCinematic_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCinematic_Manager* pMgr = new CCinematic_Manager(pDevice, pDeviceContext);

	if (FAILED(pMgr->Initialize()))
	{
		Safe_Release(pMgr);
		MSG_BOX(" Cinematic Manager is failed to Create ");
		return nullptr;
	}

	return pMgr;
}

void CCinematic_Manager::Free()
{
	Super::Free();

	m_mapCinematicCameraSequence.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);

}
