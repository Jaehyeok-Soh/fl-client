#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Engine)
struct DelegateHandle;
NS_END

NS_BEGIN(Client)
class CUIQuestNavi_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIQuestNaviImageDesc : public DIMAGE_DESC
	{
	}QUEST_NAVI_IMAGE_DESC;

private:
	CUIQuestNavi_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIQuestNavi_Image(const CUIQuestNavi_Image& rhs);
	virtual ~CUIQuestNavi_Image() = default;
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
	HRESULT Ready_Components(QUEST_NAVI_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Bind_Events()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;

	void Proj_World_To_Screen();

	void Tick_Navi_Icon(const _float fTimeDelta);
	void Tick_Navi_Fx(const _float fTimeDelta);
	void Tick_Navi_Dir(const _float fTimeDelta);

private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	Vec3 m_vTargetPos = {};

	_float m_fViewZ = {};

	_float m_fVPWidth = {};
	_float m_fVPHegiht = {};
	_float m_fVPTopLeftX = {};
	_float m_fVPTopLeftY = {};

	Vec2 m_vScreenPos = {};

	_bool m_isChange = { false };
	_bool m_isMonsterKill = { false };

public:
	static CUIQuestNavi_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END