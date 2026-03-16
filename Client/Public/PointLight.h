#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Client)

class CPointLight : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagPointLight_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		LIGHT_DESC	tLightDesc{};

		_bool		isFlicker{ false };		// 깜빡일래 말래 할래 말래 할래 말래 애매하긴해~
		_float		fFlickerSpeed{ 1.0f };	// 깜빡이는 속도
		_float		fFlickerMin{ 0.5f };		// 최소 밝이 비율
	}POINTLIGHT_DESC;
public:
	CPointLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPointLight(const CPointLight& rhs);
	virtual ~CPointLight() = default;
private:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Light(const LIGHT_DESC& tLightDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
private:
	CLight*					m_pLight{};
private:
	_bool					m_isFlicker{ false };		// 깜빡일래 말래 할래 말래 할래 말래 애매하긴해~
	_float					m_fFlickerSpeed{ 1.f};		// 깜빡이는 속도
	_float					m_fFlickerMin{ 0.5f };		// 최소 밝이 비율
	_float					m_fBaseRange{0.5f};			// 원래 빛 범위 저장
	_float					m_fAccDT{0.f};
public:
	static CPointLight*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)								override;
	virtual void			Free() override;
};

NS_END

