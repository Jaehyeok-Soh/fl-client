#include "pch.h"
#include "Plants.h"

#include "Shader.h"
#include "Model.h"
#include "InstanceMesh.h"
#include "GameInstance.h"

CPlants::CPlants(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CMapObject(pDevice, pDeviceContext), m_ePlantsType{ Type::END }, m_vMI_TintColor{1.f,1.f,1.f,1.f}
{
}

CPlants::CPlants(const CPlants& rhs)
	: CMapObject(rhs) , m_ePlantsType(rhs.m_ePlantsType) , m_vMI_TintColor{rhs.m_vMI_TintColor}
{ 
}

HRESULT CPlants::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlants::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;


    PLANTS_DESC* pDesc = static_cast<PLANTS_DESC*>(pArg);

    this->m_vMI_TintColor = pDesc->vMI_TintColor;


    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;


	return S_OK;
}


HRESULT CPlants::Ready_Component(PLANTS_DESC* pDesc)
{
    CModel* pModel = Get_Component<CModel>();
    if (!pModel) return E_FAIL;

    
    _uint iMtlCount = pModel->Get_MaterialCount();

    /* Plants들을 모두 Free타입으로바꿔준다 */
    for (_uint i = 0; i < iMtlCount; ++i)
    {
        pModel->Change_MI(i,Engine::EMaterialInstanceType::Free);
    }


	return S_OK;
}

HRESULT CPlants::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CPlants::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CPlants::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CPlants::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);
}

void CPlants::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

}

HRESULT CPlants::Render()
{
	if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
	{

	}


	return S_OK;
}

HRESULT CPlants::Render_Plnats(_uint iShaderPassIndex)
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
    {

        CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)         return E_FAIL;
        CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)          return E_FAIL;
        CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)      return E_FAIL;
        /* 제일 앞에있는 친구의 Desc을 참고해서 해준다 모든애들을 가져올수없음 */


        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(iShaderPassIndex);
        pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i,m_vMI_TintColor);
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

        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
        _uint iInstanceCount = pInstanceMesh->Get_InstanceCount();


        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(iShaderPassIndex);;
        pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
        pInstanceMesh->Bind_Instance(1);
        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i , m_vMI_TintColor);
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

void CPlants::Free()
{
	Super::Free();

	return;
}
