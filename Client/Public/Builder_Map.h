#pragma once
#include "BuilderBase.h"
#include "DataStruct_Map.h"

NS_BEGIN(Client)

class CBuilder_Map : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_Map() = default;
private:
	HRESULT		Initialize();
public:
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_StaticModel(const DTO::TMap_StaticModelData& tData);
private:
	CGameInstance* m_pGameInstance{ nullptr };
public:
	static CBuilder_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END
