#include "Engine_pch.h"
#include "AnimEffectHandler.h"

#include "Model.h"
#include "Transform.h"
#include "GameObject.h"
#include "PartObject.h"
#include "ContainerObject.h"
#include "Engine_Utils.h"
// #include "Effect_Manager.h"
#include "GameInstance.h"


CAnimEffectHandler::CAnimEffectHandler()
	:Super()
{

}

CAnimEffectHandler::CAnimEffectHandler(const CAnimEffectHandler& rhs)
	:Super(rhs)
{

}

HRESULT CAnimEffectHandler::Initialize_Prototype(void* pArg)
{
	if (pArg)
	{
		// 빌더에서 넘겨준 가공된 데이터를 저장한다.
		m_tDesc = *(static_cast<ANIM_EFFECT_HANDLER_DESC*>(pArg));
	}
	return S_OK;
}

HRESULT CAnimEffectHandler::Initialize(void* pArg)
{
	return S_OK;
}

void CAnimEffectHandler::Awake()
{
	// 미리 캐싱하는 작업을 여기서 한다.
	GetAnimation();

	if (Get_Owner())
	{
		auto pTransform = Get_Owner()->Get_Component<CTransform>();
		if (pTransform)
			m_pOwnerMatrix = &pTransform->Get_WorldMatrix();
	}
}

void CAnimEffectHandler::Update(_float fDT)
{
	CheckAnim();
}

void CAnimEffectHandler::GetAnimation()
{
	// 주인의 모델 컴포넌트 참조 획득
	m_pOwnerModel = Get_Owner()->Get_Component<CModel>();
	Safe_AddRef(m_pOwnerModel);
}

void CAnimEffectHandler::CheckAnim()
{
	if (nullptr == m_pOwnerModel)
		return;

	// 현재 애니메이션 인덱스 & 트랙 포지션을 들고온다.
	int iCurAnimIndex = m_pOwnerModel->Get_CurrentAnimationIndex();
	_float fCurTrackPos = m_pOwnerModel->Get_AnimTrackPosition();

	// 애니메이션이 바뀌었을 경우 이전 위치 초기화 로직.
	if (m_iPrevAnimIndex != (_uint)iCurAnimIndex)
	{
		m_iPrevAnimIndex = (_uint)iCurAnimIndex;
		m_fPrevTrackPosition = 0.f;
	}

	// 내 map 컨테이너 안에 현재 애니메이션이 있는가?
	auto iter = m_tDesc.mapEvents.find((_uint)iCurAnimIndex);
	if (iter != m_tDesc.mapEvents.end())
	{
		for (auto& tNotifyData : iter->second)
		{
			// 구간 체크
			if (m_fPrevTrackPosition <= tNotifyData.fTrackPosition && fCurTrackPos >= tNotifyData.fTrackPosition)
			{
				for (auto& script : tNotifyData.vecScript)
				{
					Request_SpawnEffect(script);
				}
			}
		}
	}
	m_fPrevTrackPosition = fCurTrackPos;
}

void CAnimEffectHandler::Request_SpawnEffect(const DTO::EFFECT_EVENT_SCRIPT& Script)
{
	// GameIntance를 통해 이펙트를 생성 요청을 하는 곳.
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

	if(FAILED(pInstance->Initialize(pArg)))
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
