#pragma once
#include "BuilderBase.h"
#include "DataDocument_Map.h"


NS_BEGIN(Engine)

NS_END

NS_BEGIN(Tool)

class CMapToolManager;

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
	HRESULT	Create_InstanceModel(const DTO::TMap_InstanceModelData& tData);
private:
	CGameInstance*		m_pGameInstance{ nullptr };
	CMapToolManager*	m_pMapToolManager{nullptr};
public:
	static CBuilder_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END