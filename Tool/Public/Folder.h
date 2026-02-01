#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;

NS_END

NS_BEGIN(Tool)

class CFile;

class CFolder : public CBase
{
private:
	explicit CFolder();
	virtual ~CFolder() {}
private:
	HRESULT	Initialize(CFolder* pParentFloder, CONST wstring& wstrRootPath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	bool	IsRootFloder()	CONST { return m_pParentFloder == nullptr ? true : false; }
	const   map<wstring, CFolder*>& GetFloders()	const { return m_mapTreeFloder; }
	const   vector<CFile*>& GetFiles()				const { return m_vecFile; }
	const   FLODER_INFO& GetFloderInfo()			const { return m_tInfo; }
public:
	void							ImGui_Update();
	vector<wstring>					Find_File(const wstring& wstrFileName);
private:
	CGameInstance*					m_pGameInstance;
private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*			m_pContext;

	vector<CFile*>					m_vecFile;
	map<std::wstring, CFolder*>		m_mapTreeFloder;
	FLODER_INFO						m_tInfo;
	CFolder* m_pParentFloder;
public:
	static	CFolder*	Create(CFolder* pParentFloder, const std::wstring& wstrRootPath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

NS_END

