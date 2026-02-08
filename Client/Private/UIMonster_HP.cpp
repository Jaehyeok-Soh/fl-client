#include "pch.h"
#include "UIMonster_HP.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
#include "GameInstance.h"

CUIMonster_HP::CUIMonster_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIMonster_HP::CUIMonster_HP(const CUIMonster_HP& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIMonster_HP::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonster_HP::Initialize(void* pArg)
{
	PLAYER_HP_DESC* pDesc = static_cast<PLAYER_HP_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrTextureTag, 0)))
		return E_FAIL;

	Get_Component<CShader>()->Set_Pass(3);
	return S_OK;
}

HRESULT CUIMonster_HP::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIMonster_HP::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMonster_HP::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMonster_HP::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMonster_HP::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMonster_HP::Render()
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

HRESULT CUIMonster_HP::Ready_Components(PLAYER_HP_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIMonster_HP::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
		return E_FAIL;

	if (Get_Script_Component(L"UIProgress_Component"))
	{
		pShader->Set_Pass(3);
		
		if (FAILED(pShader->Get_Variable("g_fProgressRatio")->SetRawValue(&m_pTargetStat->Get_HealthRatio(), 0, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Get_Variable("g_iFillDir")->SetRawValue(&m_iFillDir, 0, sizeof(uint32_t))))
			return E_FAIL;
	}

	return S_OK;
}

CUIMonster_HP* CUIMonster_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMonster_HP* pInstance = new CUIMonster_HP(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMonster_HP::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMonster_HP::Clone(void* pArg)
{
	CUIMonster_HP* pInstance = new CUIMonster_HP(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMonster_HP::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMonster_HP::Free()
{
	Super::Free();
}
