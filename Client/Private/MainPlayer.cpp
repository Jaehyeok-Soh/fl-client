#include "pch.h"
#include "MainPlayer.h"
#include "Client_EventDefine.h"

// components
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "PlayerActionState.h"
#include "PlayerControlContext.h"
#include "StatCom_Player.h"
#include "Navigation.h"
#include "Bone.h"
#include "MyStat.h"
#include "ActionSkill.h"
#include "SkillComp_MoonE.h"
#include "SkillComp_MoonQ.h"

// objects
#include "CameraMan_Targeter.h"
#include "Camera.h"
#include "ColliderPart.h"
#include "PhysicsCCT.h"
#include "PhysicsCollider.h"
#include "PhysicsAttackOverlap.h"
#include "Ray.h"
#include "CameraMan.h"
#include "Body.h"
#include "Weapon.h"
#include "Gun.h"

#include "StateBase_Player.h"

#pragma region State
#include "State_MoonCombo.h"

#include "State_JumpAttStart.h"
#include "State_JumpAttEnd.h"

#include "State_Charge.h"
#include "State_MoonCharge.h"

#include "State_GunAttack.h"
#include "State_GunIdle.h"
#include "State_GunReload.h"
#include "State_GunWalk.h"

#include "State_MoonSkill.h"

#pragma endregion
#include "UI_Manager.h"
#include "GameInstance.h"

// Test
#include "ImGui_ClientDebug.h"


CMainPlayer::CMainPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

CMainPlayer::CMainPlayer(const CMainPlayer& rhs)
    : Super(rhs)
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

HRESULT CMainPlayer::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainPlayer::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    Set_Name("Eun_bi_Main");

    if (FAILED(Ready_Ability()))
        return E_FAIL;

    CPlayerControlContext::PLAYER_CONTROLCONTEXT_DESC tDesc = {};
    tDesc.FKeys = CPlayerControlContext::KEYFLAGS::MOVE     | CPlayerControlContext::KEYFLAGS::JUMP
                | CPlayerControlContext::KEYFLAGS::DASH     | CPlayerControlContext::KEYFLAGS::SPECIAL
                | CPlayerControlContext::KEYFLAGS::COMBO    | CPlayerControlContext::KEYFLAGS::SKILL1
                | CPlayerControlContext::KEYFLAGS::SKILL2   | CPlayerControlContext::KEYFLAGS::INTERACT |
                    CPlayerControlContext::KEYFLAGS::GUN;
    if (FAILED(Add_Component<CPlayerControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Player", &tDesc)))
        return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    if (FAILED(Ready_Ray()))
        return E_FAIL;

    if (FAILED(Ready_CCT()))
        return E_FAIL;

    if (FAILED(Ready_AttackOverlap()))
        return E_FAIL;

    if (FAILED(Ready_AttackStates()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainPlayer::Reinitialize(GAMEOBJECT_REINIT_DESC* pDesc)
{
    if (pDesc == nullptr)
        return E_FAIL;

    return S_OK;
}

HRESULT CMainPlayer::Clear_WhenChangeLevel()
{
    m_pTargeter = nullptr;
    Clear_Components_WhenChangeLevel();
    return S_OK;
}

HRESULT CMainPlayer::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    CGameInstance::GetInstance()->Add_Actor_Object(this);

    if (FAILED(Get_Component<CPlayerActionState>()->Change_State(ENUM_TO_UINT(State::IDLE))))
        return E_FAIL;
    if (FAILED(Get_Component<CControlContext>()->Awake(iCurrentLevelID)))
        return E_FAIL;

    //Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, Vec3{ 18.f,30.f,19.f });

    Get_Component<CPhysicsCCT>()->Awake();

    Get_Component<CPhysicsAttackOverlap>()->Awake();

    CImGui_ClientDebug::GetInstance()->Set_Player(this);

    return S_OK;
}

void CMainPlayer::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);

    //Get_Component<CPlayerControlContext>()->Count_Time(fTimeDelta);
}

void CMainPlayer::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    Get_Component<CMyStat>()->Update_Stat(fTimeDelta);
    Get_Component<CActionSkill>()->Update_Skills(fTimeDelta);

}

void CMainPlayer::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
    
    Get_Component<CPhysicsAttackOverlap>()->Update(fTimeDelta);

    if (Get_Component<CPhysicsCCT>())
        Get_Component<CPhysicsCCT>()->Update(fTimeDelta);

    //CPlayerControlContext* pControlContext = Get_Component<CPlayerControlContext>();
    //if (pControlContext == nullptr)
    //    return;

    //if (pControlContext->Is_WallMode())
    //{
    //    Movement_Wall(fTimeDelta);
    //}
    //else
    //    Movement_Ground(fTimeDelta);
}

void CMainPlayer::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

#ifdef _DEBUG
    m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsAttackOverlap>());
    m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsCCT>());
#endif // _DEBUG
}

HRESULT CMainPlayer::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;


    return S_OK;
}

void CMainPlayer::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    COLLIDED_DESC desc{};
    desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_STAY;
    desc.iRequesterLayer = iMyColliderLayer;
    desc.iOtherLayer = iOtherLayer;
    desc.pRequester = this;
    desc.pOther = pOther;
}

void CMainPlayer::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    COLLIDED_DESC desc{};
    desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
    desc.iRequesterLayer = iMyColliderLayer;
    desc.iOtherLayer = iOtherLayer;
    desc.pRequester = this;
    desc.pOther = pOther;
    desc.tHitInfo = tHitInfo;

    desc.tExtraDesc = Get_Component<CStatCom_Player>()->Get_ExtraAttack_Desc();

    m_pGameInstance->Push_CollidedData(desc);
}

void CMainPlayer::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    COLLIDED_DESC desc{};
    desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_EXIT;
    desc.iRequesterLayer = iMyColliderLayer;
    desc.iOtherLayer = iOtherLayer;
    desc.pRequester = this;
    desc.pOther = pOther;
}

void CMainPlayer::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    COLLIDED_DESC desc{};
    desc.iCollisionType = COLLISIONEVENT::ON_TRIGGER_ENTER;
    desc.iRequesterLayer = iMyColliderLayer;
    desc.iOtherLayer = iOtherLayer;
    desc.pRequester = this;
    desc.pOther = pOther;
}

void CMainPlayer::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    COLLIDED_DESC desc{};
    desc.iCollisionType = COLLISIONEVENT::ON_TRIGGER_EXIT;
    desc.iRequesterLayer = iMyColliderLayer;
    desc.iOtherLayer = iOtherLayer;
    desc.pRequester = this;
    desc.pOther = pOther;
}

_bool CMainPlayer::On_Hit(const HIT_DESC& hitDesc)
{
#ifdef _DEBUG
    wstring infoHeader(L"Player Hit ");
    wstring infoSeparate(L": ");
    wstring infoContant = infoHeader
        + infoSeparate
        + Engine_Utils::ToWString(m_strName)
        + infoSeparate
        + std::to_wstring(Get_ID());

    CLOG_INFO(infoContant);
#endif // _DEBUG

    CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>();

    // 만약 현재 state가 attack을 받을 수 있다면
    _uint iStateFlag = pPlayerState->Get_CurrentCapabilities();
    if (Has_Capability(iStateFlag, Engine::StateCapability::BEATTACKED))
    {
        // stat 컴포넌트에 정보 넘겨주기
        _float fDamage = hitDesc.fFinalDamage;
        static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Add_Health(fDamage * -1.f);

        // action state 내부에 set hit desc 넣어주기 : 다음 update때 state에 정보를 주기 위함
        if (pPlayerState)
            pPlayerState->Set_HitDesc(hitDesc);

        // Hit 데미지 폰트 // 색 변경은 가능 //
        {
            UI_PREFAB_DATA tPrefabData = {};
            tPrefabData.DamageFontData.iDamage = fDamage;
            tPrefabData.DamageFontData.vHitPos = hitDesc.vHitPoint;
            CUI_Manager::GetInstance()->Request_Add_Prefab(
                m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_HIT, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
        }
        return true;
    }
   
    return false;
}

void CMainPlayer::Try_Attack(const HIT_DESC& hitDesc)
{
#ifdef _DEBUG
    wstring infoHeader(L"Player Attack ");
    wstring infoSeparate(L": ");
    wstring infoContant = infoHeader
        + infoSeparate
        + Engine_Utils::ToWString(m_strName)
        + infoSeparate
        + std::to_wstring(Get_ID());

    CLOG_INFO(infoContant);
#endif // _DEBUG

    // player state에 따라 combo count 증가 여부 결정
    CStatCom_Player* pStat = Get_Component<CStatCom_Player>();
    switch ((_uint)Get_Component<CActionState>()->Get_CurrentStateIndex())
    {
    case ENUM_TO_UINT(State::COMBO):
    case ENUM_TO_UINT(State::JUMPATTEND):
        pStat->Add_ComboCount();
        break;

    default:
        pStat->Reset_ComboCount();
    }

    // damage 폰트 : iDamageFlag에 따라 크리티컬 || 일반 판정
    switch (hitDesc.iDamageFlag)
    {
    case 0:
        // 일반 공격 데미지 폰트
    {
        UI_PREFAB_DATA tPrefabData = {};
        tPrefabData.DamageFontData.iDamage = hitDesc.fFinalDamage; // 데미지 폰트에 뜰 숫자 // 플레이어 공격력 // 랜덤은 보여주기용
        tPrefabData.DamageFontData.vFontColor = Vec4{ 1.f, 0.95f, 0.47f, 1.f }; // 데미지 폰트 색 // 캐릭터 고유 색
        tPrefabData.DamageFontData.vHitPos = hitDesc.vHitPoint; // 데미지 폰트를 띄울 World 위치 // 
        tPrefabData.DamageFontData.vRandOffset = Vec3{
            m_pGameInstance->Rand_Float(-1.f, 1.f),
            m_pGameInstance->Rand_Float(-1.f, 1.f),
            m_pGameInstance->Rand_Float(-1.f, 1.f) }; // 랜덤 오프셋 // 더 커지면 이상함

        CUI_Manager::GetInstance()->Request_Add_Prefab(
            m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_COMMON, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
    }
    break;
    case 1:
        // 크리티컬 데미지 폰트
    {
        UI_PREFAB_DATA tPrefabData = {};
        tPrefabData.DamageFontData.iDamage = hitDesc.fFinalDamage;
        tPrefabData.DamageFontData.vFontColor = Vec4{ 1.f, 0.95f, 0.47f, 1.f };
        tPrefabData.DamageFontData.vHitPos = hitDesc.vHitPoint;
        tPrefabData.DamageFontData.vRandOffset = Vec3{
            m_pGameInstance->Rand_Float(-1.f, 1.f),
            m_pGameInstance->Rand_Float(-1.f, 1.f),
            m_pGameInstance->Rand_Float(-1.f, 1.f) 
        };
        CUI_Manager::GetInstance()->Request_Add_Prefab(
            m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_CRITICAL, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
    }
    break;
    }

    m_pGameInstance->Broadcast<COMBO_ATTACK_EVENT_START>();
}

#pragma region Legacy
/*
void CMainPlayer::Movement_Ground(const _float fTimeDelta)
{
    CColMesh_Manager* pColMeshManager = { nullptr };
    CBody* pBody = Get_Part<CBody>(Part::BODY);
    CCollider* pCollider = { nullptr };
    CPlayerControlContext* pControlContext = { nullptr };
    CTransform* pTransform = Get_Component<CTransform>();
    CBounding_Sphere* pBounding = { nullptr };
    if (!(pColMeshManager = CColMesh_Manager::GetInstance()))
        return;
    if (!(pCollider = pBody->Get_Component<CCollider>()))
        return;
    if (pCollider->Get_Type() != EColliderType::SPHERE)
        return;
    if (!(pControlContext = Get_Component<CPlayerControlContext>()))
        return;

    pBounding = static_cast<CBounding_Sphere*>(pCollider->Get_Bounding());
    BoundingSphere* pDesc = pBounding->Get_Desc();

    _float fCollideSphereRadius = pDesc->Radius;
   _vector vCollideSphereCneterPos = ::XMVectorSetW(::XMLoadFloat3(&pDesc->Center), 1.f);
    _vector vPrevPosition = ::XMLoadFloat4(&pTransform->Get_PrevPosition());
    _vector vCurrentPosition = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

    // 이전 포지션 Dir 뽑아서 ColMesh와 충돌검사
    _vector vMove = vCurrentPosition - vPrevPosition;
    _vector vMoveXZ = ::XMVectorSetY(vMove, 0.f);
    _float fMoveLength = ::XMVectorGetX(::XMVector3Length(vMoveXZ));

    _vector vNormalieMoveDir = ::XMVector3Normalize(vMoveXZ);
    m_pMoveRay->Setup_Ray(vCollideSphereCneterPos, vNormalieMoveDir);
    _float fRayMaxDistance = fMoveLength + fCollideSphereRadius;

    COLMESH_HITINFO wallHit = {};
    if (fMoveLength > g_XMEpsilon.f[0] &&
        pColMeshManager->Raycast_World(m_pMoveRay, fRayMaxDistance, wallHit) &&
        wallHit.eSurfaceType == ESurfaceType::WALL)
    {
        _float fAllowed = wallHit.fDistance - fCollideSphereRadius;
        if (fAllowed < 0.f)
            fAllowed = 0.f;

        if (fAllowed < fMoveLength)
        {
            const _float fCorrection = fAllowed - fMoveLength;
            const _vector vCorrection = vNormalieMoveDir * fCorrection;

            _vector vFixedPos = vCurrentPosition + vCorrection;
            vFixedPos = ::XMVectorSetY(vFixedPos, ::XMVectorGetY(vCurrentPosition));
            pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vFixedPos);
        }
    }


    pControlContext->Clear_Grounded();
    pControlContext->Set_Gravity(true);
    if (pControlContext->Is_FootRayEnabled() == false)
        return;

    // ========================
    // FootRay
    // ========================
    const _float fMax_StepUp = 0.03f;
    const _float fMax_StepDown = 0.06f;
    const _float fRayLength = fCollideSphereRadius + fMax_StepDown + 0.1f;

    // 한번더 갱신
    vCurrentPosition = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
    vCollideSphereCneterPos = ::XMVectorSetW(::XMLoadFloat3(&pDesc->Center), 1.f);

    // 이미 세팅해둔 ray에 내 world
    m_pFootRay->Setup_Ray(vCollideSphereCneterPos + ::XMVectorSet(0.f, 0.1f, 0.f, 0.f), ::XMVectorSet(0.f, -1.f, 0.f, 0.f));

    COLMESH_HITINFO footHit = {};
    if (pColMeshManager->Raycast_World(m_pFootRay, fRayLength, footHit) &&
        footHit.eSurfaceType == ESurfaceType::GROUND)
    {
        _bool IsGround = { false };
        _float fCurrentY = vCurrentPosition.m128_f32[1];
        _float fGroundY = footHit.vHitPosition.y;
        _float fDeltaY = fGroundY - fCurrentY;

        if (fDeltaY > 0.f)
        {
            if (fDeltaY > fMax_StepUp)
                fDeltaY = fMax_StepUp;

            IsGround = true;
            fCurrentY += fDeltaY;
        }
        else if (fDeltaY >= -fMax_StepDown)
        {
            IsGround = true;
            fCurrentY = std::_Common_lerp(fCurrentY, fGroundY, 0.3f);
        }
        else
            IsGround = false;

        if (IsGround)
        {
            pControlContext->Set_Grounded(true, &footHit);
            pControlContext->Set_Gravity(false);
            vCurrentPosition = ::XMVectorSetY(vCurrentPosition, fCurrentY);
            pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vCurrentPosition);
        }
    }
}
*/
#pragma endregion

#pragma region Legacy2
/*
_bool CMainPlayer::Try_AttackHit(ECollideLayer eMyLayer, CCollider* pOther)
{
    if (Is_PlayerAttackLayer(eMyLayer) == false)
        return false;

    if (pOther == nullptr)
        return false;

    ECollideLayer eOtherLayer = static_cast<ECollideLayer>(pOther->Get_Layer());
    if (eOtherLayer != ECollideLayer::ENEMY_BODY)
        return false;

    CPartObject* pAttackPart = Get_PartByLayer(eMyLayer);
    if (pAttackPart == nullptr || pAttackPart->Is_AttackWindow() == false)
        return false;

    ATTACK_DESC* pDesc = pAttackPart->Get_AttackDesc();

    if (pDesc->iAttackType < 0)
        return false;

    CGameObject* pVictim = pOther->Get_Owner();

    if (pAttackPart->Is_AlreadyHitted(pVictim))
        return false;

    if (pVictim->On_Hit(static_cast<_uint>(pOther->Get_Layer()), pDesc, this) == false)
        return false;

    if (pDesc->bImpact)
    {
        CCameraMan_Targeter* pCamera = static_cast<CCameraMan_Targeter*>(Get_CameraTargeter());
        pCamera->Change_State(TargeterState::TARGETSYNC);
    }
    pAttackPart->Regist_HitTargets(pVictim);
    return true;
}
*/
#pragma endregion


HRESULT CMainPlayer::Ready_Ability()
{
    CSkillBase* pESkill = CSkillComp_MoonE::Create();
    CSkillBase* pQSkill = CSkillComp_MoonQ::Create();

    // stat
    {
        CStatCom_Player::PLAYER_STATCOMP_DESC desc = {};
        desc.fMaxHp = 320.f;
        desc.fDefense = 400.f;
        desc.fMental = 105.f;
        desc.FStatFlags = CStatCom_Player::StatFlags::DefenseUpdtae | CStatCom_Player::StatFlags::MentalUpdate;

        desc.fComboCoolTime = 2.f;
        desc.fDashCoolTime = 2.f;

        desc.fMeleeAttack = 20.f;
        desc.fGunAttack = 20.f;

        desc.pESkill = pESkill;
        desc.pQSkill = pQSkill;

        vector<_uint> vecComputeOrder;
        {
            vecComputeOrder.resize(4);
            vecComputeOrder[0] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Normal_Add);
            vecComputeOrder[1] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Random_Add);
            vecComputeOrder[2] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Normal_Rate);
            vecComputeOrder[3] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Random_Rate);
        }

        desc.vecExtraComputeOrder = vecComputeOrder;
        desc.fCriticalAttack = 30.f;
        desc.fCriticalRate = 0.3f;

        if (FAILED(Add_Component<CMyStat>(0/* STATIC */, L"Prototype_Component_Stat_Player", &desc)))
            return E_FAIL;
    }

    // skill
    {
        CActionSkill::ACTIONSKILL_DESC desc = {};
        desc.iSkillCount = 2;
        desc.pOwnerStat = Get_Component<CMyStat>();

        if (FAILED(Add_Component<CActionSkill>(0/* STATIC */, L"Prototype_Component_ActionSkill", &desc)))
            return E_FAIL;

        CActionSkill* pActionSkill = Get_Component<CActionSkill>();

        if (FAILED(pActionSkill->Add_Skill(Skill::MoonE, pESkill)))
            return E_FAIL;
        if (FAILED(pActionSkill->Add_Skill(Skill::MoonQ, pQSkill)))
            return E_FAIL;
    }

    Start_Attack(State::COMBO);




    //CPlayerActionState* pActionState = { nullptr };
    //CModel* pModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();

    //if (!pModel)
    //    return E_FAIL;

    //if (!(pActionState = Get_Component<CPlayerActionState>()))
    //    return E_FAIL;

    //// skill components
    //{
    //    if (FAILED(Add_Script_Component(L"SkillComponent_E", L"Prototype_Component_Skill_MoonE", nullptr)))
    //        return E_FAIL;

    //    if (FAILED(Add_Script_Component(L"SkillComponent_Q", L"Prototype_Component_Skill_MoonQ", nullptr)))
    //        return E_FAIL;

    //    m_pSkillEComp = static_cast<CSkillComponent*>(Get_Script_Component(L"SkillComponent_E"));
    //    m_pSkillQComp = static_cast<CSkillComponent*>(Get_Script_Component(L"SkillComponent_Q"));
    //}

    //{


    //    if (FAILED(Add_Script_Component(L"StatComponent", L"Prototype_Component_Stat_Player", &desc)))
    //        return E_FAIL;

    //    m_pStatComp = static_cast<CStatComponent*>(Get_Script_Component(L"StatComponent"));
    //}    

    //// HandCombo_Right
    //{
    //    CComboContainer* pContainer = CComboContainer::Create(pActionState, "RightHand");
    //    // ComboFirst
    //    {
    //        CState_Combo_First::COMBOSTATE_DESC desc = {};
    //        desc.bBlend = true;
    //        desc.bLeftMouse = true;
    //        desc.fChanceTime_Start = 0.6f;
    //        desc.fChanceTime_End = 0.7f;
    //        desc.iAnimIndex = Get_AnimationIndex(L"Animation_Master_ComboRight01");
    //        if (FAILED(pContainer->Add_Comobo(CState_Combo_First::Create(pActionState, &desc))))
    //            return E_FAIL;
    //    }
    //    // ComboSecond
    //    {
    //        CState_Combo_Second::COMBOSTATE_DESC desc = {};
    //        desc.bBlend = true;
    //        desc.bLeftMouse = true;
    //        desc.fChanceTime_Start = 0.5f;
    //        desc.fChanceTime_End = 0.7f;
    //        desc.iAnimIndex = Get_AnimationIndex(L"Animation_Master_ComboRight02");
    //        if (FAILED(pContainer->Add_Comobo(CState_Combo_Second::Create(pActionState, &desc))))
    //            return E_FAIL;
    //    }
    //    // ComboThird
    //    {
    //        CState_Combo_Third::COMBOSTATE_DESC desc = {};
    //        desc.bBlend = true;
    //        desc.bLeftMouse = true;
    //        desc.fChanceTime_Start = 0.6f;
    //        desc.fChanceTime_End = 0.7f;
    //        desc.iAnimIndex = Get_AnimationIndex(L"Animation_Master_ComboRight03");
    //        if (FAILED(pContainer->Add_Comobo(CState_Combo_Third::Create(pActionState, &desc))))
    //            return E_FAIL;
    //    }
    //    // ComboFourth
    //    {
    //        CState_Combo_Fourth::COMBOSTATE_DESC desc = {};
    //        desc.bBlend = true;
    //        desc.bLeftMouse = true;
    //        desc.fChanceTime_Start = 0.5f;
    //        desc.fChanceTime_End = 0.7f;
    //        desc.iAnimIndex = Get_AnimationIndex(L"Animation_Master_ComboRight04");
    //        if (FAILED(pContainer->Add_Comobo(CState_Combo_Fourth::Create(pActionState, &desc))))
    //            return E_FAIL;
    //    }

    //    pActionState->Add_State(ENUM_TO_UINT(CPlayer::State::LEFTMELEE), pContainer);
    //}
    return S_OK;
}

HRESULT CMainPlayer::Ready_Colliders()
{
    CBody* pBody = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY));
    if (!pBody)
        return E_FAIL;
    
    //CWeapon* pWeapon = Get_Part<CWeapon>(ENUM_TO_UINT(Part::WEAPON));
    //if (!pWeapon)
    //    return E_FAIL;

    //CColliderPart* pLeftHand = Get_Part<CColliderPart>(ENUM_TO_UINT(Part::LEFTHAND));
    //if (!pLeftHand)
    //    return E_FAIL;

    //CColliderPart* pRightHand = Get_Part<CColliderPart>(ENUM_TO_UINT(Part::RIGHTHAND));
    //if (!pRightHand)
    //    return E_FAIL;

    //CColliderPart* pLeftFoot = Get_Part<CColliderPart>(ENUM_TO_UINT(Part::LEFTFOOT));
    //if (!pLeftFoot)
    //    return E_FAIL;

    //CColliderPart* pRightFoot = Get_Part<CColliderPart>(ENUM_TO_UINT(Part::RIGHTFOOT));
    //if (!pRightFoot)
    //    return E_FAIL;

    //// Body
    //{
    //    CCollider::COLLIDER_DESC colliderDesc = {};
    //    CBounding_Sphere::BOUNDING_SPHERE_DESC boundingDesc = {};
    //    colliderDesc.iLayer = ENUM_TO_UINT(ECollideLayer::PLAYER_BODY);
    //    boundingDesc.fRadius = 0.7f;
    //    boundingDesc.vCenter = { 0.f, boundingDesc.fRadius, 0.f };
    //    colliderDesc.pBoundingDesc = &boundingDesc;
    //    if (FAILED(pBody->Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_Sphere", &colliderDesc)))
    //        return E_FAIL;
    //}

    //// Weapon
    //{
    //    CCollider::COLLIDER_DESC colliderDesc = {};
    //    CBounding_Sphere::BOUNDING_SPHERE_DESC boundingDesc = {};
    //    colliderDesc.iLayer = ENUM_TO_UINT(ECollideLayer::PLAYER_WEAPON);
    //    boundingDesc.fRadius = 0.4f;
    //    boundingDesc.vCenter = { 0.f, -boundingDesc.fRadius, 0.f };
    //    colliderDesc.pBoundingDesc = &boundingDesc;
    //    if (FAILED(pWeapon->Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_Sphere", &colliderDesc)))
    //        return E_FAIL;
    //}

    //// LeftHand
    //{
    //    CColliderPart* pLeftHand = Get_Part<CColliderPart>(ENUM_TO_UINT(Part::LEFTHAND));
    //    if (!pLeftHand)
    //        return E_FAIL;

    //    CCollider::COLLIDER_DESC colliderDesc = {};
    //    CBounding_Sphere::BOUNDING_SPHERE_DESC boundingDesc = {};
    //    colliderDesc.iLayer = ENUM_TO_UINT(ECollideLayer::PLAYER_LEFTHAND);
    //    boundingDesc.fRadius = 0.3f;
    //    boundingDesc.vCenter = { 0.f, 0.f, 0.f };
    //    colliderDesc.pBoundingDesc = &boundingDesc;
    //    if (FAILED(pLeftHand->Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_Sphere", &colliderDesc)))
    //        return E_FAIL;
    //}

    //// RightHand
    //{
    //    CCollider::COLLIDER_DESC colliderDesc = {};
    //    CBounding_Sphere::BOUNDING_SPHERE_DESC boundingDesc = {};
    //    colliderDesc.iLayer = ENUM_TO_UINT(ECollideLayer::PLAYER_RIGHTHAND);
    //    boundingDesc.fRadius = 0.3f;
    //    boundingDesc.vCenter = { 0.f, 0.f, 0.f };
    //    colliderDesc.pBoundingDesc = &boundingDesc;
    //    if (FAILED(pRightHand->Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_Sphere", &colliderDesc)))
    //        return E_FAIL;
    //}

    //// LeftFoot
    //{
    //    CCollider::COLLIDER_DESC colliderDesc = {};
    //    CBounding_Sphere::BOUNDING_SPHERE_DESC boundingDesc = {};
    //    colliderDesc.iLayer = ENUM_TO_UINT(ECollideLayer::PLAYER_LEFTFOOT);
    //    boundingDesc.fRadius = 0.3f;
    //    boundingDesc.vCenter = { 0.f, 0.f, 0.f };
    //    colliderDesc.pBoundingDesc = &boundingDesc;
    //    if (FAILED(pLeftFoot->Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_Sphere", &colliderDesc)))
    //        return E_FAIL;
    //}

    //// RightFoot
    //{
    //    CCollider::COLLIDER_DESC colliderDesc = {};
    //    CBounding_Sphere::BOUNDING_SPHERE_DESC boundingDesc = {};
    //    colliderDesc.iLayer = ENUM_TO_UINT(ECollideLayer::PLAYER_RIGHTFOOT);
    //    boundingDesc.fRadius = 0.45f;
    //    boundingDesc.vCenter = { 0.f, 0.f, 0.f };
    //    colliderDesc.pBoundingDesc = &boundingDesc;
    //    if (FAILED(pRightFoot->Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_Sphere", &colliderDesc)))
    //        return E_FAIL;
    //}

    //m_pGameInstance->Register_Collider(pBody->Get_Component<CCollider>());
    //m_pGameInstance->Register_Collider(pWeapon->Get_Component<CCollider>());
    //m_pGameInstance->Register_Collider(pLeftHand->Get_Component<CCollider>());
    //m_pGameInstance->Register_Collider(pRightHand->Get_Component<CCollider>());
    //m_pGameInstance->Register_Collider(pLeftFoot->Get_Component<CCollider>());
    //m_pGameInstance->Register_Collider(pRightFoot->Get_Component<CCollider>());
    //pWeapon->Get_Component<CCollider>()->Set_Active(false);
    //pLeftHand->Get_Component<CCollider>()->Set_Active(false);
    //pRightHand->Get_Component<CCollider>()->Set_Active(false);
    //pLeftFoot->Get_Component<CCollider>()->Set_Active(false);
    //pRightFoot->Get_Component<CCollider>()->Set_Active(false);
    return S_OK;
}

HRESULT CMainPlayer::Ready_Ray()
{
    if (!(m_pFootRay = CRay::Create(Vec3{ 0.f, 0.1f, 0.f }, Vec3{ 0.f, -1.f, 0.f })))
        return E_FAIL;

    if (!(m_pMoveRay = CRay::Create(Vec3{ 0.f, 0.05f, 0.f }, Vec3{ 0.f, 0.f, 0.f })))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainPlayer::Ready_CCT()
{
    PHYSICSCCT_DESC desc;
    desc.pOwner = this;
    desc.bIsPlayer = true;
    desc.eType = EPhysicsCCTType::CAPSULE;
    desc.pOwnerMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
    desc.fRadius = 0.35f;
    desc.fHeight = 0.7f;
    desc.vExtens = { 0.f, 0.f, 0.f };

    PHYSICSMATERIAL_DESC mtrlDesc{};
    mtrlDesc.eMaterial = EPhysicsMaterial::ICE;
    desc.tMaterial = mtrlDesc;

    desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::PLAYER;
    desc.iFilterMask =
        PHYSICSFILTERGROUP::Enum::PLAYER
        | PHYSICSFILTERGROUP::Enum::MONSTER
        | PHYSICSFILTERGROUP::Enum::MONSTER_ATTACK
        | PHYSICSFILTERGROUP::Enum::MONSTER_ATTACK_PROJECTTILE
        | PHYSICSFILTERGROUP::Enum::MONSTER_SKILL
        | PHYSICSFILTERGROUP::Enum::MONSTER_SKILL_PROJECTTILE
        | PHYSICSFILTERGROUP::Enum::MAP
        | PHYSICSFILTERGROUP::Enum::OBJECT1
        | PHYSICSFILTERGROUP::Enum::OBJECT2
        | PHYSICSFILTERGROUP::Enum::TRIGGER_UI
        | PHYSICSFILTERGROUP::Enum::TRIGGER_QUEST
        | PHYSICSFILTERGROUP::Enum::TRIGGER_SPAWN
        | PHYSICSFILTERGROUP::Enum::TRIGGER_DIRECTION
        | PHYSICSFILTERGROUP::Enum::TRIGGER_BOX;

    if (FAILED(Add_Component<CPhysicsCCT>(0, L"Prototype_Component_Physics_CCT", &desc)))
        return E_FAIL;

    {
        // CCT, PhysicsCollider 세트
        PHYSICSCOLLIDER_DESC cloneDesc{};
        cloneDesc.eFilterLayer = desc.eFilterLayer;
        cloneDesc.iFilterMask = desc.iFilterMask;
        cloneDesc.bSetOnlyFilter = true;

        if (FAILED(Add_Component<CPhysicsCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_Collider", &cloneDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CMainPlayer::Ready_AttackOverlap()
{
    if (FAILED(Add_Component<CPhysicsAttackOverlap>(0, L"Prototype_Component_AttackOverlap_PlayerMoon", nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainPlayer::Ready_AttackStates()
{
    CPlayerActionState* pActionState = { nullptr };
    CModel* pModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
    if (!pModel)
        return E_FAIL;

    CComputeShader* pAnimMixCS = static_cast<CBody*>(Get_Part<CBody>(ENUM_TO_UINT(Part::BODY)))->Get_AnimMixCS();

    if (!(pActionState = Get_Component<CPlayerActionState>()))
        return E_FAIL;

    CGun* pMyGun = static_cast<CGun*>(Get_Part<CWeapon>(ENUM_TO_UINT(Part::GUN)));
    if(!pMyGun)
        return E_FAIL;

    vector<_uint> vecChangeState_ByKey{};
    vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), ENUM_TO_UINT(State::END));

    TIME_COUNTER tKeyTimer = {};
    tKeyTimer.bCountTime = false;
    tKeyTimer.bTimeReset = false;

    // combo state
    {
        CState_MoonCombo::MOONCOMBO_DESC tDesc = {};
        tDesc.vCombo_CheckTimes = Vec4{ 0.5f,0.5f,1.f,1.5f };
        tDesc.fSlide_CheckTime = 0.7f;

        tDesc.iSlideAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_SlideAttack");
        tDesc.iFirstAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_01");
        tDesc.iSecondAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_02");
        tDesc.iThirdAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_03");
        tDesc.iFourthAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_04");
        tDesc.iEndStateIndex = ENUM_TO_UINT(State::END);
        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::COMBO), CState_MoonCombo::Create(pActionState, &tDesc))))
            return E_FAIL;
    }


    // JumpAttStart
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        desc.vecPreAnims = {
                                {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_Start")}
        };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_Loop") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend = false;
        desc.bLoop = true;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;
        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        desc.FCollis = 0;

        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPATTSTART), CState_JumpAttStart::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JumpAttEnd
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_End") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;
        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        desc.FCollis = 0;

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)] = ENUM_TO_UINT(CPlayer::State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)] = ENUM_TO_UINT(CPlayer::State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;


        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 0.65f ;
        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPATTEND), CState_JumpAttEnd::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Charge
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni | CStateBase::STATEANI_FLAG::SA_PreNonEvent;
        desc.vecPreAnims = { {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_Start")}};
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_End") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        desc.FCollis = 0;

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(CPlayer::State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(CPlayer::State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 0.8f;
        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CHARGE), CState_MoonCharge::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // skill1
    {
        CState_SkillBase::Skill_DESC tDesc = {};
        tDesc.bKeyInput = true;
        tDesc.fKeyCoolTime = 1.3f;
        tDesc.iAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Light_Skill01");
        tDesc.iPlayerState = ENUM_TO_UINT(State::SKILL1);

        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::SKILL1), CState_MoonSkill::Create(pActionState, "SkillE", &tDesc))))
            return E_FAIL;
    }

    // skill2
    {
        CState_SkillBase::Skill_DESC tDesc = {};
        tDesc.bKeyInput = true;
        tDesc.fKeyCoolTime  = 4.5f;
        tDesc.iAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Light_Skill02_Red");
        tDesc.iPlayerState = ENUM_TO_UINT(State::SKILL2);
        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::SKILL2), CState_MoonSkill::Create(pActionState,"SkillQ", & tDesc))))
            return E_FAIL;
    }

    // gun idle
    {
        CStateBase_Player::PLAYER_STATE_SPECIFICDESC tDesc = {};
        tDesc.bBlend = true;
        tDesc.bLoop = true;
        tDesc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Holding_Loop") };
        tDesc.pOwnerGun = pMyGun;

        tDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNIDLE), CState_GunIdle::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    // gun walk
    {
        CStateBase_Player::PLAYER_STATE_SPECIFICDESC tDesc = {};
        tDesc.bBlend = true;
        tDesc.bLoop = true;
        tDesc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Holding_Run_Loop") };
        tDesc.pOwnerGun = pMyGun;

        tDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNWALK), CState_GunWalk::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    array<_uint, ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::END)> arrMix;
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::F)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_F");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::B)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_B");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::L)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_l");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::R)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_R");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::LF)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_LF");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::LB)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_LB");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::RF)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_RF");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::RB)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_RB");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::JUMP)] = Get_AnimationIndex(L"Animation_PlayerMoon_FirstJump_InplaceStart");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::FALL)] = Get_AnimationIndex(L"Animation_PlayerMoon_Jump_FallLoop");

    vector<CModel::DATA_ANIMIX> vecDownMix = { {304,true,1.f},{329,true,1.f},{378,true,1.f} };

    for (auto& MixAnim : arrMix)
    {
        pModel->Make_MixRatio(MixAnim, vecDownMix, pAnimMixCS);
    }

    // gun attack
    {
        CState_GunBase::GUN_STATEBASE_DESC tDesc = {};

        tDesc.arrMixAnims = arrMix;

        tDesc.bLoop = true;
        tDesc.pOwnerGun = pMyGun;
        tDesc.iMainAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Machinegun01_Shooting_Loop");



        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNATTACK), CState_GunAttack::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    // gun reload
    {
        CState_GunBase::GUN_STATEBASE_DESC tDesc = {};

        tDesc.arrMixAnims = arrMix;

        tDesc.bLoop = false;
        tDesc.pOwnerGun = pMyGun;
        tDesc.iMainAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Machinegun01_Reload");

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNRELOAD), CState_GunReload::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    return S_OK;
}

CMainPlayer* CMainPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CMainPlayer* pInsatnce = new CMainPlayer(pDevice, pDeviceContext);
    if (FAILED(pInsatnce->Initialize_Prototype()))
    {
        MSG_BOX("CMainPlayer::Create, Failed");
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

CGameObject* CMainPlayer::Clone(void* pArg)
{
    CMainPlayer* pClone = new CMainPlayer(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CMainPlayer::Clone, Failed");
        Safe_Release(pClone);
    }
    return pClone;
}

void CMainPlayer::Free()
{
    CImGui_ClientDebug::GetInstance()->Set_Player(nullptr);
    Safe_Release(m_pMoveRay);
    Safe_Release(m_pFootRay);
    Super::Free();
}
