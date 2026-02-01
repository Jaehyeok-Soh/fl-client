#include "pch.h"
#include "DebugLine.h"
#include "Grid.h"
#include "GameInstance.h"
#include "DebugDraw.h"

CDebugLine::CDebugLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)
	, m_pBatch{ nullptr }
	, m_pEffect(nullptr)
	, m_pInputLayout(nullptr)
{
}

CDebugLine::CDebugLine(const CDebugLine& rhs)
	: CGameObject(rhs)
	, m_pBatch{ rhs.m_pBatch}
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
{
}



HRESULT CDebugLine::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CDebugLine::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CDebugLine::DEBUGLINE_DESC* pDesc = static_cast<CDebugLine::DEBUGLINE_DESC*>(pArg);

	m_vColor_X = pDesc->vColor_X;
	m_vColor_Z = pDesc->vColor_Z;


	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderInput = { nullptr };
	size_t iShaderInputLenght = {};
	m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);

	if (FAILED(m_pDevice->CreateInputLayout(
		VertexPositionColor::InputElements
		, VertexPositionColor::InputElementCount
		, pShaderInput
		, iShaderInputLenght
		, &m_pInputLayout)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDebugLine::Awake(const _uint iCurrentLevelID)
{
	return S_OK;
}

void CDebugLine::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CDebugLine::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CDebugLine::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

}

void CDebugLine::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::PRIORITY, this);
}


HRESULT CDebugLine::Render()
{
	m_pEffect->SetWorld(Get_Component<CTransform>()->Get_WorldMatrix());
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());
	m_pEffect->SetVertexColorEnabled(true);
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	m_pDeviceContext->RSSetState(nullptr);
	m_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);


	m_pEffect->Apply(m_pDeviceContext);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pBatch->Begin();


	float fLineLength = 2000.f; // 2km Á¤µµ·Î ³Ë³ËÇÏ°Ô!
	float fOffset = 0.001f;
	float fY = 0.01f; // ¹Ù´ÚÀÌ¶û ¾È °ãÄ¡°Ô ¾ÆÁÖ »ìÂ¦ ¶ç¿ì±â

	// XÃà (»¡°­)
	m_pBatch->DrawLine(VertexPositionColor(Vec3(-fLineLength, fY, 0.f), Colors::Red), VertexPositionColor(Vec3(fLineLength, fY, 0.f), Colors::Red));
	m_pBatch->DrawLine(VertexPositionColor(Vec3(-fLineLength, fY, fOffset), Colors::Red), VertexPositionColor(Vec3(fLineLength, fY, fOffset), Colors::Red));
	m_pBatch->DrawLine(VertexPositionColor(Vec3(-fLineLength, fY, -fOffset), Colors::Red), VertexPositionColor(Vec3(fLineLength, fY, -fOffset), Colors::Red));

	// ZÃà (Cyan) - ½ÃÀÛÁ¡°ú ³¡Á¡ ¸ðµÎ CyanÀ¸·Î ÅëÀÏÇØ¾ß ±ò²ûÇØ!
	m_pBatch->DrawLine(VertexPositionColor(Vec3(0.f, fY, -fLineLength), Colors::Cyan), VertexPositionColor(Vec3(0.f, fY, fLineLength), Colors::Cyan));
	m_pBatch->DrawLine(VertexPositionColor(Vec3(fOffset, fY, -fLineLength), Colors::Cyan), VertexPositionColor(Vec3(fOffset, fY, fLineLength), Colors::Cyan));
	m_pBatch->DrawLine(VertexPositionColor(Vec3(-fOffset, fY, -fLineLength), Colors::Cyan), VertexPositionColor(Vec3(-fOffset, fY, fLineLength), Colors::Cyan));

	m_pBatch->End();


	return S_OK;
}



CDebugLine* CDebugLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugLine* pDebugLine = new CDebugLine(pDevice,pContext);

	if (FAILED(pDebugLine->Initialize_Prototype()))
	{
		Safe_Release(pDebugLine);
		MSG_BOX("Debug Line is Failed to Create ");
		return nullptr;
	}
	 

	return pDebugLine;
}

CGameObject* CDebugLine::Clone(void* pArg)
{
	CDebugLine* pDebugLine = new CDebugLine(*this);

	if (FAILED(pDebugLine->Initialize(pArg)))
	{
		Safe_Release(pDebugLine);
		MSG_BOX("Debug Line is Failed to Create ");
		return nullptr;
	}

	return pDebugLine;
}




void CDebugLine::Free()
{
	if (IsClone())
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
		Safe_Release(m_pInputLayout);
	}

	Super::Free();
}
