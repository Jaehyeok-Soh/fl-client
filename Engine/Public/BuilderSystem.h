#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;
class CBuilderBase;

// TODO - manifest json?
class ENGINE_DLL CBuilderSystem final : public CBase
{
	using Super = CBase;
private:
	CBuilderSystem();
	virtual ~CBuilderSystem() = default;
	
	HRESULT Initialize();
public:
	HRESULT Ready_Builder(DTO::ECategory eCategory, CBuilderBase* pBuilder);
	HRESULT Build_File(_uint iLevelID, DTO::ECategory eCategory, string strFileKey);
	// 기존 산출물 정리 후 재빌드
	HRESULT Rebuilde_File(_uint iLevelID, DTO::ECategory eCategory, string strFileKey);
private:
	_bool Has_Builder(DTO::ECategory eCategory) const { return m_arrBuilders[ENUM_TO_UINT(eCategory)] != nullptr; }
private:
	array<CBuilderBase*, ENUM_TO_SZET(DTO::ECategory::END)> m_arrBuilders;
	CGameInstance* m_pGameInstance{nullptr};
public:
	static CBuilderSystem* Create();
	virtual void Free() override;
};

NS_END