#pragma once
#include "BuilderBase.h"
#include "DataDocument_Example.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)

class CBuilder_Example final : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_Example(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBuilder_Example() = default;
public:
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_StaticModel(const DTO::TExample_StaticModelData& data);
	HRESULT Create_Light(const DTO::TExample_LightData& data);
public:
	static CBuilder_Example* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END