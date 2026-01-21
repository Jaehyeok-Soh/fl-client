#include "Engine_Utils.h"
#include "GameInstance.h"
#include "Sound_Manager.h"

#define FMOD_CALL(x) do { FMOD_RESULT __r = (x); if (__r != FMOD_OK) { MSG_BOX("Failed, FMOD_CALL"); } } while(0)

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	FMOD_RESULT fmodResult = FMOD::System_Create(&m_pSystem);
	if (fmodResult != FMOD_OK || m_pSystem == nullptr)
		return E_FAIL;

	fmodResult = m_pSystem->init(MAXCHANNEL, FMOD_INIT_NORMAL, nullptr);
	if (fmodResult != FMOD_OK)
		return E_FAIL;

	for (_uint i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i] = nullptr;

	Reset_OneShotPool();
	return S_OK;
}

void CSound_Manager::Update()
{
	Reclaim_OneShots();

	if (m_pSystem)
		FMOD_CALL(m_pSystem->update());
}

HRESULT CSound_Manager::Load_Sounds(const std::wstring& wstrFolderPath)
{
	if (m_pSystem == nullptr)
		return E_FAIL;

	StopAll();

	std::filesystem::path folder{ wstrFolderPath };
	if (std::filesystem::exists(folder) == false || std::filesystem::is_directory(folder) == false)
		return E_FAIL;

	for (auto& Pair : m_umapSounds)
	{
		if (Pair.second)
			FMOD_CALL(Pair.second->release());
	}
	m_umapSounds.clear();

	for (const auto& entry : std::filesystem::directory_iterator(folder))
	{
		if (entry.is_regular_file() == false)
			continue;

		const std::filesystem::path entryPath = entry.path();
		if (entryPath.has_extension() == false)
			continue;

		wstring wstrExtension = entryPath.extension().wstring();
		std::transform(wstrExtension.begin(), wstrExtension.end(), wstrExtension.begin(), ::towlower);
		if (wstrExtension != L".wav")
			continue;

		string strPath = Engine_Utils::ToString(entryPath.wstring());

		wstring wstrKey = entryPath.stem().wstring();

		FMOD::Sound* pSound{ nullptr };
		FMOD_RESULT fmodResult = m_pSystem->createSound(strPath.c_str(), FMOD_DEFAULT, nullptr, &pSound);
		if (fmodResult != FMOD_OK || pSound == nullptr)
			continue;

		FMOD_CALL(pSound->setMode(FMOD_LOOP_OFF));

		m_umapSounds.emplace(std::move(wstrKey), pSound);
	}

	return S_OK;
}

void CSound_Manager::Play_Controlled(const _tchar* pSoundKey, _uint iControlledId, _float fVolume, _bool bLoop, _float fPitch)
{
	if (m_pSystem == nullptr || pSoundKey == nullptr)
		return;

	if (iControlledId >= CONTROLLED_COUNT)
		return;

	FMOD::Sound* pSound = FindSound(pSoundKey);
	if (pSound == nullptr)
		return;

	StopAndClearChannelSlot(iControlledId);

	FMOD::Channel* pChannel = { nullptr };

	FMOD_RESULT fmodResult = m_pSystem->playSound(pSound, nullptr, true, &pChannel);
	if (fmodResult != FMOD_OK || pChannel == nullptr)
		return;

	ApplyChannelParams(pChannel, fVolume, bLoop, fPitch);

	FMOD_CALL(pChannel->setPaused(false));
	m_pChannelArr[iControlledId] = pChannel;
}

void CSound_Manager::PlayBGM(const _tchar* pSoundKey, _float fVolume, _bool bLoop, _float fPitch)
{
	Play_Controlled(pSoundKey, 0, fVolume, bLoop, fPitch);
}

void CSound_Manager::Stop_Controlled(_uint iControlledId)
{
	if (iControlledId >= CONTROLLED_COUNT)
		return;

	StopAndClearChannelSlot(iControlledId);
}

void CSound_Manager::Set_ControlledVolume(_uint iControlledId, _float fVolume)
{
	if (iControlledId >= CONTROLLED_COUNT)
		return;

	FMOD::Channel* pChannel = m_pChannelArr[iControlledId];
	if (pChannel == nullptr)
		return;

	fVolume = std::clamp(fVolume, 0.f, 1.f);
	FMOD_CALL(pChannel->setVolume(fVolume));
}

void CSound_Manager::Set_ControlledPitch(_uint iControlledId, _float fPitch)
{
	if (iControlledId >= CONTROLLED_COUNT)
		return;

	FMOD::Channel* pChannel = m_pChannelArr[iControlledId];
	if (pChannel == nullptr)
		return;

	if (fPitch <= 0.f) fPitch = 0.01f;
	FMOD_CALL(pChannel->setPitch(fPitch));
}

void CSound_Manager::Play_OneShot(const _tchar* pSoundKey, _float fVolume, _float fPitch)
{
	if (!m_pSystem || !pSoundKey)
		return;

	FMOD::Sound* pSound = FindSound(pSoundKey);
	if (pSound == nullptr)
		return;

	if (m_vecOneShotStack.empty())
		return;

	_uint iTopIndex = m_vecOneShotStack.back();
	m_vecOneShotStack.pop_back();

	// 혹시라도 슬롯에 남아있는게 있으면 정리
	StopAndClearChannelSlot(iTopIndex);

	FMOD::Channel* pChannel = nullptr;
	FMOD_RESULT fmodResult = m_pSystem->playSound(pSound, nullptr, true, &pChannel);
	if (fmodResult != FMOD_OK || pChannel == nullptr)
	{
		m_vecOneShotStack.push_back(iTopIndex);
		return;
	}

	ApplyChannelParams(pChannel, fVolume, false, fPitch);
	FMOD_CALL(pChannel->setPaused(false));

	m_pChannelArr[iTopIndex] = pChannel;
	m_vecActiveOneShots.push_back(iTopIndex);
}

void CSound_Manager::Play_RandOneShot(const _tchar* pSoundKey, _float fVolume, _int iCount, _float fPitch)
{
	_int iRand = CGameInstance::GetInstance()->Rand_Int(1, iCount);
	wstring wstrKey = wstring(pSoundKey) + std::to_wstring(iRand);
	Play_OneShot(wstrKey.c_str(), fVolume, fPitch);
}

void CSound_Manager::StopSound(_uint iChannelIndex)
{
	if (iChannelIndex >= MAXCHANNEL)
		return;

	StopAndClearChannelSlot(iChannelIndex);

	// 원샷 영역이면 active에서 제거, free로 반환
	if (iChannelIndex >= ONE_SHOT_BEGIN && iChannelIndex < ONE_SHOT_END)
	{
		RemoveActiveOneShotIfExists(iChannelIndex);
		if (std::find(m_vecOneShotStack.begin(), m_vecOneShotStack.end(), iChannelIndex) == m_vecOneShotStack.end())
			m_vecOneShotStack.push_back(iChannelIndex);
	}
}

void CSound_Manager::StopAll()
{
	for (_uint i = 0; i < MAXCHANNEL; ++i)
		StopAndClearChannelSlot(i);

	// 원샷 풀 초기화
	Reset_OneShotPool();
}

void CSound_Manager::ApplyChannelParams(FMOD::Channel* pChannel, _float fVolume, _bool bLoop, _float fPitch)
{
	if (!pChannel)
		return;

	fVolume = std::clamp(fVolume, 0.f, 1.f);
	if (fPitch <= 0.f) fPitch = 0.01f;

	FMOD_CALL(pChannel->setVolume(fVolume));
	FMOD_CALL(pChannel->setPitch(fPitch));

	if (bLoop)
	{
		FMOD_CALL(pChannel->setMode(FMOD_LOOP_NORMAL));
		FMOD_CALL(pChannel->setLoopCount(-1));
	}
	else
	{
		FMOD_CALL(pChannel->setMode(FMOD_LOOP_OFF));
		FMOD_CALL(pChannel->setLoopCount(0));
	}
}

FMOD::Sound* CSound_Manager::FindSound(const std::wstring& wstrKey) const
{
	auto it = m_umapSounds.find(wstrKey);
	if (it == m_umapSounds.end())
		return nullptr;

	return it->second;
}

void CSound_Manager::Reset_OneShotPool()
{
	m_vecOneShotStack.clear();
	m_vecActiveOneShots.clear();

	m_vecOneShotStack.reserve(ONE_SHOT_END - ONE_SHOT_BEGIN);
	for (_uint i = ONE_SHOT_BEGIN; i < ONE_SHOT_END; ++i)
		m_vecOneShotStack.push_back(i);
}

void CSound_Manager::Reclaim_OneShots()
{
	for (size_t i = 0; i < m_vecActiveOneShots.size(); /*increment inside*/)
	{
		_uint iIndex = m_vecActiveOneShots[i];
		FMOD::Channel* pChannel = (iIndex < MAXCHANNEL) ? m_pChannelArr[iIndex] : nullptr;

		_bool bPlaying = false;

		if (pChannel)
		{
			FMOD_RESULT fmodResult = pChannel->isPlaying(&bPlaying);
			if (fmodResult != FMOD_OK) bPlaying = false;
		}

		if (pChannel == nullptr || bPlaying == false)
		{
			StopAndClearChannelSlot(iIndex);

			_uint iTopIndex = m_vecActiveOneShots.back();
			m_vecActiveOneShots[i] = iTopIndex;
			m_vecActiveOneShots.pop_back();

			m_vecOneShotStack.push_back(iIndex);
			continue;
		}

		++i;
	}
}

void CSound_Manager::StopAndClearChannelSlot(_uint iIndex)
{
	if (iIndex >= MAXCHANNEL)
		return;

	FMOD::Channel* pChannel = m_pChannelArr[iIndex];
	if (pChannel == nullptr)
		return;

	_bool bPlaying = { false };
	if (pChannel->isPlaying(&bPlaying) == FMOD_OK && bPlaying)
		FMOD_CALL(pChannel->stop());

	m_pChannelArr[iIndex] = nullptr;
}

void CSound_Manager::RemoveActiveOneShotIfExists(_uint iIndex)
{
	auto it = std::find(m_vecActiveOneShots.begin(), m_vecActiveOneShots.end(), iIndex);
	if (it == m_vecActiveOneShots.end())
		return;

	*it = m_vecActiveOneShots.back();
	m_vecActiveOneShots.pop_back();
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CSound_Manager::Create, Failed");
	}
	return pInstance;
}

void CSound_Manager::Free()
{
	StopAll();

	for (auto& Pair : m_umapSounds)
	{
		if (Pair.second)
			FMOD_CALL(Pair.second->release());
	}
	m_umapSounds.clear();

	if (m_pSystem)
	{
		FMOD_CALL(m_pSystem->close());
		FMOD_CALL(m_pSystem->release());
		m_pSystem = nullptr;
	}

	Super::Free();
}
