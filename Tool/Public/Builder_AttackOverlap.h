#pragma once
#include "BuilderBase.h"
#include "DataStruct_AttackOverlap.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)

class CBuilder_AttackOverlap final : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_AttackOverlap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_AttackOverlap() = default;

public:
	HRESULT Initialize();
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_AttackOverlap_Prototype(const DTO::ATTACKOVERLAP_DESC& data);

public:
	static CBuilder_AttackOverlap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END