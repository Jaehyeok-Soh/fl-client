#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUIMiniMap_Monster_Icon final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIMiniMapMonsterIcon : public DIMAGE_DESC
	{
	}MINIMAP_MONSTER_ICON_DESC;
private:
	CUIMiniMap_Monster_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMiniMap_Monster_Icon(const CUIMiniMap_Monster_Icon& rhs);
	virtual ~CUIMiniMap_Monster_Icon() = default;
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
	HRESULT Ready_Components(MINIMAP_MONSTER_ICON_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
	virtual void Bind_Events()override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
	void Convert_Count_To_Rank();
	virtual HRESULT Spawn_FromPool(void* pArg)override;
	virtual HRESULT Despawn_FromPool()override;

	void Rotate_MonsterIcon();
private:
	CGameObject* m_pTarget = { nullptr };
	CGameObject* m_pPlayer = { nullptr };

	_bool m_isSpawned = { false };
	_float m_fRadian = {};
public:
	static CUIMiniMap_Monster_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END