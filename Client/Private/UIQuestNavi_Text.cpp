#include "pch.h"
#include "UIQuestNavi_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Player.h"
#include "CameraMan.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include <UI_Manager.h>

#define CLAMP_MIN_X 266.5f
#define CLAMP_MAX_X 1333.5f
#define CLAMP_MIN_Y 150.f
#define CLAMP_MAX_Y 750.f

CUIQuestNavi_Text::CUIQuestNavi_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIQuestNavi_Text::CUIQuestNavi_Text(const CUIQuestNavi_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIQuestNavi_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuestNavi_Text::Initialize(void* pArg)
{
	QUEST_NAVI_TEXT_DESC* pDesc = static_cast<QUEST_NAVI_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuestNavi_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}


void CUIQuestNavi_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIQuestNavi_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUIQuestNavi_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIQuestNavi_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;

	m_vFontPos.x = m_vScreenPos.x + m_vMoveOffset.x;
	m_vFontPos.y = m_vScreenPos.y + m_vMoveOffset.y + 40.f;
}

HRESULT CUIQuestNavi_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuestNavi_Text::Ready_Components(QUEST_NAVI_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuestNavi_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuestNavi_Text::Attach_Personal_Info()
{
	D3D11_VIEWPORT tViewport = {};
	_uint iNumViewports = 1;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &tViewport);

	m_fVPTopLeftX = tViewport.TopLeftX;
	m_fVPTopLeftY = tViewport.TopLeftY;
	m_fVPWidth = tViewport.Width;
	m_fVPHegiht = tViewport.Height;

	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(pResult);
	if (nullptr == m_pPlayer)
		return E_FAIL;

	return S_OK;
}

HRESULT CUIQuestNavi_Text::Convert_Stat_To_Text()
{
	m_wstrText = Float_To_Wstring(m_fDistance, 0) + L"m";

	return S_OK;
}

void CUIQuestNavi_Text::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					this->Set_Visible();
				}
			})
	);
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
					if (-1 == desc.tChapterInfo.tQuestDesc.iId)
						return;

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
		m_pGameInstance->Subscribe<QUEST_CHANGE_SCENARIO_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				this->m_vTargetPos = desc.tChapterInfo.vObjectPosition;

				if (desc.tChapterInfo.eEvent == DTO::EQuestEvent::MONSTER_KILL ||
					desc.tChapterInfo.eEvent == DTO::EQuestEvent::NPC_TALK)
					this->Set_Invisible();
				else
					this->Set_Visible();
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_CHAPTER_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				this->m_vTargetPos = desc.tChapterInfo.vObjectPosition;

				if (desc.tChapterInfo.eEvent == DTO::EQuestEvent::MONSTER_KILL ||
					desc.tChapterInfo.eEvent == DTO::EQuestEvent::NPC_TALK)
					this->Set_Invisible();
				else
					this->Set_Visible();

			})
	);
}

void CUIQuestNavi_Text::Initialize_Visible_Event()
{
}

void CUIQuestNavi_Text::Initialize_InVisible_Event()
{
}

_bool CUIQuestNavi_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

_bool CUIQuestNavi_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIQuestNavi_Text::Tick_By_Type(const _float fTimeDelta)
{
	Vec4 clip = Vec4(m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z, 1.f);
	clip = Vec4::Transform(clip, m_pGameInstance->Get_ViewMatrix());
	m_fViewZ = clip.z;
	clip = Vec4::Transform(clip, m_pGameInstance->Get_ProjMatrix());

	Vec3 vCameraRight = m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vCameraLook = m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	Vec3 vPlayerPos = m_pPlayer->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vDir = m_vTargetPos - vPlayerPos;

	m_fDistance = vDir.Length();

	vDir.y = 0.f;
	vCameraLook.y = 0.f;
	vCameraRight.y = 0.f;

	if (vDir.LengthSquared() > 0.0001f)
		vDir.Normalize();

	if (vCameraLook.LengthSquared() > 0.0001f)
		vCameraLook.Normalize();

	if (vCameraRight.LengthSquared() > 0.0001f)
		vCameraRight.Normalize();

	_float fx = vDir.Dot(vCameraRight);
	_float fy = vDir.Dot(vCameraLook);

	if (clip.w <= 0.1f)
	{
		m_vScreenPos.x = (g_iWinSizeX / 2.f) + (fx * 533.5f);
		m_vScreenPos.y = (g_iWinSizeY / 2.f) - (fy * 250.f);
	}
	else
	{
		clip /= clip.w;

		m_vScreenPos.x = (clip.x * 0.5f + 0.5f) * m_fVPWidth + m_fVPTopLeftX;
		m_vScreenPos.y = (1.f - (clip.y * 0.5f + 0.5f)) * m_fVPHegiht + m_fVPTopLeftY;
	}

	if (m_vScreenPos.x < CLAMP_MIN_X)
		m_vScreenPos.x = CLAMP_MIN_X;
	else if (m_vScreenPos.x > CLAMP_MAX_X)
		m_vScreenPos.x = CLAMP_MAX_X;

	if (m_vScreenPos.y < CLAMP_MIN_Y)
		m_vScreenPos.y = CLAMP_MIN_Y;
	else if (m_vScreenPos.y > CLAMP_MAX_Y)
		m_vScreenPos.y = CLAMP_MAX_Y;
}

CUIQuestNavi_Text* CUIQuestNavi_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIQuestNavi_Text* pInstance = new CUIQuestNavi_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIQuestNavi_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIQuestNavi_Text::Clone(void* pArg)
{
	CUIQuestNavi_Text* pInstance = new CUIQuestNavi_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIQuestNavi_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIQuestNavi_Text::Free()
{
	Super::Free();
}
