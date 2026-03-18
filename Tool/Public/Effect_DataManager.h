#pragma once
#include "Base.h"
#include "Effect.h"

NS_BEGIN(Engine)
class  CGameInstance;
class  CGameObject;
class  CBuilderSystem;
NS_END

NS_BEGIN(Tool)

enum class EEFFECT_DATATYPE
{
	NORMAL,
	ENVIRONMENT,
};

class CEffect_DataManager :
    public CBase
{
public:
	using Super = CBase;
	DECLARE_SINGLETON(CEffect_DataManager)

private:
	CEffect_DataManager();
	virtual ~CEffect_DataManager() = default;

public:
	HRESULT Initialize_EffectDataManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	void Update(const _float& fTimeDelta);
	void Render();

private:

	HRESULT			Ready_Builder();
	HRESULT			Ready_BuildFiles();

public:
	std::vector<string>& Get_EffectTagList() { return m_vecEffectTags; }
	CGameObject*		 Make_EffectPrototype(EEFFECT_DATATYPE DataType, const string& Tag);
	void Push_ToolEffectData(_uint HashTag, void* Desc);
	void* Find_ToolEffectData(_uint iHashTag);


private:
	void				 Push_EffectTag(const string& Tag);

private:
	std::vector<string> m_vecEffectTags;	// 이펙트 전용 Tag 리스트들.

	CBuilderSystem*		 m_pBuilderSystem = { nullptr };
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	class CGameInstance*  m_pGameInstance = { nullptr };

private:
	std::map<_uint, Effect::EFFECT_CONTAINERDESC>				m_EffectDescData = {};	// 해싱한 Data

public:
	virtual void Free() override;
};

NS_END