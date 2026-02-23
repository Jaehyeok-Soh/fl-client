#pragma once
#include "MapObject.h"

NS_BEGIN(Client)

class CLandScape final : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagLandScape_Desc : public CMapObject::MAPOBJECT_DESC
	{
		_int		iIndex{ 0 };
		/* ÁöÇü Texture UV ÁÂÇ¥  */
		Vec2		vTextureUV_LT{};
		Vec2		vTextureUV_RB{};

	}LANDSCAPE_DESC;
public:
	CLandScape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLandScape(const CLandScape& rhs);
	virtual ~CLandScape() = default;
private:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(LANDSCAPE_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
private:
	/* ÁöÇü Index , Texture UV ÁÂÇ¥  */
	_int					m_iIndex{ 0 };
	Vec2					m_vTextureUV_LT{};
	Vec2					m_vTextureUV_RB{};
public:
	static CLandScape*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)								override;
	virtual void			Free() override;
};

NS_END