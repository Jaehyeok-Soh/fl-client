#include "Engine_pch.h"
#include "AnimEffectHandler.h"
#include "Model.h"
#include "Transform.h"
#include "GameObject.h"
#include "PartObject.h"
#include "ContainerObject.h"
#include "GameInstance.h"

CAnimEffectHandler::CAnimEffectHandler() : Super() {}

CAnimEffectHandler::CAnimEffectHandler(const CAnimEffectHandler& rhs)
    : Super(rhs), m_tDesc(rhs.m_tDesc) {
}

HRESULT CAnimEffectHandler::Initialize_Prototype(void* pArg)
{
    if (pArg)
        m_tDesc = *(static_cast<ANIM_EFFECT_HANDLER_DESC*>(pArg));
    return S_OK;
}

HRESULT CAnimEffectHandler::Initialize(void* pArg) { return S_OK; }

void CAnimEffectHandler::Awake()
{
    GetAnimation();

    if (Get_Owner())
    {
        // 툴에서 부모의 Matrix를 참조할 때 안전하게 가져오기
        auto pTransform = Get_Owner()->Get_Component<CTransform>();
        if (pTransform)
        {
            // PartObject 기반이면 부모 Container의 WorldMatrix를 참조함
            auto pPart = static_cast<CPartObject*>(Get_Owner());
            if (pPart->Get_Parent())
                m_pOwnerMatrix = &pPart->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix();
            else
                m_pOwnerMatrix = &pTransform->Get_WorldMatrix();
        }
    }
}

void CAnimEffectHandler::Update(_float fDT) { CheckAnim(); }

void CAnimEffectHandler::GetAnimation()
{
    if (Get_Owner())
    {
        m_pOwnerModel = Get_Owner()->Get_Component<CModel>();
        Safe_AddRef(m_pOwnerModel);
    }
}

void CAnimEffectHandler::CheckAnim()
{
    if (nullptr == m_pOwnerModel) return;

    int iCurAnimIndex = m_pOwnerModel->Get_CurrentAnimationIndex();
    _float fCurTrackPos = m_pOwnerModel->Get_AnimTrackPosition();

    // 1. 애니메이션이 바뀌거나 트랙이 루프되어 처음으로 돌아갔을 때의 처리
    if (m_iPrevAnimIndex != (_uint)iCurAnimIndex || fCurTrackPos < m_fPrevTrackPosition)
    {
        m_iPrevAnimIndex = (_uint)iCurAnimIndex;
        m_fPrevTrackPosition = 0.f;
    }

    // 2. 현재 애니메이션에 등록된 이벤트 검색
    auto iter = m_tDesc.mapEvents.find((_uint)iCurAnimIndex);
    if (iter != m_tDesc.mapEvents.end())
    {
        for (auto& tEventData : iter->second)
        {
            // 이전 트랙 위치와 현재 트랙 위치 사이에 이벤트 시점이 존재하는지 확인
            if (m_fPrevTrackPosition <= tEventData.fStartTrackPosition && fCurTrackPos >= tEventData.fStartTrackPosition)
            {
                Request_SpawnEffect(tEventData);
            }
        }
    }
    m_fPrevTrackPosition = fCurTrackPos;
}
void CAnimEffectHandler::Request_SpawnEffect(const DTO::EFFECTEVENT& Script)
{
    Matrix matTargetWorld = XMMatrixIdentity();
    Matrix* pTargetBoneMatrix = nullptr;
    Matrix matOwnerNoScale = *m_pOwnerMatrix;

    Vector3 vRight = Vector3(matOwnerNoScale.m[0][0], matOwnerNoScale.m[0][1], matOwnerNoScale.m[0][2]);
    Vector3 vUp = Vector3(matOwnerNoScale.m[1][0], matOwnerNoScale.m[1][1], matOwnerNoScale.m[1][2]);
    Vector3 vLook = Vector3(matOwnerNoScale.m[2][0], matOwnerNoScale.m[2][1], matOwnerNoScale.m[2][2]);

    vRight.Normalize();
    vUp.Normalize();
    vLook.Normalize();

    // 정규화된 벡터를 다시 행렬에 꽂아줍니다.
    matOwnerNoScale.m[0][0] = vRight.x; matOwnerNoScale.m[0][1] = vRight.y; matOwnerNoScale.m[0][2] = vRight.z;
    matOwnerNoScale.m[1][0] = vUp.x;    matOwnerNoScale.m[1][1] = vUp.y;    matOwnerNoScale.m[1][2] = vUp.z;
    matOwnerNoScale.m[2][0] = vLook.x;  matOwnerNoScale.m[2][1] = vLook.y;  matOwnerNoScale.m[2][2] = vLook.z;
    // ------------------------------------------

    if (Script.strSocketName.empty() == false)
    {
        // pTargetBoneMatrix 로직 (주석 해제 시 동일하게 적용)
        if (pTargetBoneMatrix)
            matTargetWorld = (*pTargetBoneMatrix) * matOwnerNoScale; // 스케일 빠진 행렬 사용
        else
            matTargetWorld = matOwnerNoScale;
    }
    else
    {
        matTargetWorld = matOwnerNoScale;
    }

    // 오프셋 적용
    Matrix matOffset = XMMatrixTranslation(Script.vOffset.x, Script.vOffset.y, Script.vOffset.z);
    matTargetWorld = matOffset * matTargetWorld;

    // 이펙트 생성 요청
    m_pGameInstance->Spawn_Effect(
        Script.strEffectTag,
        matTargetWorld,
        Script.fDuration,
        (_bool)Script.iSimulationType,
        Script.bFollowBone ? pTargetBoneMatrix : nullptr
    );
}

CAnimEffectHandler* CAnimEffectHandler::Create(void* pArg)
{
    CAnimEffectHandler* pInstance = new CAnimEffectHandler();
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Created : CAnimEffectHandler");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CAnimEffectHandler::Clone(void* pArg)
{
    CAnimEffectHandler* pInstance = new CAnimEffectHandler(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAnimEffectHandler");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CAnimEffectHandler::Free()
{
    Safe_Release(m_pOwnerModel);
    Super::Free();
}