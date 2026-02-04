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

_bool CPhysics_Utils::HasNegativeScale(const Matrix& mat)
{
	Vec4 detVec = XMMatrixDeterminant(mat);
	if (XMVectorGetX(detVec) >= -1e-4)
		return false;

	Matrix matUE = GetUnrealMatrix(mat);
	Vec4 detVecUE = XMMatrixDeterminant(matUE);

	return XMVectorGetX(detVecUE) < -1e-4f;
}

_int CPhysics_Utils::GetNegativeScaleAxis(const Matrix& mat)
{
	//ver1
	////////////////////////////////////////////////////////////////////////////////////

	//Vec3 u = Vec3(mat._11, mat._12, mat._13);
	//Vec3 v = Vec3(mat._21, mat._22, mat._23);
	//Vec3 w = Vec3(mat._31, mat._32, mat._33);

	//u.Normalize();
	//v.Normalize();
	//w.Normalize();

	//float det = u.Cross(v).Dot(w);

	//if (det > -1e-4f)
	//{
	//	return -1;
	//}

	//float scoreX = -u.x + v.y + w.z;

	//float scoreY = u.x - v.y + w.z;

	//float scoreZ = u.x + v.y - w.z;

	//if (scoreX >= scoreY && scoreX >= scoreZ)
	//	return 0; // "X축이 음수 스케일입니다"

	//else if (scoreY > scoreX && scoreY >= scoreZ)
	//	return 1; // "Y축이 음수 스케일입니다"

	//else
	//	return 2; // "Z축이 음수 스케일입니다"
	
	//ver2
	////////////////////////////////////////////////////////////////////////////////////

	//Matrix matUE = GetUnrealMatrix(mat);

	//Vec3 u = Vec3(matUE._11, matUE._12, matUE._13);
	//Vec3 v = Vec3(matUE._21, matUE._22, matUE._23);
	//Vec3 w = Vec3(matUE._31, matUE._32, matUE._33);

	//u.Normalize();
	//v.Normalize();
	//w.Normalize();

	//float det = u.Cross(v).Dot(w);

	//if (det > -1e-4f)
	//{
	//	return -1;
	//}

	//float scoreX = -u.x + v.y + w.z;

	//float scoreY = u.x - v.y + w.z;

	//float scoreZ = u.x + v.y - w.z;

	//if (scoreX >= scoreY && scoreX >= scoreZ)
	//	return 0; // Unreal X Flip -> Engine X Flip

	//else if (scoreY > scoreX && scoreY >= scoreZ)
	//	return 2; // Unreal Y Flip -> Engine Z Flip

	//else
	//	return 1; // Unreal Z Flip -> Engine Y Flip

	//ver3
	////////////////////////////////////////////////////////////////////////////////////

	// 1. 언리얼 기준 행렬로 변환
	Matrix matUE = GetUnrealMatrix(mat);

	Vec3 u = Vec3(matUE._11, matUE._12, matUE._13);
	Vec3 v = Vec3(matUE._21, matUE._22, matUE._23);
	Vec3 w = Vec3(matUE._31, matUE._32, matUE._33);

	u.Normalize(); v.Normalize(); w.Normalize();

	// 2. 점수 계산
	float scoreX = -u.x + v.y + w.z;
	float scoreY = u.x - v.y + w.z;
	float scoreZ = u.x + v.y - w.z;

	// 3. [최종 수정] "압도적인 1등"인지 검사 (동점자 처리)

	// 가장 높은 점수 찾기
	float maxScore = max(scoreX, max(scoreY, scoreZ));

	// 1등과 비슷한 점수가 몇 개인지 세어봅니다.
	int tieCount = 0;
	if (abs(scoreX - maxScore) < 0.1f) tieCount++;
	if (abs(scoreY - maxScore) < 0.1f) tieCount++;
	if (abs(scoreZ - maxScore) < 0.1f) tieCount++;

	// ★ 핵심: 1등이 2명 이상이면(예: Z-180 회전은 X,Y 동점) -> 미러링 아님!
	// 진짜 미러링은 혼자서 3점이고 나머지는 -1점이라 tieCount가 무조건 1임.
	if (tieCount > 1)
	{
		return -1; // "이건 미러링이 아니라 그냥 회전입니다. 건드리지 마세요."
	}

	// 4. 여기까지 왔으면 진짜 미러링임 (범인 색출)
	// 매핑: UE X -> Eng X(0), UE Y -> Eng Z(2), UE Z -> Eng Y(1)
	if (scoreX >= scoreY && scoreX >= scoreZ) return 0;
	else if (scoreY > scoreX && scoreY >= scoreZ) return 2;
	else return 1;
}

PxQuat CPhysics_Utils::GetPureRotation(const Matrix& mat)
{
	Vec3 u = Vec3(mat._11, mat._12, mat._13);
	Vec3 v = Vec3(mat._21, mat._22, mat._23);
	Vec3 w = Vec3(mat._31, mat._32, mat._33);

	u.Normalize();
	v.Normalize();
	w.Normalize();

	_int negativeAxis = GetNegativeScaleAxis(mat);

	if (negativeAxis != -1)
	{
		switch (negativeAxis)
		{
		case 0: u *= -1.0f; break; // X 스케일 반전이면 -> 회전 X축 반전
		case 1: v *= -1.0f; break; // Y 스케일 반전이면 -> 회전 Y축 반전
		case 2: w *= -1.0f; break; // Z 스케일 반전이면 -> 회전 Z축 반전
		default: u *= -1.0f; break; // 안전장치
		}
	}

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

	float det = u.Cross(v).Dot(w);

	_int negativeScaleAxis = GetNegativeScaleAxis(mat);
	if (negativeScaleAxis != -1)
	{
		switch (negativeScaleAxis)
		{
		case 0: scale.x *= -1.f; break;
		case 1: scale.y *= -1.f; break;
		case 2: scale.z *= -1.f; break;
		default: scale.x *= -1.f; break;
		}
	}

	return scale;
}

Matrix CPhysics_Utils::GetUnrealMatrix(const Matrix& mat)
{
	Matrix matBasis = g_UnrealToEngineBasis;
	Matrix matBasisInv = matBasis.Transpose();

	return matBasis * mat * matBasisInv;
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
