#pragma once
#include "BuilderBase.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Client)

class EffectBuilder final : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	EffectBuilder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~EffectBuilder() = default;

	HRESULT Initialize();
public:
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_Effect(const DTO::TEFFECT_ContainerData& data);
public:
	static EffectBuilder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END