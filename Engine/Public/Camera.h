#pragma once
#include "Component.h"

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
		_float			fViewWidth = { 1280.f };
		_float			fViewHeight = { 720.f };
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

	const Matrix& Get_ViewMatrix() const { return m_matView; }
	const Matrix& Get_ProjectionMatrix() const { return m_matProjection; }

	_float Get_Fov() const { return m_fFov; }
	void Set_Fov(_float fFov) { m_fFov; Update_Proj(); }
	_float Get_ViewWidth() const { return m_fViewWidth; }
	void Set_ViewWidth(_float fViewWidth) { m_fViewWidth = fViewWidth; Update_Proj(); }
	_float Get_ViewHeight() const { return m_fViewHeight; }
	void Set_ViewHeight(_float fViewHeight) { m_fViewHeight = fViewHeight; Update_Proj(); }
	_float Get_Far() const { return m_fFar; }
	void Set_Far(_float fFar) { m_fFar = fFar; Update_Proj(); }
private:
	void Update_Proj();
private:
	EProjectionType m_eProjectionType = { EProjectionType::PERSPECTIVE };
	_float m_fFov = { 0.f };
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