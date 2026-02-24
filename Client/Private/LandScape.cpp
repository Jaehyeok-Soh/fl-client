#include "pch.h"
#include "LandScape.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

CLandScape::CLandScape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
	, m_iIndex			{ 0 }
	, m_vTextureUV_LT	{ 0.f,0.f }
	, m_vTextureUV_RB	{ 0.f,0.f }
{
}

CLandScape::CLandScape(const CLandScape& rhs)
	: CMapObject(rhs)
	, m_iIndex{ rhs.m_iIndex }
	, m_vTextureUV_LT{ rhs.m_vTextureUV_LT }
	, m_vTextureUV_RB{ rhs.m_vTextureUV_RB }
{
}

HRESULT CLandScape::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLandScape::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	LANDSCAPE_DESC* pDesc = static_cast<LANDSCAPE_DESC*>(pArg);

	/* Land Scape 는 Instance 형식이 될수 없다 */
	if (pDesc->eMapObjectDrawType == EMapObject_DrawType::Instance)
		return E_FAIL;
	
	/* LandScape 관련 정보 세팅 */
	m_iIndex = pDesc->iIndex;
	m_vTextureUV_LT = pDesc->vTextureUV_LT;
	m_vTextureUV_RB = pDesc->vTextureUV_RB;


	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLandScape::Ready_Component(LANDSCAPE_DESC* pDesc)
{
	if (pDesc == nullptr) return E_FAIL;

	/* Model 준비는 Map object에서 알아서 된다 */


	return S_OK;
}

HRESULT CLandScape::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CLandScape::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);

}

void CLandScape::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CLandScape::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CLandScape::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

}

HRESULT CLandScape::Render()
{
	CShader*	pShader		= Get_Component<CShader>();      if (pShader == nullptr)             return E_FAIL;
	CModel*		pModel		= Get_Component<CModel>();       if (pModel == nullptr)              return E_FAIL;
	CTransform* pTransform	= Get_Component<CTransform>();   if (pTransform == nullptr)          return E_FAIL;

	//if (FAILED(m_pGameInstance->GameDataManager_Bind_SplatingTextureInfo(pShader, L"Village")))
	//	return E_FAIL;

	/* UV 좌표 업데이트 */
	if (FAILED(pShader->Get_Vector("g_LandScape_TextureUV_LT")->SetRawValue(&m_vTextureUV_LT, 0, sizeof(m_vTextureUV_LT)))) return E_FAIL;
	if (FAILED(pShader->Get_Vector("g_LandScape_TextureUV_RB")->SetRawValue(&m_vTextureUV_RB, 0, sizeof(m_vTextureUV_RB)))) return E_FAIL;

	_uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());


	pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
	pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::LandScape));


	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

CLandScape* CLandScape::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLandScape* pLandScape = new CLandScape(pDevice, pContext);

	if (FAILED(pLandScape->Initialize_Prototype()))
	{
		Safe_Release(pLandScape);
		MSG_BOX(" Land Scape is failed to Create ");
		return nullptr;
	}

	return pLandScape;
}

CGameObject* CLandScape::Clone(void* pArg)
{
	CLandScape* pLandScape = new CLandScape(*this);

	if (FAILED(pLandScape->Initialize(pArg)))
	{
		Safe_Release(pLandScape);
		MSG_BOX(" Land Scape is failed to Clone ");
		return nullptr;
	}

	return pLandScape;
}

void CLandScape::Free()
{
	Super::Free();
}