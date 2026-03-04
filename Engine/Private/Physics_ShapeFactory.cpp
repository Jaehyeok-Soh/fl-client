#include "Engine_pch.h"

#include "GameInstance.h"
#include "Physics_ShapeFactory.h"

CPhysics_ShapeFactory::CPhysics_ShapeFactory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
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

HRESULT CPhysics_ShapeFactory::Initialize(class CPhysics_ResourceManager* pResourceManager)
{
	m_pResourceManager = pResourceManager;
	Safe_AddRef(m_pResourceManager);

	return S_OK;
}

vector<PxShape*> CPhysics_ShapeFactory::GetShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	if (pDesc->bIsConvex)
		return MakeShape(pDesc, MakeConvexGeometry(pDesc));
	else
		return MakeShape(pDesc, MakeGeometry(pDesc));
}

vector<PxShape*> CPhysics_ShapeFactory::GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	if (pDesc->eShape == EPhysicsShape::HEIGHT_FIELD)
	{
		//return MakeShape(pDesc, MakeHeightField(pDesc));
		return MakeHeightFieldShapes(pDesc);
	}

	if (pDesc->bIsConvex)
		return MakeShape(pDesc, MakeConvexMeshGeometry(pDesc));
	else
		return MakeShape(pDesc, MakeTriangleMeshGeometry(pDesc));
}

vector<PxShape*> CPhysics_ShapeFactory::CopyShapes(vector<PxShape*>& shapes)
{
	vector<PxShape*> result;

	result.reserve(shapes.size());

	for (auto& srcShape : shapes)
	{
		PxGeometryHolder geom = srcShape->getGeometry();

		PxMaterial* pMaterial = { nullptr };
		if (srcShape->getNbMaterials() > 0)
		{
			PxMaterial* mats[1];
			srcShape->getMaterials(mats, 1);
			pMaterial = mats[0];
		}

		if (pMaterial == nullptr)
			continue;

		PxShape* pNewShape = m_pPhysics->createShape(geom.any(), *pMaterial);

		if (pNewShape)
		{
			pNewShape->setLocalPose(srcShape->getLocalPose());
			pNewShape->setSimulationFilterData(srcShape->getSimulationFilterData());
			pNewShape->setQueryFilterData(srcShape->getQueryFilterData());
			pNewShape->setFlags(srcShape->getFlags());
			pNewShape->setContactOffset(srcShape->getContactOffset());
			pNewShape->setRestOffset(srcShape->getRestOffset());

			result.push_back(pNewShape);
		}
	}

	return result;
}

vector<PxShape*> CPhysics_ShapeFactory::MakeShape(PHYSICSCOLLIDER_DESC* pDesc, vector<PxGeometryHolder> geometries)
{
	vector<PxShape*> result;

	PxMaterial* pMaterial = m_pResourceManager->GetMaterial(&pDesc->tMaterial);
	if (pMaterial == nullptr)
	{
		MSG_BOX("ShapeFactory : material null");
	}

	for (auto& geometry : geometries)
	{
		PxGeometryType::Enum type = geometry.getType();

		bool isValid = false;

		switch (type)
		{
		case physx::PxGeometryType::eSPHERE:
			isValid = geometry.sphere().isValid();
			break;
		case physx::PxGeometryType::ePLANE:
			isValid = geometry.plane().isValid();
			break;
		case physx::PxGeometryType::eCAPSULE:
			isValid = geometry.capsule().isValid();
			break;
		case physx::PxGeometryType::eBOX:
			isValid = geometry.box().isValid();
			break;
		case physx::PxGeometryType::eCONVEXCORE:
			isValid = geometry.convexCore().isValid();
			break;
		case physx::PxGeometryType::eCONVEXMESH:
		{
			const PxConvexMeshGeometry& convexGeom = geometry.convexMesh();

			if (convexGeom.convexMesh == nullptr)
				MSG_BOX("ShapeFactory : convex mesh null reference");

			if (convexGeom.scale.scale.isZero())
				MSG_BOX("ShapeFactory : convex mesh zero scale");

			isValid = geometry.convexMesh().isValid();
		}
		break;
		case physx::PxGeometryType::eTRIANGLEMESH:
		{
			const PxTriangleMeshGeometry& triGeom = geometry.triangleMesh();
			if (triGeom.triangleMesh == nullptr)
				MSG_BOX("ShapeFactory : tri mesh null reference");

			isValid = geometry.triangleMesh().isValid();
		}
		break;
		case physx::PxGeometryType::eHEIGHTFIELD:
			isValid = geometry.heightField().isValid();
			break;
		}

		if (!isValid)
			continue;

		PxShape* shape = m_pPhysics->createShape(geometry.any(), *pMaterial, true);
		if (shape == nullptr)
			MSG_BOX("ShapeFactory : shape null");

		PxGeometryHolder geom = shape->getGeometry();

		PxTransform checkPose = PxTransform(PxIdentity) * shape->getLocalPose();

		result.push_back(shape);
	}

	for (auto* shape : result)
	{
		shape->setLocalPose(PxTransform(PxVec3(pDesc->vCenter.x, pDesc->vCenter.y, pDesc->vCenter.z)));
		if (pDesc->bIsTrigger)
		{
			if (pDesc->bIsSkillTrigger)
			{
				// 트리거 오프 시뮬레이션 온
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
			}
			else
			{
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			}
		}
		else
		{
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
			shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		}
	}

	return result;
}

vector<PxShape*> CPhysics_ShapeFactory::MakeHeightFieldShapes(PHYSICSCOLLIDER_DESC* pDesc)
{
	vector<CPhysics_ResourceManager::HEIGHTFIELD_INFO> hfInfoes = MakeHeightField(pDesc);
	
	vector<PxShape*> result;

	PxMaterial* pMaterial = m_pResourceManager->GetMaterial(&pDesc->tMaterial);
	if (pMaterial == nullptr)
		MSG_BOX("ShapeFactory : material null");

	for (auto& hf : hfInfoes)
	{
		PxGeometryType::Enum type = hf.hfGeom.getType();

		bool isValid = false;

		if (type == physx::PxGeometryType::eHEIGHTFIELD)
			isValid = hf.hfGeom.isValid();
		else
			isValid = false;

		if (!isValid)
			continue;

		PxShape* shape = m_pPhysics->createShape(hf.hfGeom, *pMaterial);
		if (shape == nullptr)
			MSG_BOX("ShapeFactory : shape null");

		PxVec3 offset(hf.minX, 0.f, hf.minZ);
		
		shape->setLocalPose(PxTransform(offset));

		if (pDesc->bIsTrigger)
		{
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		}
		else
		{
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
			shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		}

		result.push_back(shape);
	}

	return result;
}

vector<PxGeometryHolder> CPhysics_ShapeFactory::MakeGeometry(PHYSICSCOLLIDER_DESC* pDesc)
{
	vector<PxGeometryHolder> result;

	switch (pDesc->eShape)
	{
	case EPhysicsShape::SPHERE:
		result.push_back(MakeSphere(pDesc));
		break;

	case EPhysicsShape::CAPSULE:
		result.push_back(MakeCapsule(pDesc));
		break;

	case EPhysicsShape::BOX:
		result.push_back(MakeBox(pDesc));
		break;

	case EPhysicsShape::PLANE:
		result.push_back(MakePlane(pDesc));
		break;

	default:
		result.push_back(MakeBox(pDesc));
		break;
	}

	return result;
}

vector<PxGeometryHolder> CPhysics_ShapeFactory::MakeConvexGeometry(PHYSICSCOLLIDER_DESC* pDesc)
{
	vector<PxGeometryHolder> result;

	switch (pDesc->eConvexShape)
	{
	case EPhysicsConvexShape::POINT:
		result.push_back(MakeConvexPoint(pDesc));
		break;

	case EPhysicsConvexShape::SEGMENT:
		result.push_back(MakeConvexSegment(pDesc));
		break;

	case EPhysicsConvexShape::BOX:
		result.push_back(MakeConvexBox(pDesc));
		break;

	case EPhysicsConvexShape::ELLIPSOID:
		result.push_back(MakeConvexEllipsoid(pDesc));
		break;

	case EPhysicsConvexShape::CYLINDER:
		result.push_back(MakeConvexCylinder(pDesc));
		break;

	case EPhysicsConvexShape::CONE:
		result.push_back(MakeConvexCone(pDesc));
		break;

	default:
		result.push_back(MakeConvexBox(pDesc));
		break;
	}

	return result;
}

vector<CPhysics_ResourceManager::HEIGHTFIELD_INFO> CPhysics_ShapeFactory::MakeHeightField(PHYSICSCOLLIDER_DESC* pDesc)
{
	vector<CPhysics_ResourceManager::HEIGHTFIELD_INFO> result;

	PxReal heightScale = 0.01f;

	if (pDesc->wstrModelPrototypeTag != L"")
	{
		auto fields = GetHeightField(pDesc);

		for (auto& field : fields)
		{
			PxHeightFieldGeometry hfGeom(
				field.pHeightField,
				PxMeshGeometryFlags(),
				heightScale,
				field.rowScale,
				field.colScale);

			field.hfGeom = hfGeom;
		}

		return fields;
	}

	if (pDesc->wstrFilePath != L"")
	{
		auto fields = GetHeightField(pDesc);

		for (auto& field : fields)
		{
			PxHeightFieldGeometry hfGeom(
				field.pHeightField,
				PxMeshGeometryFlags(),
				heightScale,
				field.rowScale,
				field.colScale);

			field.hfGeom = hfGeom;
		}

		return fields;
	}

	return result;
}

vector<PxGeometryHolder> CPhysics_ShapeFactory::MakeTriangleMeshGeometry(PHYSICSCOLLIDER_DESC* pDesc)
{
	vector<PxGeometryHolder> result;

	if (pDesc->wstrModelPrototypeTag != L"")
	{
		auto meshes = GetTriangleMesh(pDesc);

		for (auto& mesh : meshes)
			result.push_back(PxTriangleMeshGeometry(mesh, PxMeshScale(1.f)));

		return result;
	}

	if (pDesc->wstrFilePath != L"")
	{
		auto meshes = GetTriangleMesh(pDesc);

		for (auto& mesh : meshes)
			result.push_back(PxTriangleMeshGeometry(mesh, PxMeshScale(1.f)));

		return result;
	}

	return result;
}

vector<PxGeometryHolder> CPhysics_ShapeFactory::MakeConvexMeshGeometry(PHYSICSCOLLIDER_DESC* pDesc)
{
	vector<PxGeometryHolder> result;

	if (pDesc->wstrModelPrototypeTag != L"")
	{
		auto meshes = GetConvexMesh(pDesc);

		for (auto& mesh : meshes)
			result.push_back(PxConvexMeshGeometry(mesh, PxMeshScale(1.f)));

		return result;
	}

	if (pDesc->wstrFilePath != L"")
	{
		auto meshes = GetConvexMesh(pDesc);

		for (auto& mesh : meshes)
			result.push_back(PxConvexMeshGeometry(mesh, PxMeshScale(1.f)));

		return result;
	}

	return result;
}

PxGeometryHolder CPhysics_ShapeFactory::MakeSphere(PHYSICSCOLLIDER_DESC* pDesc)
{
	return PxSphereGeometry(pDesc->fRadius);
}

PxGeometryHolder CPhysics_ShapeFactory::MakeBox(PHYSICSCOLLIDER_DESC* pDesc)
{
	return PxBoxGeometry(pDesc->vExtents.x / 2.f, pDesc->vExtents.y / 2.f, pDesc->vExtents.z / 2.f);
}

PxGeometryHolder CPhysics_ShapeFactory::MakeCapsule(PHYSICSCOLLIDER_DESC* pDesc)
{
	return PxCapsuleGeometry(pDesc->fRadius, pDesc->fHeight / 2.f);
}

PxGeometryHolder CPhysics_ShapeFactory::MakePlane(PHYSICSCOLLIDER_DESC* pDesc)
{
	return PxPlaneGeometry();
}

PxGeometryHolder CPhysics_ShapeFactory::MakeConvexPoint(PHYSICSCOLLIDER_DESC* pDesc)
{
	PxConvexCore::Point pointCore;
	return PxConvexCoreGeometry(pointCore);
}

PxGeometryHolder CPhysics_ShapeFactory::MakeConvexSegment(PHYSICSCOLLIDER_DESC* pDesc)
{
	PxConvexCore::Segment segmentCore(pDesc->fHeight);
	return PxConvexCoreGeometry(segmentCore);
}

PxGeometryHolder CPhysics_ShapeFactory::MakeConvexBox(PHYSICSCOLLIDER_DESC* pDesc)
{
	PxConvexCore::Box boxCore(pDesc->vExtents.x, pDesc->vExtents.y, pDesc->vExtents.z);
	return PxConvexCoreGeometry(boxCore, pDesc->fMargin);
}

PxGeometryHolder CPhysics_ShapeFactory::MakeConvexEllipsoid(PHYSICSCOLLIDER_DESC* pDesc)
{
	PxConvexCore::Ellipsoid ellipsoidCore(pDesc->vExtents.x, pDesc->vExtents.y, pDesc->vExtents.z);
	return PxConvexCoreGeometry(ellipsoidCore, pDesc->fMargin);
}

PxGeometryHolder CPhysics_ShapeFactory::MakeConvexCylinder(PHYSICSCOLLIDER_DESC* pDesc)
{
	PxConvexCore::Cylinder cylinderCore(pDesc->fHeight, pDesc->fRadius);
	return PxConvexCoreGeometry(cylinderCore, pDesc->fMargin);
}

PxGeometryHolder CPhysics_ShapeFactory::MakeConvexCone(PHYSICSCOLLIDER_DESC* pDesc)
{
	PxConvexCore::Cone coneCore(pDesc->fHeight, pDesc->fRadius);
	return PxConvexCoreGeometry(coneCore, pDesc->fMargin);
}

vector<PxTriangleMesh*> CPhysics_ShapeFactory::GetTriangleMesh(PHYSICSCOLLIDER_DESC* pDesc)
{
	return 	m_pResourceManager->GetTriangleMeshes(pDesc);
}

vector<PxConvexMesh*> CPhysics_ShapeFactory::GetConvexMesh(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pResourceManager->GetConvexMeshes(pDesc);
}

vector<CPhysics_ResourceManager::HEIGHTFIELD_INFO> CPhysics_ShapeFactory::GetHeightField(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pResourceManager->GetHeightFields(pDesc);
}

CPhysics_ShapeFactory* CPhysics_ShapeFactory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene, CPhysics_ResourceManager* pResourceManager)
{
	CPhysics_ShapeFactory* pInstance = new CPhysics_ShapeFactory(pDevice, pContext, pPhysics, pScene);

	if (FAILED(pInstance->Initialize(pResourceManager)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_ShapeFactory");
	}

	return pInstance;
}

void CPhysics_ShapeFactory::Free()
{
	Safe_Release(m_pResourceManager);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	Super::Free();
}
