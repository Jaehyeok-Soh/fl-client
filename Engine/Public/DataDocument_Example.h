#pragma once
#include "DataDocumentBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDataDocument_Example final : public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_Example();
	virtual ~CDataDocument_Example() = default;

	HRESULT Initialize();
public:
	virtual DTO::ECategory Get_Category() const { return DTO::ECategory::MAP; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;
	HRESULT Try_Add(const DTO::TExample_LightData &data);
	HRESULT Try_Add(const DTO::TExample_StaticModelData& data);
private:
	DTO::IObjectDataBase* Create_ObjectData(DTO::EMapType eType);
	HRESULT Try_Add(DTO::IObjectDataBase* pObject);
public:
	static CDataDocument_Example* Create();
	virtual void Free() override;
};

NS_END