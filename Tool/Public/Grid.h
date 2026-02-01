#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class CVIBuffer_Line_Color;

NS_END


NS_BEGIN(Tool)

class CGrid : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct Grid_Desc : CGameObject::GAMEOBJECT_DESC
	{
		Color	vColor{1.f,1.f,1.f,1.f};
		_uint	iMaxLineCount{};
	}GRID_DESC;
private:
	CGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrid(const CGrid& rhs);
	virtual ~CGrid() = default;
private:
	virtual HRESULT	Initialize(void* pArg) override;
	HRESULT			Ready_Buffer();
public:
	virtual void	Update_Priority(const _float fTimeDelta)override;
	virtual void	Update(const _float fTimeDelta)override;
	virtual void	Update_Late(const _float fTimeDelta)override;
	virtual void	Ready_Before_Render(const _float fTimeDelta)override;
	virtual HRESULT Render()override;
public:
	
	CVIBuffer_Line_Color*	m_pBuffer{nullptr};


	_uint					m_iMaxLineCount;
	Color					m_vColor{0.f,0.f,0.f,1.f};
	
public:
	static	CGrid*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual	void			Free()override;

	// CGameObject을(를) 통해 상속됨
	HRESULT					Initialize_Prototype() override;
	HRESULT					Awake(const _uint iCurrentLevelID) override;
};

NS_END