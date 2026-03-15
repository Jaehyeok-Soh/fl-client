#include "pch.h"
#include "Grass.h"
#include "Shader.h"
#include "Model.h"
#include "InstanceMesh.h"
#include "Body.h"
#include "Player.h"
#include "PhysicsCCT.h"

#include "GameInstance.h"

CGrass::CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPlants(pDevice, pDeviceContext)
	, m_tGrassData{}
	, m_pCBGrassData{nullptr}
{
	m_ePlantsType = CPlants::Type::Grass;
}

CGrass::CGrass(const CGrass& rhs)
	: CPlants(rhs)
	, m_tGrassData{rhs.m_tGrassData }
	, m_pCBGrassData{rhs.m_pCBGrassData }
{
}


HRESULT CGrass::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CGrass::Grass_Desc* pDesc = static_cast<CGrass::Grass_Desc*>(pArg);

	m_tGrassData.g_fGrassSwaySpeed	= pDesc->fGrassSwaySpeed;
	m_tGrassData.g_fGrassWaveSize	= pDesc->fGrassWaveSize;


	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Ready_Component(GRASS_DESC* pDesc)
{
	/* Effect Data 정리 */
	CShader* pShader = Get_Component<CShader>();
	if (pShader == nullptr) return E_FAIL;
	
	m_pCBGrassData = pShader->Get_ConstantBuffer("CB_GrassData");
	if (m_pCBGrassData->IsValid() == false) return E_FAIL;

	/* Model Min Max */
	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr) return E_FAIL;
	const Vec3* pMinMax =pModel->Get_StaticModelMinMax();
	if (pMinMax == nullptr) return E_FAIL;
	m_tGrassData.g_fGrassMaxHeight =  pMinMax[1].y;

	return S_OK;
}

HRESULT CGrass::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CGrass::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGrass::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	m_tGrassData.g_fGrassDT += fTimeDelta;
	if (m_tGrassData.g_fGrassDT > 1000.f)
		m_tGrassData.g_fGrassDT = 0.f;
}

void CGrass::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CGrass::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CGrass::Render()
{
	SHADER_PLAYER_INFO tInfo{};
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer));
	if (pPlayer == nullptr)
		return E_FAIL;


	if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
	{

		CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)         return E_FAIL;
		CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)          return E_FAIL;
		CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)      return E_FAIL;
		/* 제일 앞에있는 친구의 Desc을 참고해서 해준다 모든애들을 가져올수없음 */  

		pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
		_uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

		pPlayer->Bind_PlayerInfo(pShader);

		/* Client Make Path를 이용한다 */
		pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Grass));

		if (m_pCBGrassData == nullptr) return E_FAIL;
		if (FAILED(m_pCBGrassData->SetRawValue(&m_tGrassData, 0, sizeof(CB_GrassData))))
			return E_FAIL;


		for (_uint i = 0; i < iMeshCount; ++i)
		{
			pModel->Set_MI_TintColor(i, m_vMI_TintColor);
			pModel->Bind_Material(pShader, i);
			pModel->Bind_MaterialInstance(pShader, i);
			pShader->Apply();
			pModel->Render(i);
		}

	}
	else if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
	{
		CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)             return E_FAIL;
		CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)              return E_FAIL;
		CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)          return E_FAIL;
		CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();                      if (pInstanceMesh == nullptr)       return E_FAIL;

		if (m_pCBGrassData == nullptr) return E_FAIL;
		if (FAILED(m_pCBGrassData->SetRawValue(&m_tGrassData, 0, sizeof(CB_GrassData))))
			return E_FAIL;


		_uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
		_uint iInstanceCount = pInstanceMesh->Get_InstanceCount();

		pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
		pPlayer->Bind_PlayerInfo(pShader);


		/* Client Make Path를 이용한다 */
		pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Grass));


		pInstanceMesh->Bind_Instance(1);
		for (_uint i = 0; i < iMeshCount; ++i)
		{
			pModel->Set_MI_TintColor(i, m_vMI_TintColor);
			pModel->Bind_Material(pShader, i);
			pModel->Bind_MaterialInstance(pShader, i);
			pShader->Apply();
			pModel->Render_Instance(i, iInstanceCount);
		}
		pInstanceMesh->Unbind_Resource(1);
	}
	else
		return E_FAIL;

	return S_OK;
}

CGrass* CGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGrass* pGrass = new CGrass(pDevice, pDeviceContext);

	if (FAILED(pGrass->Initialize_Prototype()))
	{
		Safe_Release(pGrass);
		MSG_BOX("Grass is failed to Create");
		return nullptr;
	}
	return pGrass;
}

CGameObject* CGrass::Clone(void* pArg)
{
	CGrass* pGrass = new CGrass(*this);

	if (FAILED(pGrass->Initialize(pArg)))
	{
		Safe_Release(pGrass);
		MSG_BOX(" Grass is failed to Clone ");
		return nullptr;
	}

	return pGrass;
}

void CGrass::Free()
{
	Super::Free();
}
