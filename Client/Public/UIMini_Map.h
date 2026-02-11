#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUIMini_Map final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagMiniMapDesc : public DIMAGE_DESC
	{
	}MINIMAP_DESC;

private:
	CUIMini_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMini_Map(const CUIMini_Map& rhs);
	virtual ~CUIMini_Map() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

	HRESULT Attach_Personal_Info();

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Tick_CameraSight(const Vec3& vLook);
	void Rotate_Translate_CameraSight();
	void Rotate_PlayerIcon();

	_float Clamp(_float v, _float lo, _float hi);
	_float WrapPi(_float a);
	void TickRotate(_int dir, _float dt);

private:
	HRESULT Ready_Components(MINIMAP_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	// Player Icon Values
	CTransform* m_pPlayerTransform = { nullptr };

	// Player Sight Values
	Vec2 m_vPivotPos		= {};
	Vec2 m_vOriginPos		= {};
	Vec2 m_vPivotToOrigin	= {};
	_float m_fRadius		= {};
	Vec3 m_vOriginDir		= { Vec3{0.f, -1.f, 0.f} };
	_float m_fRadian		= {};

	// BG Frame Values
	_float m_fAngle		= {};
	_float m_fOmega		= {};
	_float m_fMaxOmega	= { 10.f };		// rad/s
	_float m_fAcc		= { 8.f };		// rad/s^2
	_float m_fDrag		= { 3.f };		// 1/s

public:
	static CUIMini_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END