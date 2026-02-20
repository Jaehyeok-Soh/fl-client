#include "Engine_pch.h"

#include "GameInstance.h"
#include "Physics_ResourceManager.h"
#include "Physics_Utils.h"
#include "Physics_QueryFilterCallback.h"

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
			if (m_bIsOnMeshDebug)
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

HRESULT CPhysics_Utils::Render(const PxGeometry& geom, const PxTransform& transform, XMVECTOR color)
{
	m_pEffect->SetWorld(Matrix::Identity);
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->OMSetDepthStencilState(m_pDSS, 0);

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_pBatch->Begin();

	Matrix pxMatrix = PxTransformToXMMatrix(transform);
	XMMATRIX matWorld = XMLoadFloat4x4(&pxMatrix);

	switch (geom.getType())
	{
	case physx::PxGeometryType::eSPHERE:
	{
		PxSphereGeometry sphere = static_cast<const PxSphereGeometry&>(geom);
		BoundingSphere boundingSphere{};
		boundingSphere.Radius = sphere.radius;
		boundingSphere.Center = Vec3(transform.p.x, transform.p.y, transform.p.z);
		DX::Draw(m_pBatch, boundingSphere);
	}
	break;
	case physx::PxGeometryType::ePLANE:
		break;
	case physx::PxGeometryType::eCAPSULE:
	{
		PxCapsuleGeometry capsule = static_cast<const PxCapsuleGeometry&>(geom);
		BoundingSphere boundingSphereHead{};
		boundingSphereHead.Radius = capsule.radius + 0.1f;
		boundingSphereHead.Center = Vec3(transform.p.x, transform.p.y - 0.1f, transform.p.z);
		DX::DrawCapsule(m_pBatch, boundingSphereHead, capsule.halfHeight + 0.1f);
	}
	break;
	case physx::PxGeometryType::eBOX:
	{
		PxBoxGeometry box = static_cast<const PxBoxGeometry&>(geom);
		BoundingOrientedBox boundingObb{};
		boundingObb.Extents = Vec3(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
		boundingObb.Center = Vec3(transform.p.x, transform.p.y, transform.p.z);
		boundingObb.Orientation = Vec4(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
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
		//{
		//	DX::DrawMesh(m_pBatch, geom, globalPose, matWorld);
		//}
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

PxQuat CPhysics_Utils::GetPureRotation(const Matrix& mat)
{
	Vec3 u = Vec3(mat._11, mat._12, mat._13);
	Vec3 v = Vec3(mat._21, mat._22, mat._23);
	Vec3 w = Vec3(mat._31, mat._32, mat._33);

	u.Normalize();
	v.Normalize();
	w.Normalize();

	PxMat33 pMat(
		PxVec3(u.x, u.y, u.z),
		PxVec3(v.x, v.y, v.z),
		PxVec3(w.x, w.y, w.z)
	);

	return PxQuat(pMat);
}

PxVec3 CPhysics_Utils::GetPureScale(const Matrix& mat)
{
	Vec3 u = Vec3(mat._11, mat._12, mat._13);
	Vec3 v = Vec3(mat._21, mat._22, mat._23);
	Vec3 w = Vec3(mat._31, mat._32, mat._33);

	PxVec3 scale(u.Length(), v.Length(), w.Length());

	if (scale.x < 1e-4f) scale.x = 1.f;
	if (scale.y < 1e-4f) scale.y = 1.f;
	if (scale.z < 1e-4f) scale.z = 1.f;

	u.Normalize();
	v.Normalize();
	w.Normalize();

	return scale;
}

Matrix CPhysics_Utils::GetUnrealMatrix(const Matrix& mat)
{
	Matrix matBasis = g_UnrealToEngineBasis;
	Matrix matBasisInv = matBasis.Transpose();

	return matBasis * mat * matBasisInv;
}

_bool CPhysics_Utils::RayCast(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, CPhysics_QueryFilterCallback* pFilterCall)
{
	// 시작 월드 좌표
	// 쏠 방향 벡터 : 사이즈 꼭 1 이어야함
	// 검사 최대 깊이

	// 1. Vec3 -> PxVec3
	PxVec3 o3 = { vWorldPos.x,vWorldPos.y,vWorldPos.z };

	// 2. 길이가 1이어야 하므로 안정하게 한번더 노말라이즈
	vDir.Normalize();
	PxVec3 d3 = { vDir.x,vDir.y,vDir.z };

	// 2. 필터 데이터 설정 (ePRE_FILTER 필수!)
	PxQueryFilterData filterData;
	filterData.flags |= PxQueryFlag::ePREFILTER; // 콜백 사용하겠다
	filterData.flags |= PxQueryFlag::eSTATIC;    // 정적 물체(지형 등) 검사하겠다
	filterData.flags |= PxQueryFlag::eDYNAMIC;   // 동적 물체(캐릭터 등) 검사하겠다

	// 3. 검사
	if (pFilterCall)
	{
		if (m_bRayHit = m_pScene->raycast(o3, d3, fMaxDist, m_RayCastHitBuffer, PxHitFlag::eDEFAULT, filterData, pFilterCall))
		{
			PxF32 hitDist = m_RayCastHitBuffer.block.distance;

			return m_bRayHit;//m_RayCastHitBuffer.block.actor
		}
	}

	else
	{
		if (m_bRayHit = m_pScene->raycast(o3, d3, fMaxDist, m_RayCastHitBuffer))
		{
			PxF32 hitDist = m_RayCastHitBuffer.block.distance;

			return m_bRayHit;//m_RayCastHitBuffer.block.actor
		}
	}

	return m_bRayHit;
}

_bool CPhysics_Utils::Execute_Overlap(PxGeometry& shape, PxTransform& transform, OUT PxOverlapBuffer& hit, PxQueryFilterData& filterData, PxQueryFilterCallback* filterCallback)
{
	return m_pScene->overlap(shape, transform, hit, filterData, filterCallback);
}

CPhysics_QueryFilterCallback* CPhysics_Utils::GetQueryFilterCallback()
{
	return CPhysics_QueryFilterCallback::Create();
}

#ifdef _DEBUG
void CPhysics_Utils::SetMeshDebugState()
{
	m_bIsOnMeshDebug = !m_bIsOnMeshDebug;
}
#endif

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
