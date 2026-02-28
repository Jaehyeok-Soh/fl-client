#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CPhysicsCCT final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::PX_CCT;

private:
	CPhysicsCCT(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPhysicsCCT(const CPhysicsCCT& rhs);
	virtual ~CPhysicsCCT() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Awake();
	void Update();

#ifdef _DEBUG
	void Render();
#endif // _DEBUG

public:
    /// <summary>
    /// 충돌체 높이 조절
    /// </summary>
    /// <param name="height"></param>
    void SetHeight(_float height);

    const PxControllerCollisionFlags Move(Vec3 disp, _float minDist, _float fTimeDelta);

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

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pDeviceContext = { nullptr };

private:
    PxController* m_pController = { nullptr };
    PHYSICSCCT_DESC m_tDesc = {};
    _float m_fHeightOffset = {};
    _float m_fContactOffset = {};

private:
    std::set<CGameObject*> m_setCurContact;
    std::set<CGameObject*> m_setPreContact;

public:
    static CPhysicsCCT* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END