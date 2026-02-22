#pragma once
#include "BuilderBase.h"
#include "DataStruct_AttackPreset.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)

class CBuilder_AttackPreset final : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_AttackPreset(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_AttackPreset() = default;

public:
	HRESULT Initialize();
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Set_AttackPreset(const DTO::TAttackPreset_Data& data);

public:
	static CBuilder_AttackPreset* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END