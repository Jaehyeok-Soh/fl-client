#include "pch.h"
#include "UIQuickSlot_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Player.h"
#include "Canvas.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "UIQuickSlot_Manager.h"
#include "GameInstance.h"


#define BOOLEAN_SLOT_Z_KEY_HOLDING 0

#define UINT_SLOT_CURRENT_SLOT_INDEX 0
CUIQuickSlot_Image::CUIQuickSlot_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIQuickSlot_Image::CUIQuickSlot_Image(const CUIQuickSlot_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIQuickSlot_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuickSlot_Image::Initialize(void* pArg)
{
	QUICK_SLOT_IMAGE_DESC* pDesc = static_cast<QUICK_SLOT_IMAGE_DESC*>(pArg);
	m_iNumbering = pDesc->iNumbering;
	
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIQuickSlot_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIQuickSlot_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIQuickSlot_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIQuickSlot_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIQuickSlot_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIQuickSlot_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuickSlot_Image::Ready_Components(QUICK_SLOT_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuickSlot_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuickSlot_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_CENTER:
		break;
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG:
		break;
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG_FX:
		break;
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_SLOT:
	{
	}
		break;
	}

	return S_OK;
}

void CUIQuickSlot_Image::Bind_Events()
{
	Super::Bind_Events();
}

void CUIQuickSlot_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_CENTER:
	{
		if (KEY_BUTTON_HOLD(DIK_Z))
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[BOOLEAN_SLOT_Z_KEY_HOLDING] = true;
			m_pUIManager->Set_isCursor_Visible(true);
			m_pGameInstance->Request_CursorMode(ECursorMode::InVisibleClipped);
			m_pGameInstance->Active_SloMo(0.1f);

			Calc_Current_SlotIndex();
			Tick_Follow_TargetSlot(fTimeDelta);

			m_pParentCanvasCache->Get_CommonParam_uint_Ref()[UINT_SLOT_CURRENT_SLOT_INDEX] = m_iCurSlotIndex;
		}
		else
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[BOOLEAN_SLOT_Z_KEY_HOLDING] = false;
			m_pUIManager->Set_isCursor_Visible(false);

			m_pGameInstance->Deactivate_SloMo();
		}

	}
		break;
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG:
		break;
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG_FX:
	{
		Check_Current_Slot();
		Tick_Fade(fTimeDelta);
	}
		break;
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_SLOT:
	{
		if (!m_pParentCanvasCache->Get_CommonParam_bool()[BOOLEAN_SLOT_Z_KEY_HOLDING])
			break;

		WeaponType_To_Icon();
		CUIQuickSlot_Manager::GetInstance()->Request_Change_Weapon(m_pParentCanvasCache->Get_CommonParam_uint()[UINT_SLOT_CURRENT_SLOT_INDEX]);
	}
		break;
	}

	_bool isHolding = m_pParentCanvasCache->Get_CommonParam_bool()[BOOLEAN_SLOT_Z_KEY_HOLDING];

	if (isHolding)
	{
		if (m_eVisibleState != EQuickSlotVisibleState::VISIBLE)
		{
			Set_Visible();
			m_eVisibleState = EQuickSlotVisibleState::VISIBLE;
		}
	}
	else
	{
		if (m_eVisibleState != EQuickSlotVisibleState::INVISIBLE)
		{
			Set_Invisible();
			m_eVisibleState = EQuickSlotVisibleState::INVISIBLE;
		}
	}
}

void CUIQuickSlot_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_CENTER:
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG:
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_SLOT:
		Ready_Fade(0.2f, 0.f, 1.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG_FX:
		break;
	}
}

_bool CUIQuickSlot_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_CENTER:
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG:
	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_SLOT:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			return true;
		}
	}
		break;

	case DTO::EUIDImageSubClassType::WEAPON_QUIKSLOT_SIDE_BG_FX:
		return true;
		break;
	}
	return false;
}

void CUIQuickSlot_Image::Initialize_InVisible_Event()
{
	m_fAlpha_Ratio = 0.f;
}

_bool CUIQuickSlot_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIQuickSlot_Image::Calc_Current_SlotIndex()
{
	POINT CursorPoint = m_pGameInstance->Get_MousePos();

	_float fDirX = CursorPoint.x - m_vRenderPos.x;
	_float fDirY = CursorPoint.y - m_vRenderPos.y;

	_float fRadian = atan2f(fDirX, -fDirY); // 위쪽이 0도, Y 아래로 커짐

	if (fRadian < 0.f)
		fRadian += XM_2PI;

	const _float m_fTotalSlotCount = 8.f;
	m_iTargetSlotIndex = (_uint)(fRadian / (XM_2PI / m_fTotalSlotCount));

	CLOG_INFO("Current Slot : " + std::to_string(m_iTargetSlotIndex));
}

void CUIQuickSlot_Image::Tick_Follow_TargetSlot(const _float fTimeDelta)
{
	if (m_iCurSlotIndex == m_iTargetSlotIndex)
		return;

	m_fSlotTimeAcc += fTimeDelta;
	if (m_fSlotTimeAcc < 0.05f)
		return;

	m_fSlotTimeAcc = 0.f;

	const _uint iSlotCount = 8;

	_uint iClockwiseDist = (m_iTargetSlotIndex + iSlotCount - m_iCurSlotIndex) % iSlotCount;
	_uint iCounterClockwiseDist = (m_iCurSlotIndex + iSlotCount - m_iTargetSlotIndex) % iSlotCount;

	if (iClockwiseDist <= iCounterClockwiseDist)
	{
		m_iCurSlotIndex = (m_iCurSlotIndex + 1) % iSlotCount;
	}
	else
	{
		m_iCurSlotIndex = (m_iCurSlotIndex + iSlotCount - 1) % iSlotCount;
	}
}

void CUIQuickSlot_Image::Check_Current_Slot()
{
	if (m_pParentCanvasCache->Get_CommonParam_uint()[UINT_SLOT_CURRENT_SLOT_INDEX] == m_iNumbering)
	{
		if (!m_isSelected)
		{
			Ready_Fade(0.3f, 0.f, 0.7f, m_fDelay);
			m_isSelected = true;
		}
	}
	else
	{
		if (m_isSelected)
		{
			Ready_Fade(0.3f, m_fAlpha_Ratio, 0.f, m_fDelay);
			m_isSelected = false;
		}
	}
}

void CUIQuickSlot_Image::WeaponType_To_Icon()
{
	const auto& Info = CUIQuickSlot_Manager::GetInstance()->Get_Weapons(m_iNumbering);

	_wstring wstrTextureTag = CUIQuickSlot_Manager::GetInstance()->UIWeaponTypeToTextureTag(Info.eWeaponType);
	
	Get_Component<CTexture>()->Add_DefaultTexture(wstrTextureTag, ENUM_TO_UINT(EUITextureSlot::DEFAULT));
}

CUIQuickSlot_Image* CUIQuickSlot_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIQuickSlot_Image* pInstance = new CUIQuickSlot_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIQuickSlot_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIQuickSlot_Image::Clone(void* pArg)
{
	CUIQuickSlot_Image* pInstance = new CUIQuickSlot_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIQuickSlot_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIQuickSlot_Image::Free()
{
	Super::Free();
}
