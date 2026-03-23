#pragma once
#include "Component.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::CAMERA;
	typedef struct tagCameraDesc
	{
		EProjectionType eProjectionType = { EProjectionType::PERSPECTIVE };
		_float			fFov = { g_XMPi.f[0] / 4.f };
		_float			fViewWidth = { 1600.f };
		_float			fViewHeight = { 900.f };
		_float			fNear = { 1.f };
		_float			fFar = { 1000.f };
	}CAMERA_DESC;
private:
	CCamera();
	explicit CCamera(const CCamera& rhs);
	virtual  ~CCamera() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Update_View();
	void Update_View(const CAMERA_POSE& pose);
	const Matrix& Get_ViewMatrix() const { return m_matView; }
	const Matrix& Get_ProjectionMatrix() const { return m_matProjection; }

	_float Get_Fov() const { return m_fBaseFov; }
	void Set_Fov(_float fFov) { m_fBaseFov = fFov; Update_Proj(m_fBaseFov); }
	_float Get_ViewWidth() const { return m_fViewWidth; }
	void Set_ViewWidth(_float fViewWidth) { m_fViewWidth = fViewWidth; Update_Proj(m_fBaseFov); }
	_float Get_ViewHeight() const { return m_fViewHeight; }
	void Set_ViewHeight(_float fViewHeight) { m_fViewHeight = fViewHeight; Update_Proj(m_fBaseFov); }
	_float Get_Far() const { return m_fFar; }
	void Set_Far(_float fFar) { m_fFar = fFar; Update_Proj(m_fBaseFov); }
	_float Get_Near() const { return m_fNear; }
	void Set_Near(_float fNear) { m_fNear = fNear; Update_Proj(m_fBaseFov); }
private:
	void Update_Proj(_float fFov);
private:
	EProjectionType m_eProjectionType = { EProjectionType::PERSPECTIVE };
	_float m_fRenderFov = { 0.f };
	_float m_fBaseFov = { 0.f };
	_float m_fViewWidth = { 0.f };
	_float m_fViewHeight = { 0.f };
	_float m_fAspectRatio = { 0.f };
	_float m_fNear = { 0.f };
	_float m_fFar = { 0.f };

	Matrix m_matView = {};
	Matrix m_matProjection = {};
public:
	static CCamera* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END