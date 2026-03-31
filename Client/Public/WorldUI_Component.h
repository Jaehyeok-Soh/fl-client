#pragma once
#include "MonoBehaviour.h"
NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CWorldUI_Component : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagWorldUIComponentDesc
	{
		CGameObject* pTargetObject = { nullptr };
		_float fVPWidth;
		_float fVPHegiht;
		_float fVPTopLeftX;
		_float fVPTopLeftY;
		Vec2 fInitOffset;

		_string strBoneName = {};

	}WOLRD_UI_COMP_DESC;

protected:
	CWorldUI_Component();
	explicit CWorldUI_Component(const CWorldUI_Component& rhs);
	virtual ~CWorldUI_Component() = default;

	virtual HRESULT Initialize_Prototype() override;
public:
	virtual HRESULT Awake(_uint iLevelIndex) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float fTimeDelta) override;
public:
	void Proj_World_To_Screen();
	void Calc_Perspective();

	void Request_ScaleOffset(const _float fScale);

public:
	const Vec2& Get_TargetScreenPos() const { return m_vScreenPos; }
	const _float Get_ScaleOffset() const { return m_fScaleOffset; }

	void Set_Target(CGameObject* pTarget) { m_pTargetObject = pTarget; }
	void Set_TargetBoneName(const _string& str) { m_strBoneName = str; }
	void Set_TargetPos(const Vec3& vPos ) { m_vTargetPos = vPos; }
	void Set_TargetWorldOffset(const Vec3& vOffset ) { m_vTargetWorldOffset = vOffset; }

private:
	CGameInstance* m_pGameInstance = { nullptr };
	Matrix m_WorldProjMatrix	= {};
	_float m_fVPWidth			= {};
	_float m_fVPHegiht			= {};
	_float m_fVPTopLeftX		= {};
	_float m_fVPTopLeftY		= {};
	_float m_fViewZ				= {};

	CGameObject* m_pTargetObject= { nullptr };
	Vec2 m_fInitOffset			= {};
	Vec2 m_fCalcOffset			= {};
	Vec2 m_vScreenPos			= {};
	_float m_fScaleOffset		= {};
	Vec3 m_vTargetWorldOffset	= {};

	_bool m_isRequestScaleOffset = { false };
	_float m_fRequestScaleOffset = {};

	Vec3 m_vTargetPos = {};

	_string m_strBoneName = {};

public:
	static CWorldUI_Component* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END