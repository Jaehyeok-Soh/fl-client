#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CUILevelChange_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUILevelChangeImageDesc : public DIMAGE_DESC
	{
	}LEVEL_CHANGE_DESC;

private:
	CUILevelChange_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUILevelChange_Image(const CUILevelChange_Image& rhs);
	virtual ~CUILevelChange_Image() = default;
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
	HRESULT Ready_Components(LEVEL_CHANGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Trigger_By_InteractState()override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;

private:
	ELevelType m_eNextLevelID = { ELevelType::END };
	_wstring m_wstrText = {};
	Vec4 m_vFontColor = {};
	_bool m_isHover = { false };

public:
	static CUILevelChange_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END