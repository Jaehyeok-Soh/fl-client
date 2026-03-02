#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUIMonster_MiniMap_Icon final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIComboImageDesc : public DIMAGE_DESC
	{
	}COMBO_IMAGE_DESC;
private:
	CUIMonster_MiniMap_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMonster_MiniMap_Icon(const CUIMonster_MiniMap_Icon& rhs);
	virtual ~CUIMonster_MiniMap_Icon() = default;
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
	HRESULT Ready_Components(COMBO_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

	void Convert_Count_To_Rank();
private:
	CStatCom_Player* m_pPlayerStatCom = { nullptr };

public:
	static CUIMonster_MiniMap_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END