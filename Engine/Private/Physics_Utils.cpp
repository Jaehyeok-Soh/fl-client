#include "Engine_pch.h"

#include "GameInstance.h"
#include "Physics_ResourceManager.h"
#include "Physics_Utils.h"

#include "DebugDraw.h"

CPhysics_Utils::CPhysics_Utils(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pPhysics{ pPhysics },
	m_pScene{ pScene }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPhysics_Utils::Initialize()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext, 200000, 200000);
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

	D3D11_DEPTH_STENCIL_DESC dssDesc{};
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

	m_pDevice->CreateDepthStencilState(&dssDesc, &m_pDSS);
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPhysics_Utils::Render(PxRigidActor* pActor, XMVECTOR color)
{
	m_pEffect->SetWorld(Matrix::Identity);
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->OMSetDepthStencilState(m_pDSS, 0);

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_pBatch->Begin();

	//  const PxRenderBuffer& rb = m_pScene->getRenderBuffer();
	//  rb.getNbLines()
	//	const PxDebugLine& line = rb.getLines()[i];
	//	line.pos0
	//	line.pos1
	//	line.color0
	//  PxDebugLine (start, end, color)
	//  PxDebugTrinagle

	vector<PxShape*> shapes;
	shapes.resize(m_iMaxRenderShape);
	PxU32 numShape = pActor->getShapes(shapes.data(), m_iMaxRenderShape);

	for (PxU32 i = 0; i < numShape; i++)
	{
		PxShape* shape = shapes[i];

		PxTransform globalPose = PxShapeExt::getGlobalPose(*shape, *pActor);
		Matrix pxMatrix = PxTransformToXMMatrix(globalPose);
		XMMATRIX matWorld = XMLoadFloat4x4(&pxMatrix);
		PxGeometryHolder geom = shape->getGeometry();

		switch (geom.getType())
		{
		case physx::PxGeometryType::eSPHERE:
		{
			PxSphereGeometry sphere = geom.sphere();
			BoundingSphere boundingSphere{};
			boundingSphere.Radius = sphere.radius;
			boundingSphere.Center = Vec3(globalPose.p.x, globalPose.p.y, globalPose.p.z);
			DX::Draw(m_pBatch, boundingSphere);
		}
		break;
		case physx::PxGeometryType::ePLANE:
		{
			_float fGridSize = 50.f;
			size_t numDiv = 200;

			Vec4 yAxis = matWorld.r[0];
			Vec4 zAxis = matWorld.r[2];
			Vec4 origin = matWorld.r[3];

			Vec4 gridAxis1 = yAxis * fGridSize;
			Vec4 gridAxis2 = zAxis * fGridSize;

			DX::DrawGrid(m_pBatch, XMLoadFloat4(&gridAxis1), XMLoadFloat4(&gridAxis2), origin, numDiv, numDiv);
		}
			break;
		case physx::PxGeometryType::eCAPSULE:
		{
			PxCapsuleGeometry capsule = geom.capsule();
			BoundingSphere boundingSphereHead{};
			boundingSphereHead.Radius = capsule.radius + 0.1f;
			boundingSphereHead.Center = Vec3(globalPose.p.x, globalPose.p.y - 0.1f, globalPose.p.z);
			DX::DrawCapsule(m_pBatch, boundingSphereHead, capsule.halfHeight + 0.1f);
		}
		break;
		case physx::PxGeometryType::eBOX:
		{
			PxBoxGeometry box = geom.box();
			BoundingOrientedBox boundingObb{};
			boundingObb.Extents = Vec3(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
			boundingObb.Center = Vec3(globalPose.p.x, globalPose.p.y, globalPose.p.z);
			boundingObb.Orientation = Vec4(globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w);
			DX::Draw(m_pBatch, boundingObb);
		}
		break;
		case physx::PxGeometryType::eCONVEXCORE:
			break;
		case physx::PxGeometryType::eCONVEXMESH:
			break;
		case physx::PxGeometryType::ePARTICLESYSTEM:
			break;
		case physx::PxGeometryType::eTETRAHEDRONMESH:
			break;
		case physx::PxGeometryType::eTRIANGLEMESH:
		{
			DX::DrawMesh(m_pBatch, geom, globalPose, matWorld);
		}
		break;
		case physx::PxGeometryType::eHEIGHTFIELD:
			break;
		case physx::PxGeometryType::eCUSTOM:
			break;
		case physx::PxGeometryType::eGEOMETRY_COUNT:
			break;
		case physx::PxGeometryType::eINVALID:
			break;
		default:
			break;
		}
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

PxTransform CPhysics_Utils::XMMatrixToPxTransform(Matrix mat)
{
	PxTransform result;

	Vec3 scale = {};
	Quat quaternion = {};
	Vec3 translation = {};
	mat.Decompose(scale, quaternion, translation);

	return PxTransform(PxVec3(translation.x, translation.y, translation.z), PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
}

Matrix CPhysics_Utils::PxTransformToXMMatrix(PxTransform pxTransform)
{
	Matrix matQuat, matTrans;

	Quat vQuat(pxTransform.q.x, pxTransform.q.y, pxTransform.q.z, pxTransform.q.w);
	Vec4 vTrans(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z, 1.f);
	
	matQuat = XMMatrixRotationQuaternion(vQuat);
	matTrans = XMMatrixTranslation(vTrans.x, vTrans.y, vTrans.z);

	return matQuat * matTrans;
}

_bool CPhysics_Utils::RayCast()
{
	PxVec3 o3(0.f, 0.f, 0.f);
	PxVec3 d3(0.f, 0.f, 0.f);
	PxReal dist{};
	if (m_bRayHit = m_pScene->raycast(o3, d3, dist, m_RayCastHitBuffer))
		return m_bRayHit;//m_RayCastHitBuffer.block.actor

	return m_bRayHit;
}

CPhysics_Utils* CPhysics_Utils::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
{
	CPhysics_Utils* pInstance = new CPhysics_Utils(pDevice, pContext, pPhysics, pScene);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_Utils");
	}

	return pInstance;
}

void CPhysics_Utils::Free()
{
#ifdef _DEBUG
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);

	Safe_Release(m_pInputLayout);
	Safe_Release(m_pDSS);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	Super::Free();
}
