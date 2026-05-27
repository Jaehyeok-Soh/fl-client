#include "pch.h"
#include "Level_Client.h"
#include "Client_EventDefine.h"

//=================
// GameInstance
//=================
#include "GameInstance.h"

//=================
// GameObject
//=================
#include "RemotePlayer.h"

//=================
// Component
//=================
#include "Transform.h"

//=================
// DataModel
//=================
#include "UserModel.h"

CLevel_Client::CLevel_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{pDevice, pDeviceContext}
{
}

HRESULT CLevel_Client::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	Bind_Network();

	return S_OK;
}

HRESULT CLevel_Client::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (iCurrentLevelID >= ENUM_TO_UINT(ELevelType::TUTORIAL_VILLAGE))
		Ready_Network();

	return S_OK;
}

void CLevel_Client::Bind_Network()
{
	m_pGameInstance->BindNetworkJoinedUser([this](UserModel userModel) {
		this->SpawnRemotePlayer(userModel);
		});
	m_pGameInstance->BindNetworkLeftUser([this](UINT32 clientId) {
		this->DespawnRemotePlayer(clientId);
		});
}

void CLevel_Client::Ready_Network()
{
	m_pGameInstance->RoomEnterRequest();
}

void CLevel_Client::SpawnRemotePlayer(UserModel userModel)
{
	auto iter = m_dictRemoteUser.find(userModel.ClientIndex);
	if (iter != m_dictRemoteUser.end())
		return;

	CRemotePlayer::REMOTE_PLAYER_DESC desc = {};
	desc.iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	desc.ePlayerType = CPlayer::PLAYER_TYPE::MOON;
	desc.wstrBodyModelTag = L"Prototype_Component_Model_Moon";
	desc.vSpawnPosition = Vec3{ 0.f, 0.f, 0.f };
	desc.tUserModel = userModel;

	auto callback = [this, userModel](CGameObject* pSpawnedObj)
		{
			if (pSpawnedObj == nullptr)
				return;

			CRemotePlayer* pRemote = static_cast<CRemotePlayer*>(pSpawnedObj);

			pRemote->Set_Name(userModel.UserID);

			CTransform* pTransform = pRemote->Get_Component<CTransform>();
			if (pTransform != nullptr)
				pTransform->Rotation(Vec3{ 0.f, 1.f, 0.f }, 0.f);

			m_dictRemoteUser.insert({ static_cast<UINT32>(userModel.ClientIndex), pRemote });
		};

		m_pGameInstance->Request_AddObject(
			ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_RemotePlayer",
			m_pGameInstance->Get_CurrentLevelIndex(),
			L"Remote_Player_Layer",//g_wszPlayerLayer,
			&desc,
			callback
		);
}

void CLevel_Client::DespawnRemotePlayer(UINT32 clientId)
{
	auto iter = m_dictRemoteUser.find(clientId);
	if (iter == m_dictRemoteUser.end())
		return;

	CGameObject* pRemotePlayer = iter->second;

	m_pGameInstance->Request_DeleteGameObject(m_pGameInstance->Get_CurrentLevelIndex(),
		pRemotePlayer);

	m_dictRemoteUser.erase(iter);
}

void CLevel_Client::Free()
{
	m_dictRemoteUser.clear();

	Super::Free();
}
