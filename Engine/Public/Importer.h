#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CImporter : public CBase
{
	using Super = CBase;
private:
	CImporter();
	virtual ~CImporter() = default;

	HRESULT Initialize();
public:
	Assimp::Importer* Get_Assimp_Importer() { return m_pImporter; }
private:
	Assimp::Importer* m_pImporter = { nullptr };
public:
	static CImporter* Create();
	virtual void Free() override;
};

NS_END