#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPhysics_ResourceManager;
class CPhysics_CCTHitReport;
class CPhysics_CCTBehaviorCallback;
class CPhysics_CCTFilterCallback;

class CPhysics_CCTManager final : public CBase
{
	using Super = CBase;
private:
	CPhysics_CCTManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene, CPhysics_ResourceManager* pResourceManager);
	virtual ~CPhysics_CCTManager() = default;

	HRESULT Initialize();

public:
    /// <summary>
    /// 캐릭터 컨트롤러 팩토리
    /// </summary>
    /// <param name="pDesc"> 캡슐 : radius, height // 박스 : vExtens </param>
    PxController* CreateController(PHYSICSCCT_DESC* pDesc);

    /// <summary>
    /// 캐릭터 끼임 해결 활성/비활성
    /// </summary>
    void SetOverLapRecovery(_bool bFlag);
    /// <summary>
    /// 스윕 테스트(정밀 스캔) 활성/비활성
    /// </summary>
    void SetPreciseSweeps(_bool bFlag);

    _int GetNumCharacterControllers();
    void ReleaseCharacter(PxController* cct);
    void ReleaseCCTManager();

    CPhysics_CCTFilterCallback* GetCCTFilterCallback() { return m_pCCTFilterCallback; }

    PxControllerManager* GetPhysicsCCTManager() { return m_pControllerManager; }

private:
    PxController* MakeBoxController(PHYSICSCCT_DESC* pDesc);
    PxController* MakeCapsuleController(PHYSICSCCT_DESC* pDesc);


private:
    class CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    CPhysics_ResourceManager* m_pResourceManager = { nullptr };

private:
    PxPhysics* m_pPhysics = { nullptr };
    PxScene* m_pScene = { nullptr };
    PxControllerManager* m_pControllerManager = { nullptr };

    CPhysics_CCTHitReport* m_pCCTHitReport = { nullptr };
    CPhysics_CCTBehaviorCallback* m_pCCTBehaviorCallback = { nullptr };
    CPhysics_CCTFilterCallback* m_pCCTFilterCallback = { nullptr };

    Vec3 m_vPoolingPosition = {0.f, -1000.f, 0.f};
    _int m_iPoolingRaw = {};

public:
    static CPhysics_CCTManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene, CPhysics_ResourceManager* pResourceManager);
    virtual void Free();
};

NS_END