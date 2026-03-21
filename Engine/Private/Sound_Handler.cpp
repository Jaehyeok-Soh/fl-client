#include "Engine_pch.h"
#include "Sound_Handler.h"
#include "Model.h"
#include "GameInstance.h"

CSound_Handler::CSound_Handler()
	:Super()
{

}

CSound_Handler::CSound_Handler(const CSound_Handler& rhs)
	:Super(rhs)
{

}

HRESULT CSound_Handler::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Handler::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSound_Handler::Bind_Events()
{
	if (Get_Owner() == nullptr)
		return;

	AnimNotifyKey key{};
	
}

void CSound_Handler::Ready_Event()
{
	Release_Event();

	m_EventHandle = m_pOwnerModel->OnNotify.Subscribe(
		[this](const AnimNotifyKey& key)
		{
			this->CallbackEvent(key);
		});
}

void CSound_Handler::Release_Event()
{
	if (m_pOwner)
	{
		if (m_pOwnerModel)
			m_pOwnerModel->OnNotify.Unsubscribe(m_EventHandle);
	}
}

void CSound_Handler::CallbackEvent(const AnimNotifyKey& key)
{
	if (key.eID != EAnimNotifyId::Sound)
		return;
}

void CSound_Handler::Process_SoundEvent(const DTO::SOUNDEVENT& tEvent)
{
}

CSound_Handler* CSound_Handler::Create()
{
	CSound_Handler* pInstance = new CSound_Handler();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CSound_Handler::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CSound_Handler::Clone(void* pArg)
{
	CSound_Handler* pInstance = new CSound_Handler();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSound_Handler::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSound_Handler::Free()
{
	Release_Event();
	Super::Free();
}