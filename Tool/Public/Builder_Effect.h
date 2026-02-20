#pragma once
#include "C:\Users\jwroy\OneDrive\πŸ≈¡ »≠∏È\TEAM_3D_PROJECT\FinalProject-develop\EngineSDK\Include\BuilderBase.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)

class CBuilder_Effect :
    public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_Effect() = default;

	HRESULT Initialize();
public:
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_Effect(const DTO::TEFFECT_ContainerData& data);
public:
	static CBuilder_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END