#include "Engine_pch.h"
#include "CinematicCameraSequnce.h"

CCinematicCameraSequnce::CCinematicCameraSequnce(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice		(pDevice)
	, m_pContext	(pContext)
	, m_pBatch		{ nullptr }
	, m_pEffect		{ nullptr }
	, m_pInputLayout{ nullptr }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CCinematicCameraSequnce::CCinematicCameraSequnce(const CCinematicCameraSequnce& rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{rhs.m_pContext}
	, m_pEffect{nullptr}
	, m_pBatch{ nullptr }
	, m_pInputLayout{nullptr}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCinematicCameraSequnce::Initialize()
{
	/* Ready */
	if (FAILED(Ready_Batch()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCinematicCameraSequnce::Ready_Batch()
{
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	const void* pShaderInput = { nullptr };
	size_t iShaderInputLenght = {};
	m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &m_pInputLayout);

	return S_OK;
}


CCinematicCameraSequnce* CCinematicCameraSequnce::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicCameraSequnce* pCCS = new CCinematicCameraSequnce(pDevice,pContext);

	if (FAILED(pCCS->Initialize()))
	{
		Safe_Release(pCCS);
		MSG_BOX(" Cinemaitc Camera Sequence is failed to Craete ");
		return nullptr;
	}

	return pCCS;
}

CCinematicCameraSequnce* CCinematicCameraSequnce::Clone(const CCinematicCameraSequnce& rhs)
{
	CCinematicCameraSequnce* pCCs = new CCinematicCameraSequnce(rhs);
	if (pCCs->Initialize())
	{
		Safe_Release(pCCs);
		MSG_BOX("Cinemaitc Camera Sequence is failed to Clone");
		return nullptr;
	}
	return pCCs;
}

void CCinematicCameraSequnce::Free()
{
	Super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	/* CCinematicCameraSequence::Free() */
}
