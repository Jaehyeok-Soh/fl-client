#pragma once
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Client abstract : public CLevel
{
	using Super = CLevel;

protected:
	explicit CLevel_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Client() = default;

	virtual HRESULT Initialize() override;
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;

protected:
	virtual void Bind_Network() override;
	virtual void Ready_Network() override;
	virtual void SpawnRemotePlayer(struct UserModel userModel) override;
	virtual void DespawnRemotePlayer(UINT32 clientId) override;

protected:
	unordered_map<UINT32, CGameObject*> m_dictRemoteUser;

public:
	virtual void Free() override;
};

NS_END