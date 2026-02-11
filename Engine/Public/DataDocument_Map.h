#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_Map.h"


NS_BEGIN(Engine)


class ENGINE_DLL CDataDocument_Map : public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_Map();
	virtual ~CDataDocument_Map();
private:
	HRESULT	Initialize();
public:
	virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::MAP; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;
	HRESULT Try_Add(const DTO::TMap_MapObjectData& data);
private:
	HRESULT Try_Add(IObjectDataBase* pObject);

public:
	static CDataDocument_Map* Create();
	virtual void Free() override;
};

NS_END