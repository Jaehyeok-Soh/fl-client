#pragma once
#include "BuilderBase.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Client)

class CBuilder_Effect final : public CBuilderBase
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
	const wstring Create_PrototypeTag(const string Tag);
	void Regist_pool(void* pArg, string& PrefabEffectTag);

public:
	static CBuilder_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END