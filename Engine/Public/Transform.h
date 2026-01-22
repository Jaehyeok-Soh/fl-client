#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CNavigation;

class ENGINE_DLL CTransform final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::TRANSFORM;
	typedef struct tagTransformDesc
	{
		_float3 vPosition = { 0.f, 0.f, 0.f };
		_float fMovePerSec = { 5.f };
		_float fRotatePerSec = { 8.f };
	}TRANSFORM_DESC;
private:
	CTransform();
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_ShaderResource(class CShader* pShader);
	const _float4x4& Get_WorldMatrix() const { return m_matWorld; }	
	inline _vector Get_Info(TRANSFORM_INFO_STATE eState);
	inline void Set_Info(TRANSFORM_INFO_STATE eState, _fvector vValue);
	inline void Set_Info(TRANSFORM_INFO_STATE eState, _float4 vValue);
	inline _float3 Get_Scaled();
	inline _float Get_Scaled_X();
	inline _float Get_Scaled_Y();
	inline _float Get_Scaled_Z();
	inline void Set_Scale(_float fX, _float fY, _float fZ);
	inline void Set_Scale(const _float3 &vValue);
	inline void Add_Scale(_float fX, _float fY, _float fZ);
	inline void Go_Dir(const _float fTimeDelta, _fvector vTargetDir, CNavigation* pNavigation = nullptr);
	inline void Go_Straight(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_BackWard(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Up(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Down(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Up(_fvector vAxis, const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Down(_fvector vAxis, const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Right(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Left(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Roll_Turn(const _float fTimeDelta);
	inline void Pitch_Turn(const _float fTimeDelta);
	inline void Yaw_Turn(const _float fTimeDelta);
	inline void Rotation(_fvector vAxis, _float fRadian);
	inline void Rotation(_float3 vAxis, _float fRadian);
	inline void Rotation(_float fX, _float fY, _float fZ);
	inline void Turn_WorldYAxis(_fvector vTargetDir, const _float fTimeDelta);
	inline void Turn(_fvector vAxis, const _float fTimeDelta);
	inline void Look_At(_fvector vPoint);
	inline void Look_At_XZ(_fvector vPoint);
	inline void Chase(_fvector vPoint, _float fMinDistance, const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	const _float4& Get_PrevPosition() const { return m_vPrevPosition; }
	_float Get_RotatePerSec() const { return m_fRotatePerSec; }
	void Set_RotatePerSec(_float fSpeed) { m_fRotatePerSec = fSpeed; }
	_float Get_MovePerSec() const { return m_fMovePerSec; }
	void Set_MoveScale(_float fScale) { m_fMoveScale = fScale; }
	_float Get_MoveScale() const { return m_fMoveScale; }
	void Start_Force(_fvector vTargetDir, _float fForceAbs, _float fDragK);
	void Apply_Force(_float fDeltaTime, CNavigation* pNavigation = nullptr);
	void Update_PrevPosition();
	void Force_Clear();
private:
	_bool m_bControll = { false };
	_float4 m_vPrevPosition = { 0.f, 0.f, 0.f, 1.f };
	_float m_fMoveScale = { 1.f };
	_float m_fMovePerSec = { 5.f };
	_float m_fRotatePerSec = { 8.f };
	_float3 m_vForceVelocity = { 0.f, 0.f, 0.f };
	_float m_fDragK = { 0.f };
	_float4x4		m_matWorld;
public:
	virtual CComponent* Clone(void* pArg) override;
	static CTransform* Create();
	virtual void Free() override;
};

NS_END