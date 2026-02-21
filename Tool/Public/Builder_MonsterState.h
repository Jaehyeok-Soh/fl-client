#pragma once
#include "BuilderBase.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)

class CBuilder_MonsterState final : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_MonsterState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_MonsterState() = default;

public:
	HRESULT Initialize();
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_MonsterState_Prototype(const DTO::MONSTER_STATEBASE_DESC& data);

public:
	static CBuilder_MonsterState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END

