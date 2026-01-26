#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)

class CBuilderBase abstract : public CBase
{
	using Super = CBase;
public:
	CBuilderBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBuilderBase() = default;
public:
	virtual HRESULT Build(const CDataDocumentBase& document) PURE;
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	_uint m_iLevelID = { 0 };
public:
	virtual void Free() override;
};

NS_END