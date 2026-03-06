#include "Engine_pch.h"
#include "PhysicsAttackRaycast.h"
#include "GameInstance.h"

#include "GameObject.h"
#include "ContainerObject.h"
#include "PartObject.h"

#include "CameraMan.h"

#include "DebugDraw.h"

CPhysicsAttackRaycast::CPhysicsAttackRaycast(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(),
	m_pDevice(pDevice),
	m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CPhysicsAttackRaycast::CPhysicsAttackRaycast(const CPhysicsAttackRaycast& rhs)
	: Super(rhs),
	m_pDevice(rhs.m_pDevice),
	m_pDeviceContext(rhs.m_pDeviceContext)
#ifdef _DEBUG
	, m_pBatch{ rhs.m_pBatch },
	m_pEffect{ rhs.m_pEffect },
	m_pInputLayout{ rhs.m_pInputLayout }
#endif // _DEBUG
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CPhysicsAttackRaycast::Initialize_Prototype(void* pArg)
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderInput = {};
	size_t iShaderInputLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		pShaderInput,
		iShaderInputLength,
		&m_pInputLayout)))
	{
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CPhysicsAttackRaycast::Initialize(void* pArg)
{
	m_tDesc = *static_cast<ATTACKRAYCASTDESC*>(pArg);

	m_filterData.data.word0 = m_tDesc.eFilterLayer;
	m_filterData.data.word1 = m_tDesc.iFilterMask;
	m_filterData.flags = PxQueryFlag::ePREFILTER
		| PxQueryFlag::eDYNAMIC
		| PxQueryFlag::eSTATIC;

	if (m_tDesc.strAttackPresetTag.size() > 0)
		m_tDesc.iAttackPresetID = m_pGameInstance->Get_AttackPresetIdByTag(m_tDesc.strAttackPresetTag);

	m_pFilterCallback = m_pGameInstance->GetQueryFilterCallback();
	m_pFilterCallback->SetOwner(Get_Owner());

	m_pScene = m_pGameInstance->GetPhysicsScene();
	return S_OK;
}

#ifdef _DEBUG
void CPhysicsAttackRaycast::Render()
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());

	m_pEffect->Apply(m_pDeviceContext);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_pBatch->Begin();

	CTransform* pCameraTransform = m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>();

	Vec3 vDebugPos = pCameraTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	vDebugPos.y -= 0.5f;
	
	Vec3 vDebugLook = pCameraTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);

	DX::DrawRay(m_pBatch, vDebugPos, XMLoadFloat3(&vDebugLook) * 1000.f, false, DirectX::Colors::Red);

	m_pBatch->End();
}
#endif // _DEBUG

_bool CPhysicsAttackRaycast::ShootRay(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, _float* hitDist)
{
	PxVec3 o3 = ToPxVec3(vWorldPos);

	vDir.Normalize();
	PxVec3 d3 = ToPxVec3(vDir);

	if (m_bRayHit = m_pScene->raycast(o3, d3, fMaxDist, m_RayCastHitBuffer, PxHitFlag::eDEFAULT, m_filterData, m_pFilterCallback))
	{
		*hitDist = m_RayCastHitBuffer.block.distance;

		m_pGameInstance->Raycast_EventCallback(m_pOwner, &m_RayCastHitBuffer, &m_tDesc);

		return m_bRayHit;
	}
}

CPhysicsAttackRaycast* CPhysicsAttackRaycast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CPhysicsAttackRaycast* pInstance = new CPhysicsAttackRaycast(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CPhysicsAttackRaycast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysicsAttackRaycast::Clone(void* pArg)
{
	CPhysicsAttackRaycast* pInstance = new CPhysicsAttackRaycast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysicsAttackRaycast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysicsAttackRaycast::Free()
{
	Safe_Release(m_pFilterCallback);
	
#ifdef _DEBUG
	if (false == IsClone())
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Super::Free();
}
