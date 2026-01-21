#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
	using Super = CBase;
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

	HRESULT Initialize(_uint iLevelCount);
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring &wstrPrototypeTag, CBase *pPrototype);
	CBase* Find_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag);
	CBase* Clone_Prototype(EPrototypeType ePrototypeID, _uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg = nullptr);
	void Clear(_uint iLevelIndex);
private:
	_uint m_iLevelCount = { 0 };
	vector<map<const wstring, CBase*>> m_pPrototypes;
public:
	static CPrototype_Manager* Create(_uint iLevelCount);
	virtual void Free() override;
};

NS_END