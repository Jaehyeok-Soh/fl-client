#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CUIIcon_Component : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagUIIConComponentDesc
	{
		CGameObject* pPlayer = { nullptr };
		_wstring wstrIconTextureTag = {};
	}UI_ICON_COMP_DESC;

protected:
	CUIIcon_Component();
	explicit CUIIcon_Component(const CUIIcon_Component& rhs);
	virtual ~CUIIcon_Component() = default;

	virtual HRESULT Initialize_Prototype() override;
public:
	virtual HRESULT Awake(_uint iLevelIndex) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float fTimeDelta) override;

public:
	void Set_ScreenPos(const Vec2& vPos) { m_vScreenPos = vPos; }
	void Set_isRanged(_bool is) { m_isRanged = is; }

	_bool Get_isRanged() const { return m_isRanged; }
	const Vec2& Get_ScreenPos() const { return m_vScreenPos; }

	const _wstring& Get_IconTextureTag()const { return m_wstrTextureTag; }

private:
	CGameInstance* m_pGameInstance = { nullptr };
	Vec2 m_vScreenPos = {};
	_bool m_isRanged = { false };
	
	CGameObject* m_pPlayer = { nullptr };

	_wstring m_wstrTextureTag = {};

public:
	static CUIIcon_Component* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END