#include "Engine_pch.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

CCamera::CCamera()
	: Super()
{
	m_matView = Matrix::Identity;
	m_matProjection = Matrix::Identity;
}

CCamera::CCamera(const CCamera& rhs)
	: Super(rhs)
	, m_eProjectionType(rhs.m_eProjectionType)
	, m_fFov(rhs.m_fFov)
	, m_fViewWidth(rhs.m_fViewWidth)
	, m_fViewHeight(rhs.m_fViewHeight)
	, m_fAspectRatio(rhs.m_fAspectRatio)
	, m_fNear(rhs.m_fNear)
	, m_fFar(rhs.m_fFar) 
	, m_matView(rhs.m_matView)
	, m_matProjection(rhs.m_matProjection)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_eProjectionType = pDesc->eProjectionType;
	m_fFov = pDesc->fFov;
	m_fViewWidth = pDesc->fViewWidth;
	m_fViewHeight = pDesc->fViewHeight;
	m_fAspectRatio = m_fViewWidth / m_fViewHeight;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;
	
	Update_Proj();
	return S_OK;
}

void CCamera::Update_View()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();

	Matrix matView = Matrix::Identity;

	if (m_eProjectionType == EProjectionType::PERSPECTIVE)
	{
		matView = ::XMMatrixLookToLH(pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS),
			pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK),
			pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::UP));
	}
	
	m_matView = matView;
}

void CCamera::Update_Proj()
{
	m_fAspectRatio = m_fViewWidth / m_fViewHeight;

	Matrix matProjection = Matrix::Identity;
	switch (m_eProjectionType)
	{
	case Engine::EProjectionType::PERSPECTIVE:
	{
		matProjection = ::XMMatrixPerspectiveFovLH(m_fFov, m_fAspectRatio, m_fNear, m_fFar);
	} break;
	case Engine::EProjectionType::ORTHOGRAPHIC:
	{
		matProjection = ::XMMatrixOrthographicLH(m_fViewWidth, m_fViewHeight, m_fNear, m_fFar);
	} break;
	}
	m_matProjection = matProjection;
}

CCamera* CCamera::Create()
{
	CCamera* pInstance = new CCamera();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CCamera::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCamera::Clone(void* pArg)
{
	CCamera* pClone = new CCamera(*this);

	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CCamera::Clone, Failed");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCamera::Free()
{
	Super::Free();
}
