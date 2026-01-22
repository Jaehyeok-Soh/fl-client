#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CResourceBase abstract : public CBase
{
	using Super = CBase;
public:
	typedef struct tagResourceBaseDesc
	{
		wstring wstrPath = { L"" };
		wstring wstrName = { L"" };
	}RESOURCE_BASE_DESC;
protected:
	CResourceBase(EResourceType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CResourceBase(const CResourceBase& rhs);
	virtual ~CResourceBase() = default;

	virtual HRESULT Initialize(void* pArg) PURE;
	virtual HRESULT Initialize_Copy(void* pArg) { return S_OK; }
public:
	EResourceType Get_Type() const { return m_eType; }
	const _tchar* Get_Path() const { return m_wszPath; }
	const _tchar* Get_Name() const { return m_wszName; }
	void Set_Name(const wstring& wstrName) { ::lstrcpy(m_wszName, wstrName.data()); }
protected:
	_bool IsClone() const { return m_bClone; }
private:
	_bool m_bClone = { false };
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
protected:
	EResourceType m_eType = { EResourceType::END };
	_tchar m_wszPath[MAX_PATH];
	_tchar m_wszName[MAX_NAME];
public:
	virtual void Free() override;
};

NS_END