#pragma once
#include "InteractiveObject.h"


NS_BEGIN(Client)

class CWeaponPickUp final: public CInteractiveObject
{
	using Super = Client::CInteractiveObject;
public:
	enum class EWeaponType
	{
		One_Handed_Sword,	/* 한손검 */
		Dual_Daggers,		/* 쌍칼 */
		Machine_gun,		/* 머신건 */
		END,
	};
public:
	typedef struct tagWeaponPickUp_Desc : public CInteractiveObject::INTERACTIVEOBJECT_DESC
	{
		/* 충돌처리 할건지말건지 */
		_bool						isTutorialEvent{ false }; /* Tutoiral Event 인지 아닌지 */
		string						strWeaponType{""};
	}WEAPONPICKUP_DESC;
protected:
	CWeaponPickUp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWeaponPickUp(const CWeaponPickUp& rhs);
	virtual ~CWeaponPickUp() {}
protected:
	virtual HRESULT					Initialize_Prototype()							override;
	virtual HRESULT					Initialize(void* pArg)							override;
	HRESULT							Ready_RGBColorDesc();
public:
	virtual HRESULT					Awake(const _uint iCurrentLevelID)				override;
	virtual void					Update_Priority(const _float fTimeDelta)		override;
	virtual void					Update(const _float fTimeDelta)					override;
	virtual void					Update_Late(const _float fTimeelta)				override;
	virtual void					Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT					Render()										override;
public:
	EWeaponType						m_eWeaponType;
	_bool							m_isTutorialEvent;
	SHADER_RGBCOLOR_DESC			m_tShaderRGBColorDesc;
public:
	static CWeaponPickUp*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*			Clone(void* pArg)override;
	virtual					void	Free() override;
public:
	// IInteractable을(를) 통해 상속됨
	void Interact() override;

	// IQuest을(를) 통해 상속됨
	void QuestEnter() override;
	void QuestExit() override;
public:
	static	CWeaponPickUp::EWeaponType				WeaponType_ToEnum(const string& strType);
	static	string									WeaponType_ToString(CWeaponPickUp::EWeaponType eType);
};

NS_END