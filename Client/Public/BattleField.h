#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

/* Battle Field는 단하나? */
class CBattleField : public CGameObject
{
	using Super = CGameObject;
public:
	enum class Field_State
	{
		Idle,      // 작동전 (대기)
		Active,    // 작동중
		Finished   // 작동후 (완료)
	};
	enum class Field_Type
	{
		Sphere,
		Box,
		END,
	};
public:
	typedef struct tagBattleField_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		CBattleField::Field_Type eFieldType{ CBattleField::Field_Type::END};

		Vec3				vExtents{1.f,1.f,1.f};
		_float				fRadius{1.f};
	}BATTLEFIELD_DESC;
protected:
	CBattleField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBattleField(const CBattleField& rhs);
	virtual ~CBattleField() = default;
public:
	virtual HRESULT				Initialize_Prototype()							override;
	virtual HRESULT				Initialize(void* pArg)							override;
	HRESULT						Ready_Component(BATTLEFIELD_DESC* pDesc);
public:
	virtual HRESULT				Awake(const _uint iCurrentLevelID)				override;
	virtual void				Update_Priority(const _float fTimeDelta)		override;
	virtual void				Update(const _float fTimeDelta)					override;
	virtual void				Update_Late(const _float fTimeelta)				override;
	virtual void				Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT				Render()										override;
private:
	CBattleField::Field_State	m_eFieldState{ CBattleField::Field_State::Idle};
	CBattleField::Field_Type	m_eFieldType{ CBattleField::Field_Type::END };
public:
	static CBattleField*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual void			Free()	override;
};

NS_END
