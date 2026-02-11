#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUISkill_BG final : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUISkillBGDesc : public GENERIC_UI_DESC
	{

	}SKILL_BG_DESC;

private:
	CUISkill_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUISkill_BG(const CUISkill_BG& rhs);
	virtual ~CUISkill_BG() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(SKILL_BG_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static CUISkill_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END