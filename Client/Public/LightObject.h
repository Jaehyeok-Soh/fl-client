#pragma once
#include "MapObject.h"


/* 모델도 있고 Light도 있는 애들 전용 */

NS_BEGIN(Client)

class CLightObject : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagLightObject_Desc : public CMapObject::MAPOBJECT_DESC
	{
		float		fEmissivePower{1.f};
		LIGHT_DESC	tLightDesc{};
		_bool		isFlicker{ false };		// 깜빡일래 말래 할래 말래 할래 말래 애매하긴해~
		_float		fFlickerSpeed{ 1.0f };	// 깜빡이는 속도
		_float		fFlickerMin{ 0.5f };		// 최소 밝이 비율
	}LIGHTOBJECT_DESC;
public:
	CLightObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightObject(const CLightObject& rhs);
	virtual ~CLightObject() = default;
private:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Light(const LIGHT_DESC& tLightDesc);
	HRESULT					Ready_Collider();
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
private:
	CLight* m_pLight{};
private:
	_bool					m_isRenderModel{ false };
	EMapObjectShaderPass	m_eShaderPass{EMapObjectShaderPass::StaticObject};

	_bool					m_isFlicker{ false };		// 깜빡일래 말래 할래 말래 할래 말래 애매하긴해~
	_float					m_fFlickerSpeed{ 1.f };		// 깜빡이는 속도
	_float					m_fFlickerMin{ 0.5f };		// 최소 밝이 비율
	_float					m_fBaseRange{ 0.5f };			// 원래 빛 범위 저장
	_float					m_fAccDT{ 0.f };
	CCollider*				m_pLightCollider{nullptr};
public:
	static CLightObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)								override;
	virtual void			Free() override;
};

NS_END