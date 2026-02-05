#include "Engine_pch.h"
#include "Shader.h"
#include "Engine_Utils.h"
#include "Constant_Buffer.h"
#include "Material.h"
#include "FxShaderVariant.h"
#include "FxEffectAsset.h"
#include "GameInstance.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CShader::CShader(const CShader& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pVariant(rhs.m_pVariant)
	, m_pBone_CBuffer(rhs.m_pBone_CBuffer)
	, m_pMaterial_CBuffer(rhs.m_pMaterial_CBuffer)
	, m_pTransform_CBuffer(rhs.m_pTransform_CBuffer)
	, m_pMI_CBuffer(rhs.m_pMI_CBuffer)
	, m_pKeyFrame_CBuffer(rhs.m_pKeyFrame_CBuffer)
	, m_pEffect_CBuffer(rhs.m_pEffect_CBuffer)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pVariant);
	Safe_AddRef(m_pBone_CBuffer);
	Safe_AddRef(m_pMaterial_CBuffer);
	Safe_AddRef(m_pTransform_CBuffer);
	Safe_AddRef(m_pKeyFrame_CBuffer);
	Safe_AddRef(m_pMI_CBuffer);
	Safe_AddRef(m_pEffect_CBuffer);
}

HRESULT CShader::Initialize_Prototype(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	SHADER_ORIGIN_DESC* pDesc = static_cast<SHADER_ORIGIN_DESC*>(pArg);
	m_pVariant = m_pGameInstance->GetOrCreate_Variant(pDesc->pShaderFilePath, pDesc->eLayout);
	if (m_pVariant == nullptr)
		return E_FAIL;
	Safe_AddRef(m_pVariant);

	Create_ConstantBuffer();
	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CShader::Apply()
{
	auto* pOwner = m_pVariant->m_pOwner;
	m_pDeviceContext->IASetInputLayout(m_pVariant->Get_InputLayout(m_iPass));
	pOwner->Get_Pass(m_iPass)->Apply(0, m_pDeviceContext);
}

//void CShader::Dispatch(_uint iX, _uint iY, _uint iZ)
//{
//	m_vecTechniques[0].vecPasses[m_iPass].pPass->Apply(0, m_pDeviceContext);
//	m_pDeviceContext->Dispatch(iX, iY, iZ);
//
//	ID3D11ShaderResourceView* null[1] = { 0 };
//	m_pDeviceContext->CSSetShaderResources(0, 1, null);
//
//	ID3D11UnorderedAccessView* nullUav[1] = { 0 };
//	m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUav, NULL);
//
//	m_pDeviceContext->CSSetShader(NULL, NULL, 0);
//}

ID3DX11EffectVariable* CShader::Get_Variable(string name)
{
	return m_pVariant->m_pOwner->Get_Variable(name);
}

ID3DX11EffectScalarVariable* CShader::Get_Scalar(string name)
{
	return m_pVariant->m_pOwner->Get_Scalar(name);
}

ID3DX11EffectVectorVariable* CShader::Get_Vector(string name)
{
	return m_pVariant->m_pOwner->Get_Vector(name);
}

ID3DX11EffectMatrixVariable* CShader::Get_Matrix(string name)
{
	return m_pVariant->m_pOwner->Get_Matrix(name);
}

ID3DX11EffectStringVariable* CShader::Get_String(string name)
{
	return  m_pVariant->m_pOwner->Get_String(name);
}

ID3DX11EffectShaderResourceVariable* CShader::Get_SRV(string name)
{
	return m_pVariant->m_pOwner->Get_SRV(name);
}

ID3DX11EffectRenderTargetViewVariable* CShader::Get_RTV(string name)
{
	return m_pVariant->m_pOwner->Get_RTV(name);
}

ID3DX11EffectDepthStencilViewVariable* CShader::Get_DSV(string name)
{
	return m_pVariant->m_pOwner->Get_DSV(name);
}

ID3DX11EffectUnorderedAccessViewVariable* CShader::Get_UAV(string name)
{
	return m_pVariant->m_pOwner->Get_UAV(name);
}

ID3DX11EffectConstantBuffer* CShader::Get_ConstantBuffer(string name)
{
	return m_pVariant->m_pOwner->Get_ConstantBuffer(name);
}

ID3DX11EffectShaderVariable* CShader::Get_Shader(string name)
{
	return m_pVariant->m_pOwner->Get_Shader(name);
}

ID3DX11EffectBlendVariable* CShader::Get_Blend(string name)
{
	return m_pVariant->m_pOwner->Get_Blend(name);
}

ID3DX11EffectDepthStencilVariable* CShader::Get_DepthStencil(string name)
{
	return m_pVariant->m_pOwner->Get_DepthStencil(name);
}

ID3DX11EffectRasterizerVariable* CShader::Get_Rasterizer(string name)
{
	return m_pVariant->m_pOwner->Get_Rasterizer(name);
}

ID3DX11EffectSamplerVariable* CShader::Get_Sampler(string name)
{
	return m_pVariant->m_pOwner->Get_Sampler(name);
}

void CShader::Bind_MaterialData(const SHADER_MATERIALDESC& desc)
{
	m_pMaterial_CBuffer->Copy_Data(desc);
}

void CShader::Bind_MaterialInstanceData(const SHADER_MI_DESC& desc)
{
	m_pMI_CBuffer->Copy_Data(desc);
}

void CShader::Bind_EffectData(const SHADER_EFFECT_DESC& desc)
{
	m_pEffect_CBuffer->Copy_Data(desc);
}

void CShader::Bind_GlobalMask(_uint iMask)
{
	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pG_TextureMask->SetInt((_int)iMask);
}

HRESULT CShader::Bind_DefaultTexture(ID3D11ShaderResourceView* pSRV)
{
	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	return BindingCache->pSRV_Textures->SetResource(pSRV);
}

HRESULT CShader::Bind_CubeTexture(ID3D11ShaderResourceView* pSRV)
{
	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	return BindingCache->pSRV_Cube->SetResource(pSRV);
}

HRESULT CShader::Bind_DefaultTextures(ID3D11ShaderResourceView** ppSRV, _uint iCount)
{
	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	return BindingCache->pSRV_Textures->SetResourceArray(ppSRV, 0, iCount);
}

HRESULT CShader::Bind_TransformData(const SHADER_TRANSFORMDESC& trnasformDesc)
{
	m_pTransform_CBuffer->Copy_Data(trnasformDesc);
	return S_OK;
}

HRESULT CShader::Bind_TransformData(const Matrix& matTransform)
{
	SHADER_TRANSFORMDESC desc = {};
	desc.matWorld = matTransform;
	m_pTransform_CBuffer->Copy_Data(desc);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_RT->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetDiffuseTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_RT_Diffuse->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetNormalTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_RT_Normal->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetShadeTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_RT_Shade->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetDepthTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_RT_Depth->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_DiffuseTexture(ID3D11ShaderResourceView* pDiffuse)
{
	if (pDiffuse == nullptr)
		return E_FAIL;

	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_Material->SetResourceArray(&pDiffuse, 0, 1);
	BindingCache->pG_MaterialMask->SetInt(1 << ENUM_TO_UINT(EMaterialTextureType::DIFFUSE));
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetSceneTexture(ID3D11ShaderResourceView* pScene)
{
	if (pScene == nullptr)
		return E_FAIL;

	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_RT_Scene->SetResource(pScene);
	return S_OK;
}

void CShader::Bind_MaterialTextures(ID3D11ShaderResourceView** ppSRV, _uint iCount)
{
	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_Material->SetResourceArray(ppSRV, 0, iCount);
}

void CShader::Bind_MaterialMask(_short iMask)
{
	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pG_MaterialMask->SetInt(iMask);
}

void CShader::Bind_TransformTexture(ID3D11ShaderResourceView* pSRV)
{
	auto* BindingCache = &m_pVariant->m_pOwner->m_tBindingCache;
	BindingCache->pSRV_Transform->SetResource(pSRV);
}

void CShader::Bind_KeyFrameData(const SHADER_KEYFRAMEDESC& keyframeDesc)
{
	m_pKeyFrame_CBuffer->Copy_Data(keyframeDesc);
}

HRESULT CShader::Bind_BoneData(const SHADER_BONEDESC& boneDesc)
{
	m_pBone_CBuffer->Copy_Data(boneDesc);
	return S_OK;
}
void CShader::Create_ConstantBuffer()
{
	auto *pCache = &m_pVariant->m_pOwner->m_tBindingCache;

	if (pCache->pCB_Bone)
	{
		m_pBone_CBuffer = CConstant_Buffer<SHADER_BONEDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->pCB_Bone->SetConstantBuffer(m_pBone_CBuffer->Get_Buffer());
	}
	if (pCache->pCB_Transform)
	{
		m_pTransform_CBuffer = CConstant_Buffer<SHADER_TRANSFORMDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->pCB_Transform->SetConstantBuffer(m_pTransform_CBuffer->Get_Buffer());
	}
	if(pCache->pCB_Material)
	{
		m_pMaterial_CBuffer = CConstant_Buffer<SHADER_MATERIALDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->pCB_Material->SetConstantBuffer(m_pMaterial_CBuffer->Get_Buffer());
	}
	if (pCache->pCB_MaterialInst)
	{
		m_pMI_CBuffer = CConstant_Buffer<SHADER_MI_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->pCB_MaterialInst->SetConstantBuffer(m_pMI_CBuffer->Get_Buffer());
	}
	if (pCache->pCB_Keyframe)
	{
		m_pKeyFrame_CBuffer = CConstant_Buffer<SHADER_KEYFRAMEDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->pCB_Keyframe->SetConstantBuffer(m_pKeyFrame_CBuffer->Get_Buffer());
	}
	if (pCache->pCB_Effect)
	{
		m_pEffect_CBuffer = CConstant_Buffer<SHADER_EFFECT_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->pCB_Effect->SetConstantBuffer(m_pEffect_CBuffer->Get_Buffer());
	}
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CShader* pInstance = new CShader(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("CShader::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CShader::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShader::Clear_ConstantBuffer()
{
	Safe_Release(m_pEffect_CBuffer);
	Safe_Release(m_pMI_CBuffer);
	Safe_Release(m_pKeyFrame_CBuffer);
	Safe_Release(m_pMaterial_CBuffer);
	Safe_Release(m_pTransform_CBuffer);
	Safe_Release(m_pBone_CBuffer);
}

void CShader::Free()
{
	Safe_Release(m_pVariant);
	Clear_ConstantBuffer();
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}