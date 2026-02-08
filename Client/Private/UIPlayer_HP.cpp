#include "pch.h"
#include "UIPlayer_HP.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UIProgress_Component.h"
#include "StatComponent.h"
#include "GameInstance.h"

CUIPlayer_HP::CUIPlayer_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIPlayer_HP::CUIPlayer_HP(const CUIPlayer_HP& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIPlayer_HP::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayer_HP::Initialize(void* pArg)
{
	PLAYER_HP_DESC* pDesc = static_cast<PLAYER_HP_DESC*>(pArg);
	m_pTargetStat = pDesc->pTargetStat;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (!m_isUseColorTint)
	{
		if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrTextureTag, 0)))
			return E_FAIL;
		Get_Component<CShader>()->Set_Pass(3);
	}
	else
	{
		Get_Component<CShader>()->Set_Pass(4);
	}

	return S_OK;
}

HRESULT CUIPlayer_HP::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIPlayer_HP::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIPlayer_HP::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIPlayer_HP::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIPlayer_HP::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIPlayer_HP::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	Get_Component<CShader>()->Apply();
	Get_Component<CVIBuffer>()->Bind_Resource();
	Get_Component<CVIBuffer>()->Render();

	return S_OK;
}

HRESULT CUIPlayer_HP::Ready_Components(PLAYER_HP_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUIPlayer_HP::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (!m_isUseColorTint)
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;
	}

	_float HpRatio = 0.5f; //m_pTargetStat->Get_HealthRatio();
	if (FAILED(pShader->Get_Variable("g_fProgressRatio")->SetRawValue(&HpRatio, 0, sizeof(_float))))
		return E_FAIL;

	m_iFillDir = CUIProgress_Component::eFillDir::LEFT;
	if (FAILED(pShader->Get_Variable("g_iFillDir")->SetRawValue(&m_iFillDir, 0, sizeof(uint32_t))))
		return E_FAIL;

	if (FAILED(pShader->Get_Variable("g_vColorTint")->SetRawValue(&m_vColorTint, 0, sizeof(Vec4))))
		return E_FAIL;

	return S_OK;
}

CUIPlayer_HP* CUIPlayer_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIPlayer_HP* pInstance = new CUIPlayer_HP(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIPlayer_HP::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIPlayer_HP::Clone(void* pArg)
{
	CUIPlayer_HP* pInstance = new CUIPlayer_HP(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIPlayer_HP::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIPlayer_HP::Free()
{
	Super::Free();
}
