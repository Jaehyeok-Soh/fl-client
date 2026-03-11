#pragma once
#include "Base.h"
#include "MulticastDelegate.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUIPrefab;

enum class EUIMinimapEventID {
	END
};

typedef struct tagUIMinimapEventesc
{
	EUIMinimapEventID  eEventID = { EUIMinimapEventID::END };

	_uint iParam0 = {};

}UI_MINIMAP_EVENT_DESC;

enum class EUIMinimapIconTypeID
{
	MONSTER, END
};

class CUIMinimap_Manager final : public CBase
{
	DECLARE_SINGLETON(CUIMinimap_Manager)

private:
	CUIMinimap_Manager();
	virtual ~CUIMinimap_Manager() = default;
public:
	HRESULT Initialize_UIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update(const _float fTimeDelta);

public:
	// +Z 기준으로 얼마나 돌아있나 
	_float WorldLook_Convert_To_Radian(const Vec3& vLook);

	// 아이콘 컴포넌트가 있어야 됨
	HRESULT Add_Ranged_Object(class CGameObject* pObj, EUIMinimapIconTypeID eID);
	HRESULT Delete_Ranged_Object(class CGameObject* pObj);

	Vec2 WorldPos_Convert_MinimapPosOffset(const Vec3& vPos, const Vec3& vPlayerPos);

private:
	CMulticastDelegate<void(const UI_MINIMAP_EVENT_DESC&)>& Get_UIEvents() { return m_vEvents; }

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CMulticastDelegate<void(const UI_MINIMAP_EVENT_DESC&)> m_vEvents = {};

	vector<CGameObject*> m_vecRangedObjects;

public:
	virtual void Free()override;

};

NS_END