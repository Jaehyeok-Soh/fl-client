#include "pch.h"
#include "UIQuest_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Canvas.h"
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "QuestManager.h"
#include "GameInstance.h"

CUIQuest_Image::CUIQuest_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIQuest_Image::CUIQuest_Image(const CUIQuest_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIQuest_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Image::Initialize(void* pArg)
{
	QUEST_IMAGE_DESC* pDesc = static_cast<QUEST_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIQuest_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIQuest_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIQuest_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIQuest_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIQuest_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIQuest_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Image::Ready_Components(QUEST_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Image::Attach_Personal_Info()
{
	return S_OK;
}

void CUIQuest_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
					if (-1 == desc.tChapterInfo.tQuestDesc.iId)
						return;

					this->Set_Visible();
				}
			})
	);
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_END>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				if (-1 == desc.tChapterInfo.tQuestDesc.iId)
					return;

				this->Set_Visible();
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_CHAPTER_NOTIFY>([this]()
			{
				Set_Visible();
			})
	);

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::QUEST_NAME_CHANGE == Desc.eEventID)
				{
					this->Set_Invisible();
					this->m_isVisibleTrigger = true;
				}
			})
	);
}

void CUIQuest_Image::Tick_By_Type(const _float fTimeDelta)
{
	if (m_isVisibleTriggerStart)
	{
		Set_Visible();
		m_isVisibleTriggerStart = false;
	}
}

void CUIQuest_Image::Initialize_Visible_Event()
{
	Ready_Lerp_Movement(Vec2{ -50.f, 0.f }, Vec2{ 0.f, 0.f }, 0.5f, 5.f, m_fDelay, true);
	Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
}

_bool CUIQuest_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	_bool isMove = Tick_Lerp_Movement(fTimeDelta);

	if (isFade && isMove)
	{
		m_isFin_Event = true;
		return true;
	}

	return false;
}

void CUIQuest_Image::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ -50.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
	Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
}

_bool CUIQuest_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	_bool isMove = Tick_Lerp_Movement(fTimeDelta);

	if (isFade && isMove)
	{
		m_isFin_Event = true;

		if (m_isVisibleTrigger)
		{
			m_isVisibleTriggerStart = true;
			m_isVisibleTrigger = false;
		}

		return true;
	}

	return false;
}

CUIQuest_Image* CUIQuest_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIQuest_Image* pInstance = new CUIQuest_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIQuest_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIQuest_Image::Clone(void* pArg)
{
	CUIQuest_Image* pInstance = new CUIQuest_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIQuest_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIQuest_Image::Free()
{
	Super::Free();
}
