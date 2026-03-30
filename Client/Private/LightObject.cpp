#include "pch.h"
#include "LightObject.h"
#include "Light.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"
#include "GameInstance.h"

CLightObject::CLightObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
	, m_pLight{ nullptr }
	, m_isFlicker{ false }
	, m_fFlickerSpeed{ 1.f }
	, m_fFlickerMin{ 0.5f }
	, m_fBaseRange{ 1.f }
	, m_fAccDT{ 0.f }
	, m_isRenderModel{ false }
	, m_eShaderPass{ EMapObjectShaderPass::StaticObject }
	, m_pLightCollider{nullptr}
	, m_fEmissivePower{1.f}
	, m_fCurLightRangeRatio{0.f}
{
}

CLightObject::CLightObject(const CLightObject& rhs)
	: CMapObject(rhs)
	, m_pLight{ rhs.m_pLight }
	, m_isFlicker{ rhs.m_isFlicker }
	, m_fFlickerSpeed{ rhs.m_fFlickerSpeed }
	, m_fFlickerMin{ rhs.m_fFlickerMin }
	, m_fBaseRange{ rhs.m_fBaseRange }
	, m_fAccDT{ rhs.m_fAccDT }
	, m_isRenderModel{ rhs.m_isRenderModel }
	, m_eShaderPass{rhs.m_eShaderPass }
	, m_pLightCollider{rhs.m_pLightCollider }
	, m_fEmissivePower{rhs.m_fEmissivePower }
	, m_fCurLightRangeRatio{rhs.m_fCurLightRangeRatio }
{
}

HRESULT CLightObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CLightObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CLightObject::LIGHTOBJECT_DESC* pDesc = static_cast<CLightObject::LIGHTOBJECT_DESC*>(pArg);

	m_isFlicker = pDesc->isFlicker;
	m_fBaseRange = pDesc->tLightDesc.fRange;
	m_fEmissivePower = pDesc->fEmissivePower;

	if (m_isFlicker)
	{
		m_fFlickerSpeed = pDesc->fFlickerSpeed;
		m_fFlickerMin = pDesc->fFlickerMin;
	}

	if (FAILED(Ready_MI()))
		return E_FAIL;

	if (FAILED(Ready_Light(pDesc->tLightDesc)))
		return E_FAIL;

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightObject::Ready_MI()
{
	CModel* pModel = Get_Component<CModel>();
	_uint iCount =  (_uint)pModel->Get_MaterialCount();

	for (_uint i = 0; i < iCount; ++i)
	{
		pModel->Change_MI(i,EMaterialInstanceType::Free);
	}

	return S_OK;
}

HRESULT CLightObject::Ready_Light(const LIGHT_DESC& tLightDesc)
{
	m_pLight = CLight::Create(tLightDesc);
	if (m_pLight == nullptr) return E_FAIL;

	return S_OK;
}



HRESULT CLightObject::Ready_Collider()
{
	/* Collider Setting 전에 SetScale 1 1 1 로 설정 */
	CTransform* pTs = Get_Component<CTransform>();
	if (pTs == nullptr) return E_FAIL;
	pTs->Set_Scale(1.f, 1.f, 1.f);


	CBounding_Sphere::BOUNDING_SPHERE_DESC tBoundingSphere_Desc{};
	tBoundingSphere_Desc.fRadius = m_fBaseRange;
	tBoundingSphere_Desc.vCenter = { 0.f,0.f,0.f };

	CCollider::COLLIDER_DESC tColliderDesc{};
	tColliderDesc.pBoundingDesc = &tBoundingSphere_Desc;

	m_pLightCollider =  static_cast<CCollider*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), g_wszCollider_Sphere_PrototypeTag, &tColliderDesc));
	if (m_pLightCollider == nullptr) return E_FAIL;


	return S_OK;
}


HRESULT CLightObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CLightObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CLightObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	m_fAccDT += fTimeDelta;

	_float fFinalRange = m_fBaseRange;
	if (m_isFlicker)
	{
		float fSinValue = sinf(m_fAccDT * m_fFlickerSpeed);	/* -1 ~ 1 사이 값 */
		fSinValue = (fSinValue + 1.f) * 0.5f;			/* 0.f ~ 1.f 사이값으로 변환 */
		m_fCurLightRangeRatio = m_fFlickerMin + fSinValue * (1.f - m_fFlickerMin);
		fFinalRange = m_fBaseRange * m_fCurLightRangeRatio;
		m_pLight->Setup_Range(fFinalRange);
	}
	else
		m_fCurLightRangeRatio = 1.f;


	/* 깜빡거리는 애들이라면 프레임단위로 계산해준다 */
	CTransform* pTs = Get_Component<CTransform>();
	Vec3 vPos = pTs->Get_Info(TRANSFORM_INFO_STATE::POS);
	m_pLight->Setup_Position(Vec4(vPos.x, vPos.y, vPos.z, 1.f));



	/* Collider Update */
	static_cast<CBounding_Sphere*>(m_pLightCollider->Get_Bounding())->Get_OriginalDesc()->Radius = fFinalRange * 1.25f;
	m_pLightCollider->Update(pTs->Get_WorldMatrix());

}

void CLightObject::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CLightObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);


	/* 점조명이기에 매프레임 넣어준다 */
	EFrustrumTier eType = m_pGameInstance->Classify_BySplitFrustrum(*static_cast<CBounding_Sphere*>(m_pLightCollider->Get_Bounding())->Get_Desc());

	if (eType == EFrustrumTier::Near)
	{
		if (m_pLight)
		{
			m_pGameInstance->Push_Light(m_pLight);
		}
	}
	else
	{
		int a = 0;
	}

#ifdef _DEBUG
	CCollider* pCollider = Get_Component<CCollider>();
	if (pCollider)
		m_pGameInstance->Push_DebugComponent(pCollider);
#endif // _DEBUG
}

HRESULT CLightObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;


	CShader* pShader = Get_Component<CShader>();            if (pShader == nullptr)         return E_FAIL;
	CModel* pModel = Get_Component<CModel>();               if (pModel == nullptr)          return E_FAIL;
	CTransform* pTransform = Get_Component<CTransform>();   if (pTransform == nullptr)      return E_FAIL;
	if (!m_pLight) return E_FAIL;

	const SHADER_LIGHTDESC& tLightDesc = m_pLight->Get_LightDesc();
	Vec4 vEmissiveColor = m_pLight->Get_LightDesc().vDiffuse;
	float fRange = tLightDesc.fRange;

	/* 내 현재 Power 값을  */

	pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
	_uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

	/* Client Make Path를 이용한다 */
	/* Emissive */
	/* Emissive 관련해서  */
	pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::LightObject));

	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Set_MI_TintColor(i , Vec4(1.f,1.f,1.f,1.f) );
		pModel->Set_MI_EmissiveColor(i, vEmissiveColor);
		pModel->Set_MI_EmissivePower(i, m_fEmissivePower * m_fCurLightRangeRatio);	/* 기본적인 Emmisve 파워를 뜻할듯 */
		pModel->Bind_MaterialInstance(pShader, i);
		pModel->Bind_Material(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}



	return S_OK;
}

CLightObject* CLightObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightObject* pLightObject = new CLightObject(pDevice, pContext);

	if (FAILED(pLightObject->Initialize_Prototype()))
	{
		Safe_Release(pLightObject);
		MSG_BOX("Point Light is failed to Create");
		return nullptr;
	}
	return pLightObject;
}

CGameObject* CLightObject::Clone(void* pArg)
{
	CLightObject* pLightObject = new CLightObject(*this);

	if (FAILED(pLightObject->Initialize(pArg)))
	{
		Safe_Release(pLightObject);
		MSG_BOX("Point Light is failed to Clone");
		return nullptr;
	}
	return pLightObject;
}

void CLightObject::Free()
{
	Super::Free();
	Safe_Release(m_pLight);
	Safe_Release(m_pLightCollider);
}
