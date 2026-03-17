#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CPhysics_CCTFilterCallback;
class CPhysics_QueryFilterCallback;

class ENGINE_DLL CPhysicsCCT final : public CComponent
{
public:
    typedef struct tagCCTMovementState
    {
        _bool bGravity = { true };
        _float fGravity = { -35.f };
        
        CurMinMax CMSpeed{ 0.f, 0.f, 8.f};
        CurMinMax CMVerticalSpeed{ 0.f, -30.f, 30.f};
        CurMinMax CMAccelRate{ 10.f, 0.f, 10.f };
        CurMinMax CMDeAccelRate{ 5.f, 0.f, 5.f };

        PxVec3 vTargetVelocity{ 0.f, 0.f, 0.f};

        PxVec3 vAccelation{ 0.f, 0.f, 0.f };
        PxVec3 vExternAccelation{ 0.f, 0.f, 0.f };
        PxVec3 vImpulsAccelation{ 0.f, 0.f, 0.f };

        PxVec3 vFixedMove{ 0.f, 0.f, 0.f };

        PxVec3 vVelocity{ 0.f, 0.f, 0.f };

        PxVec3 vInputDir{ 0.f, 0.f, 0.f };

        void ReadyNext()
        {
            vInputDir = { 0.f, 0.f, 0.f };
            vFixedMove = { 0.f, 0.f, 0.f };
            vImpulsAccelation = { 0.f, 0.f, 0.f };
            vExternAccelation = { 0.f, 0.f, 0.f };
        }

    }CCTMOVEMENTSTATE;

	constexpr static EComponentType _ID = EComponentType::PX_CCT;

private:
	using Super = CComponent;

	CPhysicsCCT(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPhysicsCCT(const CPhysicsCCT& rhs);
	virtual ~CPhysicsCCT() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Ready_Position();
	void Update(const _float fTimeDelta);

#ifdef _DEBUG
	void Render();
#endif // _DEBUG

public:
    /// <summary>
    /// 이동량 모아서 한번에 업데이트(move)
    /// </summary>
    void UpdateMove(const _float fTimeDelta);
    void SetInputDir(Vec3 vInputDir);
    
    void AddAccelation(Vec3 vAccelation);
    void AddFixedMove(Vec3 vFixedMove);
    void SetImpulsAccelation(Vec3 vAccelation);

    void SetApplyGravity(_bool bVal) { m_tMoveState.bGravity = bVal; }
    void Set_GravityOffset(_float fOffset) {
        if (fOffset > m_tMoveState.fGravity * -1.f)
            return; 
        m_fGravityOffset = fOffset;  
    }

    void SetZeroVelocity();
    void SetZeroHorizontalVelocity();
    void SetZeroVerticalVelocity();

    void SetZeroDeAccelRate();
    void SetDeAccelRate(_float fRate);
    void ResetDeAccelRate();

    CCTMOVEMENTSTATE* GetMoveState() { return &m_tMoveState; }
    PxControllerCollisionFlags GetCollisionState() { return m_CollisionFlags; }

private:
    void ApplyGravity(const _float fTimeDelta);
    void ApplyExternAcc(const _float fTimeDelta);
    void ApplyImpuls(const _float fTimeDelta);

public:
    const PHYSICSCCT_DESC& Get_Desc() const { return m_tDesc; }
    /// <summary>
    /// 충돌체 높이 조절
    /// </summary>
    /// <param name="height"></param>
    void SetHeight(_float height);

    const PxControllerCollisionFlags Move(PxVec3 disp, _float minDist, _float fTimeDelta);

    void SetPosition(Vec3 position);
    Vec3 GetPosition();

    /// <summary>
    /// 발 위치
    /// </summary>
    /// <returns></returns>
    void SetFootPosition(Vec3 pos);
    Vec3 GetFootPosition();

    /// <summary>
    /// 머리 방향
    /// </summary>
    /// <returns></returns>
    Vec3 GetUpDirection();
    void SetUpDirection(Vec3 dir);

    /// <summary>
    /// 넘어갈 수 있는 최대 계단 높이
    /// </summary>
    /// <returns></returns>
    _float GetStepOffset();
    void SetStepOffset(_float offset);

    /// <summary>
    /// 이동하지 못하는 지형에서 상태 결정
    /// </summary>
    /// <param name="mode">멈추기, 멈추고 미끄러지기</param>
    void SetNonWalkableMode(PxControllerNonWalkableMode::Enum mode);
    PxControllerNonWalkableMode::Enum GetNonWalkableMode();

    /// <summary>
    /// 충돌 감지 여유 값
    /// </summary>
    /// <returns></returns>
    void SetContactOffset(_float offset);
    _float GetContactOffset();

    /// <summary>
    /// 이동 가능한 기울기 한계
    /// </summary>
    /// <param name="limit"></param>
    void SetSlopeLimit(_float limit);
    _float GetSlopeLimit();

    /// <summary>
    /// 캐릭터 컨트롤러는 충돌 테스트 속도를 높이기 위해 캐싱사용
    /// 주변 충돌체 캐싱
    /// </summary>
    void CacheReset();

    /// <summary>
    /// 저장하고 싶은 데이터를 넣는다
    /// </summary>
    /// <param name="data"></param>
    void SetUserData(void* data);

    template<typename T>
    T* GetUserData()
    {
        return static_cast<T*>(m_pController->getUserData());
    }
        
    /// <summary>
    /// 컨트롤러 상태 반환
    /// </summary>
    /// <returns></returns>
    void GetState(PxControllerState& outState);

    /// <summary>
    /// 컨트롤러 새로 만들기
    /// 컨트롤러 해제, 재할당 비용은 높지 않음
    /// </summary>
    void GetController();

    /// <summary>
    /// 컨트롤러 해제
    /// 소유 오브젝트 사망 혹은 물리 처리 필요 없을 시 사용
    /// 컨트롤러 해제, 재할당 비용은 높지 않음
    /// </summary>
    void ReleaseController();

    void SetCollisionFilter();
    void SetCollisionFilter_Empty();

    void SetIsSteppingOnCCT();
    void SetIsSideOnCCT();

    void EnableCollision(_bool bEnable);

    void SetPoolingPosition(Vec3 vPos);
    void ApplyPoolingPosition();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pDeviceContext = { nullptr };

private:
    PxController* m_pController = { nullptr };
    CPhysics_CCTFilterCallback* m_pCCTFilterCallback = { nullptr };
    CPhysics_QueryFilterCallback* m_pQueryFilterCallback = { nullptr };
    PHYSICSCCT_DESC m_tDesc = {};

    PxControllerCollisionFlags m_CollisionFlags{};

    CCTMOVEMENTSTATE m_tMoveState{};

    _float m_fHeightOffset = {};
    _float m_fContactOffset = {};

    Vec3 m_vAccDisp = {};

    _bool m_bIsSteppingOnCCT = { false };
    _bool m_bIsSideOnCCT = { false };

    _float m_fGravityOffset = { 0.f };

    _bool m_bEnableCollision = { true };

private:
    Vec3 m_vPoolingPosition{};

public:
    static CPhysicsCCT* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END