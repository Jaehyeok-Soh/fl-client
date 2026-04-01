#include "pch.h"
#include "UIQTE_Image.h"
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
#include "UIQTE_Manager.h"
#include "GameInstance.h"

#define PERFECT_DIFF 5.f
#define NORMAL_DIFF 25.f

CUIQTE_Image::CUIQTE_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIQTE_Image::CUIQTE_Image(const CUIQTE_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIQTE_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Image::Initialize(void* pArg)
{
	QTE_IMAGE_DESC* pDesc = static_cast<QTE_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUIQTE_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUIQTE_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIQTE_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIQTE_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIQTE_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Move_Rotate(m_fRotate);
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIQTE_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Image::Ready_Components(QTE_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
	{
		if (FAILED(pShader->Get_Variable("g_fGlowIntensity")->SetRawValue(&m_fGlowIntensity, 0, sizeof(_float))))
			return E_FAIL;
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
	{
		if (FAILED(pShader->Get_Variable("g_fGlowIntensity")->SetRawValue(&m_fGlowIntensity, 0, sizeof(_float))))
			return E_FAIL;
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:

		break;
	case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
		break;
	}

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIQTE_Image::Attach_Personal_Info()
{
	if (m_isSpawned)
	{
		m_isSpawned = false;

		switch (m_eDImageSubClass)
		{
		case DTO::EUIDImageSubClassType::QTE_BG:
		{
			Set_Visible();
		}
		break;
		case DTO::EUIDImageSubClassType::QTE_BG_DECO:
		case DTO::EUIDImageSubClassType::QTE_LINE:
		{
			Set_Visible();
		}
		break;
		case DTO::EUIDImageSubClassType::QTE_KEYICON:
		{
			Set_Visible();
		}
		break;

		case DTO::EUIDImageSubClassType::QTE_TIMING_CIRCLE:
		{
			if (m_iNumbering == 0)
				Set_Visible();
		}
			break;
		case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
			break;
		case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
			break;
		case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:
			break;
		case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
			break;
		}
	}

	return S_OK;
}

void CUIQTE_Image::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::QTE_SUCCESS == Desc.eEventID)
				{
					if (Desc.iParam0 == m_iNumbering)
					{
						// 지금 성공한 노드인지 // 
						switch (m_eDImageSubClass)
						{
						case DTO::EUIDImageSubClassType::QTE_BG:
						case DTO::EUIDImageSubClassType::QTE_BG_DECO:
						case DTO::EUIDImageSubClassType::QTE_KEYICON:
						case DTO::EUIDImageSubClassType::QTE_TIMING_CIRCLE:
						case DTO::EUIDImageSubClassType::QTE_LINE:
						{
							this->Set_Invisible();
						}
							break;

						case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
						case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
						case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:
						case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
						{
							m_iTimingType = Desc.iParam2;
							this->Set_Visible();
						}
							break;
						}
					}
					else if (Desc.iParam1 == m_iNumbering)
					{
						// 성공한 노드의 다음 노드인지 //
						switch (m_eDImageSubClass)
						{
						case DTO::EUIDImageSubClassType::QTE_BG:
						case DTO::EUIDImageSubClassType::QTE_BG_DECO:
						case DTO::EUIDImageSubClassType::QTE_KEYICON:
							break;
						case DTO::EUIDImageSubClassType::QTE_LINE:
						{
							this->Set_Invisible();
						}
							break;
						case DTO::EUIDImageSubClassType::QTE_TIMING_CIRCLE:
						{
							this->Set_Visible();
						}
							break;
						case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
						case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
						case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:
						case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
						break;
						}
					}
				}
			}));
}

void CUIQTE_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::QTE_BG:
		break;
	case DTO::EUIDImageSubClassType::QTE_BG_DECO:
		break;
	case DTO::EUIDImageSubClassType::QTE_LINE:
	{
		Vec2 vDir = m_vCurrentNodePosOffset - m_vPrevNodePosOffset;
		m_vMoveOffset = (m_vCurrentNodePosOffset + m_vPrevNodePosOffset) * 0.5f;
		m_fWidth = vDir.Length();
		if (m_fWidth <= 0.2f)
			m_fWidth = 0.2f;
		m_fHeight = 2.f;
		m_fRotate = atan2f(vDir.y, vDir.x);
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_KEYICON:
		break;
	case DTO::EUIDImageSubClassType::QTE_TIMING_CIRCLE:
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:
		break;
	case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
		break;
	}
}

void CUIQTE_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::QTE_BG:
	case DTO::EUIDImageSubClassType::QTE_BG_DECO:
	case DTO::EUIDImageSubClassType::QTE_LINE:
	case DTO::EUIDImageSubClassType::QTE_KEYICON:
		Ready_Fade(0.2f, 0.f, 1.f, m_fDelay);
			break;

	case DTO::EUIDImageSubClassType::QTE_TIMING_CIRCLE:
		Ready_LerpChange(m_fTimingDuration, 400.f, 50.f, 1.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
	{
		CUIQTE_Manager::EQTETiming eType = static_cast<CUIQTE_Manager::EQTETiming>(m_iTimingType);
		m_fGlowIntensity = 0.f;

		m_vColorTint = Vec4{ 1.f, 0.98f, 0.8f, 0.f };
		m_vGradiantColorTint = Vec4{ 1.f, 0.98f, 0.8f, 0.f };

		switch (eType)
		{
		case Client::CUIQTE_Manager::EQTETiming::FAST:
			m_fTargetGlowInstensity = 2.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::PERFECT:
			m_fTargetGlowInstensity = 5.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::SLOW:
			m_fTargetGlowInstensity = 2.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::FAIL:
			m_fTargetGlowInstensity = 2.f;
			m_vColorTint = Vec4{ 1.f, 0.f, 0.f, 0.f };
			m_vGradiantColorTint = Vec4{ 1.f, 0.f, 0.f, 0.f };
			break;
		}
		Ready_LerpChange(0.3f, 0.f, m_fTargetGlowInstensity, 3.f, m_fDelay, true);
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
	{
		CUIQTE_Manager::EQTETiming eType = static_cast<CUIQTE_Manager::EQTETiming>(m_iTimingType);
		m_fGlowIntensity = 0.f;

		m_vColorTint = Vec4{ 1.f, 0.98f, 0.8f, 0.f };
		m_vGradiantColorTint = Vec4{ 1.f, 0.98f, 0.8f, 0.f };

		switch (eType)
		{
		case Client::CUIQTE_Manager::EQTETiming::FAST:
			m_fTargetGlowInstensity = 2.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::PERFECT:
			m_fTargetGlowInstensity = 5.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::SLOW:
			m_fTargetGlowInstensity = 2.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::FAIL:
			m_fTargetGlowInstensity = 2.f;
			m_vColorTint = Vec4{ 1.f, 0.f, 0.f, 0.f };
			m_vGradiantColorTint = Vec4{ 1.f, 0.f, 0.f, 0.f };
			break;
		}
		Ready_LerpChange(0.3f, 0.f, m_fTargetGlowInstensity, 3.f, m_fDelay, true);
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:
	{
		CUIQTE_Manager::EQTETiming eType = static_cast<CUIQTE_Manager::EQTETiming>(m_iTimingType);
		m_fGlowIntensity = 0.f;

		m_vColorTint = Vec4{ 1.f, 0.98f, 0.8f, 0.f };
		m_vGradiantColorTint = Vec4{ 1.f, 0.98f, 0.8f, 0.f };

		switch (eType)
		{
		case Client::CUIQTE_Manager::EQTETiming::FAST:
			m_fTargetGlowInstensity = 2.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::PERFECT:
			m_fTargetGlowInstensity = 5.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::SLOW:
			m_fTargetGlowInstensity = 2.f;
			break;
		case Client::CUIQTE_Manager::EQTETiming::FAIL:
			m_fTargetGlowInstensity = 2.f;
			m_vColorTint = Vec4{ 1.f, 0.f, 0.f, 0.f };
			m_vGradiantColorTint = Vec4{ 1.f, 0.f, 0.f, 0.f };
			break;
		}
		Ready_LerpChange(0.3f, 10.f, 300.f, 3.f, m_fDelay, true);
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
		break;
	}
}

_bool CUIQTE_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::QTE_BG:
	case DTO::EUIDImageSubClassType::QTE_BG_DECO:
	case DTO::EUIDImageSubClassType::QTE_LINE:
	case DTO::EUIDImageSubClassType::QTE_KEYICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		if (isFade)
		{
			return true;
		}
	}
		break;

	case DTO::EUIDImageSubClassType::QTE_TIMING_CIRCLE:
	{
		_float fSize = {  };
		_bool isChange = Tick_LerpChange(&fSize, fTimeDelta);

		m_fWidth = fSize;
		m_fHeight = fSize;

		if (m_iNumbering == CUIQTE_Manager::GetInstance()->Get_CurrentNodeIndex())
		{
			_float fDiff = fSize - 100.f;
			_float fAbsDiff = fabsf(fDiff);

			if (fAbsDiff <= PERFECT_DIFF)
				CUIQTE_Manager::GetInstance()->Set_Current_Node_Progress(CUIQTE_Manager::EQTETiming::PERFECT);
			else if (fDiff > 0.f && fAbsDiff <= NORMAL_DIFF)
				CUIQTE_Manager::GetInstance()->Set_Current_Node_Progress(CUIQTE_Manager::EQTETiming::FAST);
			else if (fDiff < 0.f && fAbsDiff <= NORMAL_DIFF)
				CUIQTE_Manager::GetInstance()->Set_Current_Node_Progress(CUIQTE_Manager::EQTETiming::SLOW);
			else
				CUIQTE_Manager::GetInstance()->Set_Current_Node_Progress(CUIQTE_Manager::EQTETiming::FAIL);
		}

		if (isChange)
		{
			return true;
		}
	}
	break;

	case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
	{
		_bool isChange = Tick_LerpChange(&m_fGlowIntensity, fTimeDelta);
		if (isChange)
		{
			Set_Invisible();
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
	{
		_bool isChange = Tick_LerpChange(&m_fGlowIntensity, fTimeDelta);
		if (isChange)
		{
			Set_Invisible();
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:
	{
		_float fSize = {};
		_bool isChange = Tick_LerpChange(&fSize, fTimeDelta);

		m_fProgress_Ratio += fTimeDelta * 3.f;
		if (m_fProgress_Ratio > 1.f)
			m_fProgress_Ratio = 1.f;

		m_fWidth = fSize;
		m_fHeight = fSize;

		if (isChange)
		{
			Set_Invisible();
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
		Set_Invisible();
		return true;
		break;
	}

	return false;
}

void CUIQTE_Image::Initialize_InVisible_Event()
{
	Ready_Fade(0.3f, 1.f, 0.f, m_fDelay);
}

_bool CUIQTE_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);

	if (isFade)
	{
		Request_SetDead();
		return true;
	}
	return false;
}

HRESULT CUIQTE_Image::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);

	if (auto* pQTEData = std::get_if<UI_QTE_PREFAB_DATA>(&pDesc->Data))
	{
		m_isSpawned = true;

		m_isDeadRequest = false;
		m_iNumbering = pQTEData->iNodeIndex;

		m_vMoveOffset = pQTEData->vSpawnOffset;
		m_fAlpha_Ratio = 1.f;

		switch (m_eDImageSubClass)
		{
		case DTO::EUIDImageSubClassType::QTE_BG:
		case DTO::EUIDImageSubClassType::QTE_BG_DECO:
		{
			m_vCurrentNodePosOffset = pQTEData->vSpawnOffset;
			m_vPrevNodePosOffset = pQTEData->vPreSpawnOffset;
		}
		break;
		case DTO::EUIDImageSubClassType::QTE_LINE:
		{
			m_vCurrentNodePosOffset = pQTEData->vSpawnOffset;
			m_vPrevNodePosOffset = pQTEData->vPreSpawnOffset;
			m_fWidth = 0.2f;
		}
			break;
		case DTO::EUIDImageSubClassType::QTE_KEYICON:
		{
			m_iKeyType = pQTEData->iKeyType;
			KeyType_To_Texturetag(m_iKeyType);
		}
			break;

		case DTO::EUIDImageSubClassType::QTE_TIMING_CIRCLE:
		{
			m_fTimingDuration = pQTEData->fTimingTime;
		}
			break;

		case DTO::EUIDImageSubClassType::QTE_CLEAR_KEYICON_FX:
			break;
		case DTO::EUIDImageSubClassType::QTE_CLEAR_CIRCLE_FX:
			break;
		case DTO::EUIDImageSubClassType::QTE_CLEAR_BG_CIRCLE_FX:
		{
			m_fProgress_Ratio = 0.f;
			m_fWidth = 10.f;
			m_fHeight = 10.f;
		}
			break;
		case DTO::EUIDImageSubClassType::QTE_TEXT_FX:
			break;
		}
	}
	return S_OK;
}

HRESULT CUIQTE_Image::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;


	m_fGlowIntensity = 0.f;
	m_fTargetGlowInstensity = 0.f;
	m_iTimingType = ENUM_TO_UINT(CUIQTE_Manager::EQTETiming::END);
	m_vColorTint = Vec4{ 1.f, 0.98f, 0.8f, 1.f };
	m_vGradiantColorTint = Vec4{ 1.f, 0.98f, 0.8f, 1.f };
	m_fProgress_Ratio = 0.f;

	return S_OK;
}

void CUIQTE_Image::KeyType_To_Texturetag(_uint iKeyIndex)
{
	_wstring wstrTextureTag = {};
	switch (iKeyIndex)
	{
	case DIK_A:
		wstrTextureTag = L"Texture_T_Key_A";
		break;
	case DIK_S:
		wstrTextureTag = L"Texture_T_Key_S";
		break;
	case DIK_D:
		wstrTextureTag = L"Texture_T_Key_D";
		break;
	case DIK_F:
		wstrTextureTag = L"Texture_T_Key_F";
		break;
	}
	Get_Component<CTexture>()->Add_DefaultTexture(wstrTextureTag, ENUM_TO_UINT(EUITextureSlot::DEFAULT));
}

CUIQTE_Image* CUIQTE_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIQTE_Image* pInstance = new CUIQTE_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIQTE_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIQTE_Image::Clone(void* pArg)
{
	CUIQTE_Image* pInstance = new CUIQTE_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIQTE_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIQTE_Image::Free()
{
	Super::Free();
}
