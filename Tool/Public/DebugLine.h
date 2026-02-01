#pragma once
#include "GameObject.h"


NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Tool)

class CGrid;


class CDebugLine : public CGameObject
{
public:
	using Super = CGameObject;
public:
	typedef struct DebugLine_Desc : CGameObject::GAMEOBJECT_DESC
	{
		Color vColor_Z{1.f,1.f,1.f,1.f};
		Color vColor_X{1.f,1.f,1.f,1.f};
	}DEBUGLINE_DESC;
public:
	CDebugLine(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CDebugLine(const CDebugLine& rhs);
	virtual ~CDebugLine() = default;
private:
	// CGameObject을(를) 통해 상속됨
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:

	virtual HRESULT	Awake(const _uint iCurrentLevelID) override;
	virtual void	Update_Priority(const _float fTimeDelta)		override;
	virtual void	Update(const _float fTimeDelta)				override;
	virtual void	Update_Late(const _float fTimeDelta)			override;
	virtual void	Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT Render();
private:
	/* Btach */
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch{ nullptr };
	BasicEffect*									m_pEffect{ nullptr };
	ID3D11InputLayout*								m_pInputLayout{ nullptr };

	Color			 m_vColor_X{ 1.f,1.f,1.f,1.f };
	Color			 m_vColor_Z{ 1.f,1.f,1.f,1.f };
public:
	static  CDebugLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void		Free()override;
};

NS_END