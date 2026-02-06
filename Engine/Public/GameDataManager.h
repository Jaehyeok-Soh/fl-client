#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameDataManager final : public CBase
{
	using Super = CBase;
private:
	CGameDataManager();
	virtual ~CGameDataManager() = default;

	HRESULT Initialize();
public:

public:
	static ID3D11ShaderResourceView* Make_ShaderResourceViewColor(_uint A, _uint R, _uint G, _uint B,ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:

private:

private:
	class CGameInstance *m_pGameInstance = { nullptr };
public:
	static CGameDataManager* Create();
	virtual void Free() override;
};

NS_END