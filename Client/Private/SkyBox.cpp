#include "pch.h"
#include "SkyBox.h"
#include "Texture.h"
#include "Model.h"
#include "CameraMan.h"
#include "Shader.h"
#include "GameInstance.h"

CSkyBox::CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext), m_fAccDT{}
{ 
}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: CGameObject(rhs)
	, m_fAccDT{rhs.m_fAccDT }
{
}

HRESULT CSkyBox::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CSkyBox::SKYBOX_DESC* pDesc =static_cast<CSkyBox::SKYBOX_DESC*>(pArg);


	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::Ready_Component(SKYBOX_DESC* pDesc)
{
	if (pDesc == nullptr)
		return E_FAIL;

	const wstring& wstrTexutreName = pDesc->wstrTextureTag;
	const wstring& wstrModelName = pDesc->wstrModelTag;
	if (wstrTexutreName.empty() || wstrModelName.empty())
		return E_FAIL;

	/* 기본 모델 생성 */

	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	wstring wstrModelDefaultTag = L"Prototype_Component_Model_";
	if(FAILED(Add_Component<CModel>(ENUM_TO_UINT(ELevelType::STATIC) , wstrModelDefaultTag + wstrModelName, &tModelCopyDesc)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr)))
		return E_FAIL;

	
	CTexture::TEXTURE_COMPONENT_ORIGIN_DESC tTextureDesc{};
	tTextureDesc.wstrTexturePath = wstrTexutreName;
	tTextureDesc.iTextureCount = 1;
	/* Texture Origin Component 추가 */
	m_pGameInstance->Add_Prototype( pDesc->iLevelIndex ,L"Prototype_Component_Texture_" + wstrTexutreName , CTexture::Create(&tTextureDesc) );

	/* Texture 바로추가 */
	if (FAILED(Add_Component<CTexture>(pDesc->iLevelIndex, L"Prototype_Component_Texture_" + wstrTexutreName, nullptr)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkyBox::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;


	return S_OK;
}

void CSkyBox::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CSkyBox::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);


	m_fAccDT += fTimeDelta;
	if (m_fAccDT > 1000.f)
		m_fAccDT = 0.f;
}

void CSkyBox::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

}

void CSkyBox::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	m_pGameInstance->GetInstance()->Push_RenderObject(RENDER_CATEGORY::ENVIRONMENT,this);
}


HRESULT CSkyBox::Render()
{
	CTransform* pTs			= Get_Component<CTransform>();	if (pTs == nullptr) return E_FAIL;
	CModel*		pModel		= Get_Component<CModel>();	if (pTs == nullptr) return E_FAIL;
	CShader*	pShader		= Get_Component<CShader>();	if (pTs == nullptr) return E_FAIL;
	CTexture*	pTexture	= Get_Component<CTexture>(); if (pTexture == nullptr) return E_FAIL;

	if (FAILED(pShader->Get_Scalar("fEvnAccDT")->SetFloat(m_fAccDT)))
		return E_FAIL;

	CCameraMan* pCam = m_pGameInstance->Get_MainCamera();
	if (pCam == nullptr) return E_FAIL;
	pTs->Set_Info (TRANSFORM_INFO_STATE::POS, pCam->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS));

	/* WorldMatrix 바인딩 */
	pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::SkyBox));
	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(pTs->Get_WorldMatrix());
	pTexture->Bind_ShaderResource(pShader);

	_uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkyBox* pSkyBox = new CSkyBox(pDevice, pContext);

	if (FAILED(pSkyBox->Initialize_Prototype()))
	{
		Safe_Release(pSkyBox);
		MSG_BOX(" Sky Box is Failed to Create ");
		return nullptr;
	}

	return pSkyBox;
}

CGameObject* CSkyBox::Clone(void* pArg)
{
	CSkyBox* pSkyBox = new CSkyBox(*this);

	if (FAILED(pSkyBox->Initialize(pArg)))
	{
		Safe_Release(pSkyBox);
		MSG_BOX(" Sky Box is Failed to Clone ");
		return nullptr;
	}

	return pSkyBox;
}

void CSkyBox::Free()
{
	Super::Free();
}
