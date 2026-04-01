#include "pch.h"
#include "UIBossSkill_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIBossSkill_Image::CUIBossSkill_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIBossSkill_Image::CUIBossSkill_Image(const CUIBossSkill_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIBossSkill_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Image::Initialize(void* pArg)
{
	BOSS_SKILL_IMAGE_DESC* pDesc = static_cast<BOSS_SKILL_IMAGE_DESC*>(pArg);
	m_iNumbering = pDesc->iNumbering;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUIBossSkill_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUIBossSkill_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossSkill_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIBossSkill_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIBossSkill_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIBossSkill_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Image::Ready_Components(BOSS_SKILL_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIBossSkill_Image::Attach_Personal_Info()
{
	return S_OK;
}

void CUIBossSkill_Image::Bind_Events()
{
	Super::Bind_Events();


	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<BOSS_SKILL_ON>(
			[this]()
			{
				this->Set_Visible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<BOSS_SKILL_OFF>(
			[this]()
			{
				this->Set_Invisible();
			}));
}

void CUIBossSkill_Image::Tick_By_Type(const _float fTimeDelta)
{
}

void CUIBossSkill_Image::Initialize_Visible_Event()
{
	Ready_Fade(0.4f, 0.f, 1.f, m_fDelay);
	m_fProgress_Ratio = 0.f;
}

_bool CUIBossSkill_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);

	if (isFade)
	{
		return true;
	}
	return false;
}

void CUIBossSkill_Image::Initialize_InVisible_Event()
{
}

_bool CUIBossSkill_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIBossSkill_Image* CUIBossSkill_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossSkill_Image* pInstance = new CUIBossSkill_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossSkill_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossSkill_Image::Clone(void* pArg)
{
	CUIBossSkill_Image* pInstance = new CUIBossSkill_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossSkill_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossSkill_Image::Free()
{
	Super::Free();
}
