#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUISkill_BG final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUISkillBGDesc : public DIMAGE_DESC
	{
	}SKILL_BG_DESC;

private:
	CUISkill_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUISkill_BG(const CUISkill_BG& rhs);
	virtual ~CUISkill_BG() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

	HRESULT Attach_Personal_Info();

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Trigger_User_Use_Skill();

	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;

private:
	HRESULT Ready_Components(SKILL_BG_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	_bool m_isUseESkillEventStart = { FALSE };
	_bool m_isUseESkillEvnetEnd = { FALSE };

	_bool m_isUseSkillEventStart = { FALSE };
	_bool m_isUseSkillEventEnd = { FALSE };
	_float m_fSkillCoolTimeRatio = {1.f};

public:
	static CUISkill_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END