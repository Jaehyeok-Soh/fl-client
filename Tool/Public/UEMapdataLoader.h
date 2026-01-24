#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CUEMapdataParser;

class CUEMapDataLoader final : public CBase
{
	using Super = CBase;
private:
	CUEMapDataLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CUEMapDataLoader() = default;
public:

	HRESULT Make_Prototype(const wstring &wstrMapModelFolderPath);
	HRESULT Make_StaticModel(const wstring &wstrRawDataFilePath, const wstring &wstrStaticModelLayerTag, const wstring &wstrColmeshLayerTag);
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
public:
	static CUEMapDataLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END