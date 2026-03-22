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
#include "SkillBase_MoonE.h"
#include "SkillBase_MoonQ.h"
#include "RenderFx.h"

// objects
#include "CameraMan_Targeter.h"
#include "Camera.h"
#include "TriggerCollidePart.h"
#include "PhysicsCCT.h"
#include "PhysicsCollider.h"
#include "PhysicsAttackOverlap.h"
#include "EffectHandler.h"
#include "Ray.h"
#include "CameraMan.h"
#include "Body.h"
#include "Weapon.h"
#include "Gun.h"
#include "BonePart.h"

#include "StateBase_Player.h"

#pragma region State
#include "State_MoonCombo.h"
#include "State_DualCombo.h"

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
#include "UIMinimap_Manager.h"
#include "UI_Manager.h"
#include "SoundEventBinder.h"
#include "GameInstance.h"

// Test
#include "ImGui_ClientDebug.h"

#define ANIMTIC (24.f * 1.2f)


CMainPlayer::CMainPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext), m_isCinematic{false}
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

CMainPlayer::CMainPlayer(const CMainPlayer& rhs)
    : Super(rhs) , m_isCinematic(rhs.m_isCinematic)
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
    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
    m_ePlayerType = pDesc->ePlayerType;

    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    Set_Name("Eun_bi_Main");

    if (FAILED(Ready_Ability()))
        return E_FAIL;

    CPlayerControlContext::PLAYER_CONTROLCONTEXT_DESC tDesc = {};
    tDesc.FKeys = CPlayerControlContext::KEYFLAGS::MOVE     | CPlayerControlContext::KEYFLAGS::JUMP
                | CPlayerControlContext::KEYFLAGS::DASH     | CPlayerControlContext::KEYFLAGS::SPECIAL
                | CPlayerControlContext::KEYFLAGS::COMBO    | CPlayerControlContext::KEYFLAGS::SKILL1
                | CPlayerControlContext::KEYFLAGS::SKILL2   | CPlayerControlContext::KEYFLAGS::INTERACT 
                | CPlayerControlContext::KEYFLAGS::GUN;

    if (FAILED(Add_Component<CPlayerControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Player", &tDesc)))
        return E_FAIL;

    if (FAILED(Ready_Ray()))
        return E_FAIL;

    if (FAILED(Ready_CCT()))
        return E_FAIL;

    if (FAILED(Ready_AttackOverlap()))
        return E_FAIL;

    if (FAILED(Ready_AttackStates()))
        return E_FAIL;

    if (FAILED(Ready_EffectEvent()))
        return E_FAIL;

    if (FAILED(Ready_SoundHandler()))
        return E_FAIL;

    Get_Component<CPhysicsAttackOverlap>()->Bind_Events();
    Get_Component<CEffectHandler>()->Setup_ForOwner(this, Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>());

    return S_OK;
}

HRESULT CMainPlayer::Register_GlobalEvent()
{

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

    // part obj의 clear 함수 호출
    for (auto& pPartObj : m_vecPartObjects)
    {
        if (pPartObj)
            pPartObj->Clear_WhenChangeLevel();
    }

    // LoadingScene에서 비활성화
    Set_Active(false);
    Set_CollideEnabled(false);
    Set_Render(false);
    return S_OK;
}

HRESULT CMainPlayer::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    // 로딩씬 제외 매씬 Awake 호출시 활성화
    if (iCurrentLevelID != ENUM_TO_UINT(ELevelType::LOADING))
    {
        Set_Active(true);
        Set_CollideEnabled(true);
        Set_Render(true);
    }

    CGameInstance::GetInstance()->Add_Actor_Object(this);

    if (FAILED(Change_IdleForce()))
        return E_FAIL;

    if (FAILED(Get_Component<CControlContext>()->Awake(iCurrentLevelID)))
        return E_FAIL;

    if (FAILED(Register_GlobalEvent()))
        return E_FAIL;

    Get_Component<CPhysicsCCT>()->Ready_Position();
    Get_Component<CMyStat>()->Fill_StatFull(CMyStat::STAT_TYPE::HP);
    Get_Component<CMyStat>()->Fill_StatFull(CMyStat::STAT_TYPE::MENTAL);
    Get_Component<CMyStat>()->Fill_StatFull(CMyStat::STAT_TYPE::DEFENSE);

#ifdef _DEBUG
    CImGui_ClientDebug::GetInstance()->Set_Player(this);
#endif
    return S_OK;
}

void CMainPlayer::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);

    if (KEY_BUTTON_DOWN(DIK_C))
    {
        static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Toggle_Invincible();

        Set_WepaponOn(ENUM_TO_UINT(EWEAPON::MELEE), 0, true);
        Change_MainWeapon(ENUM_TO_UINT(EWEAPON::MELEE), 0);
    }

    if (KEY_BUTTON_DOWN(DIK_B))
    {
        Set_WepaponOn(ENUM_TO_UINT(EWEAPON::MELEE), 1, true);
        Change_MainWeapon(ENUM_TO_UINT(EWEAPON::MELEE), 1);
    }

    if (KEY_BUTTON_DOWN(DIK_N))
    {
        Set_WepaponOn(ENUM_TO_UINT(EWEAPON::RANGE), 0, true);
    }

    if (KEY_BUTTON_DOWN(DIK_M))
    {
        Set_WepaponOn(ENUM_TO_UINT(EWEAPON::SKILL), 0, true);
    }

    //Get_Component<CPlayerControlContext>()->Count_Time(fTimeDelta);
}

void CMainPlayer::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    Get_Component<CMyStat>()->Update_Stat(fTimeDelta);
    Get_Component<CActionSkill>()->Update_Skills(fTimeDelta);

    if (m_pGameInstance->KeyButton_Down(DIK_T))
    {
        SimpleMath::Vector4 HitPosition = { 17.f, 17.f, 17.f, 1.f};

        EFFECT_SPAWN_DESC desc = {};
        desc.VFX_fSpeed = 1.f;
        desc.iSimulationType = (_uint)E_EFFECT_TYPE::WORLD;
        desc.matWorld = XMMatrixTranslationFromVector(HitPosition);

        m_pGameInstance->Request_Effect("VFX_Sword_Hit", desc);
    }
}

void CMainPlayer::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
    
    Get_Component<CPhysicsAttackOverlap>()->Update(fTimeDelta);

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
    this->Super::Ready_Before_Render(fTimeDelta);

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

void CMainPlayer::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    switch (iMyColliderLayer)
    {
    case ENUM_TO_UINT(PHYSICSFILTERGROUP::Enum::DETECT_MONSTER):
        {
            CUIMinimap_Manager::GetInstance()->Add_Ranged_Object(pOther, EUIMinimapIconTypeID::MONSTER);
        }
        break;
    }
}

void CMainPlayer::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    COLLIDED_DESC desc{};
    desc.iCollisionType = COLLISIONEVENT::ON_TRIGGER_EXIT;
    desc.iRequesterLayer = iMyColliderLayer;
    desc.iOtherLayer = iOtherLayer;
    desc.pRequester = this;
    desc.pOther = pOther;

    switch (iMyColliderLayer)
    {
    case ENUM_TO_UINT(PHYSICSFILTERGROUP::Enum::DETECT_MONSTER):
    {
        CUIMinimap_Manager::GetInstance()->Delete_Ranged_Object(pOther);
    }
    break;
    }
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
    CTransform*         pTransform = Get_Component<CTransform>();
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
            UI_DAMAGEFONT_PREFAB_DATA Desc = {};
            Desc.iDamage = static_cast<_uint>(fDamage);
            if (hitDesc.bHasHitPoint)
                Desc.vHitPos = hitDesc.vHitPoint;
            else
            {
                Desc.vHitPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
                Desc.vHitPos.y += 0.4f;
            }
                
            tPrefabData.Data = Desc;
            CUI_Manager::GetInstance()->Request_Add_Prefab(
                m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_HIT, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
        }

        //if(pPlayerState->Get_AttackFlag())

        // Shake & Emissive
        if (CBody* pBody = Get_Part<CBody>(Part::BODY))
        {
            CRenderFx* pRenderFx = pBody->Get_Component<CRenderFx>();
            pRenderFx->Play_Shake(0.35f);
            pRenderFx->Play_EmissivePulse(0.05f, 0.08f, 0.18f);

        }

        // Shake & Emissive
        if (CBonePart* pCloak = Get_Part<CBonePart>(Part::CLOAK))
        {
            CRenderFx* pRenderFx = pCloak->Get_Component<CRenderFx>();
            pRenderFx->Play_Shake(0.35f);
            pRenderFx->Play_EmissivePulse(0.05f, 0.08f, 0.18f);
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

    Get_Component<CPlayerControlContext>()->Set_AttackLanded();

    if (hitDesc.pVictim->IsAlive())
    {
        Count_Combo();
    }
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
    CSkillBase* pESkill = CSkillBase_MoonE::Create();
    CSkillBase* pQSkill = CSkillBase_MoonQ::Create();

    // stat
    {
        CStatCom_Player::PLAYER_STATCOMP_DESC desc = {};
        desc.fMaxHp     = 320.f;
        desc.fDefense   = 400.f;
        desc.fMental    = 105.f;
        desc.FStatFlags = CStatCom_Player::StatFlags::DefenseUpdtae | CStatCom_Player::StatFlags::MentalUpdate;

        desc.fComboCoolTime     = 7.f;
        desc.fDashCoolTime      = 2.f;

        desc.fMeleeAttack       = 20.f;
        desc.fGunAttack         = 20.f;

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
        desc.fCriticalAttack    = 30.f;
        desc.fCriticalRate      = 0.1f;

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
    desc.fRadius = 0.25f;
    desc.fHeight = 0.7f;
    desc.vExtens = { 0.f, 0.f, 0.f };

    PHYSICSMATERIAL_DESC mtrlDesc{};
    mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
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
        | PHYSICSFILTERGROUP::Enum::TRIGGER_BOX
        | PHYSICSFILTERGROUP::Enum::NPC;

    desc.bGravity = { true };
    desc.fGravity = { -35.f };
    desc.MSpeed     = { 0.f, 5.f };
    desc.MAccelRate = { 0.f, 10.f };
    desc.MDeAccelRate = { 0.f, 20.f };

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


HRESULT CMainPlayer::Ready_EffectEvent()
{
    if (FAILED(Add_Component<CEffectHandler>(0, L"Prototype_Component_EffectHandler_PlayerMoon", nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainPlayer::Ready_SoundHandler()
{
    _uint iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
    CBody* pBody = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY));
    if (pBody == nullptr)
        return E_FAIL;
    CModel* pAnimModel = pBody->Get_Component<CModel>();
    if (pAnimModel == nullptr)
        return E_FAIL;
    // 내부에서 Add_Component 해줌
    CSoundEventBinder* pResult = CSoundEventBinder::Create(iLevelID, this, pAnimModel, L"../../Resources/Data/SoundAnimationData/Example.json");
    if (pResult == nullptr)
        return E_FAIL;
    Safe_Release(pResult);
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

    // combo state : moon
    {
        CState_MoonCombo::MOONCOMBO_DESC tDesc = {};
        tDesc.vCombo_CheckTimes = Vec4{ 15.f/ ANIMTIC ,15.f / ANIMTIC,20.f / ANIMTIC ,25.f / ANIMTIC };
        tDesc.fSlide_CheckTime = 15.f / ANIMTIC;

        _int iSlide = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_SlideAttack");
        _int iCombo1 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_01");
        _int iCombo2 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_02");
        _int iCombo3 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_03");
        _int iCombo4 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_04");

        tDesc.arrCombo_EndTimes = { 50.f / ANIMTIC ,33.f / ANIMTIC,39.f / ANIMTIC ,60.f / ANIMTIC ,70.f / ANIMTIC };

        tDesc.iSlideAnimIdx = iSlide;
        tDesc.iFirstAnimIdx = iCombo1;
        tDesc.iSecondAnimIdx = iCombo2;
        tDesc.iThirdAnimIdx  = iCombo3;
        tDesc.iFourthAnimIdx = iCombo4;
        tDesc.iEndStateIndex = ENUM_TO_UINT(State::END);
        tDesc.pOwnerGun      = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::COMBO), CState_MoonCombo::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    // combo state : dual
    {
        CState_DualCombo::DUALCOMBO_DESC tDesc = {};
        tDesc.vCombo_CheckTimes = Vec4{ 17.f / ANIMTIC ,15.f / ANIMTIC, 10.f / ANIMTIC ,24.f / ANIMTIC };
        tDesc.fSlide_CheckTime = 17.f / ANIMTIC;

        _int iSlide = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_SlideAttack");
        _int iCombo1 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_01");
        _int iCombo2 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_02");
        _int iCombo3 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_03");
        _int iCombo4 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_04");

        tDesc.arrCombo_EndTimes = { 55.f / ANIMTIC ,51.f / ANIMTIC,50.f / ANIMTIC ,60.f / ANIMTIC ,75.f / ANIMTIC };

        tDesc.iSlideAnimIdx = iSlide;
        tDesc.iFirstAnimIdx = iCombo1;
        tDesc.iSecondAnimIdx = iCombo2;
        tDesc.iThirdAnimIdx = iCombo3;
        tDesc.iFourthAnimIdx = iCombo4;
        tDesc.iEndStateIndex = ENUM_TO_UINT(State::END);
        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::COMBO_DUAL), CState_DualCombo::Create(pActionState, &tDesc))))
            return E_FAIL;
    }


    // JumpAttStart
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        desc.vecPreAnims = {
                                {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_Start")},
                                {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_FallAttack_Start")}
        };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_Loop") , Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_FallAttack_Loop") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend = false;
        desc.bLoop = true;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;
        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        desc.FCollis = 0;

        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::None;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPATTSTART), CState_JumpAttStart::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JumpAttEnd
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_End"), Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_FallAttack_End") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
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
        tKeyTimer.fMaxTime = 19.f / ANIMTIC;//0.55f ;
        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_End;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPATTEND), CState_JumpAttEnd::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Charge
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni | CStateBase::STATEANI_FLAG::SA_PreNonEvent;
        desc.vecPreAnims = { 
            {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_Start")}
            ,  {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_HeavyAttack_Start")}
        
        };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_End"),Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_HeavyAttack_End") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
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
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(CPlayer::State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 1.f;
        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_End;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CHARGE), CState_MoonCharge::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // skill1
    {
        CState_SkillBase::Skill_DESC tDesc = {};
        tDesc.bKeyInput = true;
        tDesc.fKeyCoolTime = 21.f / ANIMTIC;
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
        tDesc.fKeyCoolTime = 60.f / ANIMTIC;
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
            | CStateBase_Player::COLLISIONFLAGS::C_Fly
            | CStateBase_Player::COLLISIONFLAGS::C_CheckF;

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
            | CStateBase_Player::COLLISIONFLAGS::C_Fly 
            | CStateBase_Player::COLLISIONFLAGS::C_CheckF;

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

    array<_uint, ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::END)> arrAimMix;
    arrAimMix[ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::DOWN)]      = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Aim_MD");
    arrAimMix[ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::MIDDLE)]    = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Aim_MM");
    arrAimMix[ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::UP)]        = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Aim_MU");


    vector<CModel::DATA_ANIMIX> vecDownMix = { {304,true,1.f},{329,true,1.f},{378,true,1.f} };//, { 204,true,1.f } // 넣을 거면 mix도 blend 필요
    for (auto& MixAnim : arrMix)
    {
        pModel->Make_MixRatio(MixAnim, vecDownMix, pAnimMixCS);
    }

    // gun attack
    {
        CState_GunBase::GUN_STATEBASE_DESC tDesc = {};

        tDesc.arrMixAnims = arrMix;
        tDesc.arrAimAnims = arrAimMix;

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
        tDesc.arrAimAnims = arrAimMix;
        
        tDesc.bLoop = false;
        tDesc.pOwnerGun = pMyGun;
        tDesc.iMainAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Machinegun01_Reload");

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNRELOAD), CState_GunReload::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    return S_OK;
}

void CMainPlayer::Count_Combo()
{
    // player state에 따라 combo count 증가 여부 결정
    CStatCom_Player* pStat = Get_Component<CStatCom_Player>();
    switch ((_uint)Get_Component<CActionState>()->Get_CurrentStateIndex())
    {
    case ENUM_TO_UINT(State::COMBO):
    case ENUM_TO_UINT(State::JUMPATTEND):
    case ENUM_TO_UINT(State::CHARGE):
        pStat->Add_ComboCount();
        m_pGameInstance->Broadcast<COMBO_ATTACK_EVENT_START>();
        break;

    default:
        pStat->Reset_ComboCount();
    }
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
