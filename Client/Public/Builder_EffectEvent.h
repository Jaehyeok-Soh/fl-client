#pragma once
#include "BuilderBase.h"
#include "DataStruct_EffectEvent.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Client)

class CBuilder_EffectEvent :
    public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_EffectEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_EffectEvent() = default;

	HRESULT Initialize();
public:
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_Effect(const DTO::ANIM_EVENT_INFO_DESC& data);
public:
	static CBuilder_EffectEvent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;

};

NS_END
