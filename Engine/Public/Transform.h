#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CNavigation;
class CPhysics_QueryFilterCallback;

class ENGINE_DLL CTransform final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::TRANSFORM;
	typedef struct tagTransformDesc
	{
		Matrix  ScaleMatrix = Matrix::Identity;
		Matrix  RotationMatrix = Matrix::Identity;
		Matrix  TranslationMatrix = Matrix::Identity;
		_float	fMovePerSec = { 5.f };
		_float	fRotatePerSec = { 8.f };
	}TRANSFORM_DESC;
private:
	CTransform();
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_ShaderResource(class CShader* pShader);
	Matrix *Get_WorldMatrixPtr() { return &m_matWorld; }
	const Matrix& Get_WorldMatrix() const { return m_matWorld; }
	inline Matrix Get_WorldMatrix_Transpose();
	inline Matrix Get_WorldMatrix_Inverse();
	inline Vec3 Get_Info(TRANSFORM_INFO_STATE eState);
	inline void Set_Info(TRANSFORM_INFO_STATE eState, Vec3 vValue);
	inline Vec3 Get_Scaled();
	inline void Set_Scale(_float fX, _float fY, _float fZ);
	inline void Set_Scale(const Vec3 &vValue);
	inline void Add_Scale(_float fX, _float fY, _float fZ);
	inline void Go_Dir(const Vec3 &vTargetDir, const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Straight(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_BackWard(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Up(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Down(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Up(const Vec3 &vAxis, const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Down(const Vec3& vAxis, const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Right(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Go_Left(const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	inline void Rotation(TRANSFORM_INFO_STATE eState, _float fRadian);
	inline void Rotation(_float fRadianX, _float fRadianY, _float fRadianZ);
	inline void Roll_Turn(const _float fTimeDelta);
	inline void Pitch_Turn(const _float fTimeDelta);
	inline void Yaw_Turn(const _float fTimeDelta);
	inline void Rotation(const Vec3& vAxis, _float fRadian);
	inline void Rotation(const Quat& vQuat);
	inline void Turn_WorldYAxis(const Vec3 &vTargetDir, const _float fTimeDelta);
	inline void Turn(const Vec3 &vAxis, const _float fTimeDelta);
	inline void Look_At(const Vec3 &vPoint);
	inline void Look_At_XZ(Vec3 vPoint);
	inline _bool Chase(const Vec3 &vPoint, _float fMinDistance, const _float fTimeDelta, CNavigation* pNavigation = nullptr);
	const Vec3& Get_PrevPosition() const { return m_vPrevPosition; }
	_float Get_RotatePerSec() const { return m_fRotatePerSec; }
	void Set_RotatePerSec(_float fSpeed) { m_fRotatePerSec = fSpeed; }
	_float Get_MovePerSec() const { return m_fMovePerSec; }
	void Set_MoveScale(_float fScale) { m_fMoveScale = fScale; }
	void Set_MovePerSec(float fMoveSpeed) { m_fMovePerSec = fMoveSpeed; }
	void Set_WorldMatrix(const Matrix& WorldMatrix) { m_matWorld = WorldMatrix; }
	_float Get_MoveScale() const { return m_fMoveScale; }
	void Start_Force(Vec3 vTargetDir, _float fForceAbs, _float fDragK);
	void Apply_Force(_float fDeltaTime, CNavigation* pNavigation = nullptr);
	void Update_PrevPosition();
	void Force_Clear();

public:
	void MoveArgWorld_ToMyWorld(Matrix& vNewWorld, _bool isChangeThis = false);
	void MoveMyWorld_ToArgWorld(Matrix& vNewWorld, _bool isChangeArg = false);
	void Add_Position(const Vec3& vAddPos);
	Vec3 LocalPos_toMyWorld(const Vec3& vLocalPos, _bool bDir);

public:
	_bool Is_OnGround(_float fMaxDist, CPhysics_QueryFilterCallback* pFilterCall = nullptr);

private:
	_bool		m_bControll = { false };
	_float		m_fMoveScale = { 1.f };
	_float		m_fMovePerSec = { 5.f };
	_float		m_fRotatePerSec = { 8.f };
	_float		m_fDragK = { 0.f };
	Vec3		m_vForceVelocity = { 0.f, 0.f, 0.f };
	Vec3		m_vPrevPosition = { 0.f, 0.f, 0.f };
	Matrix		m_matWorld;

	_float		m_fMovePSRate = { 1.f };
public:
	virtual CComponent* Clone(void* pArg) override;
	static CTransform* Create();
	virtual void Free() override;
};

NS_END