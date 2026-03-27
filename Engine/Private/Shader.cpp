#include "Engine_pch.h"
#include "Shader.h"
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
	, m_pObjectInfo_CBuffer(rhs.m_pObjectInfo_CBuffer)
	, m_pRGB_CBuffer(rhs.m_pRGB_CBuffer)
	, m_pRenderFx_CBuffer(rhs.m_pRenderFx_CBuffer)
	, m_pPlayerInfo_CBuffer{rhs.m_pPlayerInfo_CBuffer}
	, m_pEffect_Dissolve_CBuffer{rhs.m_pEffect_Dissolve_CBuffer }
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
	Safe_AddRef(m_pObjectInfo_CBuffer);
	Safe_AddRef(m_pRGB_CBuffer);
	Safe_AddRef(m_pRenderFx_CBuffer);
	Safe_AddRef(m_pPlayerInfo_CBuffer);
	Safe_AddRef(m_pEffect_Dissolve_CBuffer);
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
	auto* pEffectAsset = m_pVariant->Get_EffectAsset();
	m_pDeviceContext->IASetInputLayout(m_pVariant->Get_InputLayout(m_iPass));
	pEffectAsset->Get_Pass(m_iPass)->Apply(0, m_pDeviceContext);
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
	return m_pVariant->Get_EffectAsset()->Get_Variable(name);
}

ID3DX11EffectScalarVariable* CShader::Get_Scalar(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_Scalar(name);
}

ID3DX11EffectVectorVariable* CShader::Get_Vector(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_Vector(name);
}

ID3DX11EffectMatrixVariable* CShader::Get_Matrix(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_Matrix(name);
}

ID3DX11EffectStringVariable* CShader::Get_String(string name)
{
	return  m_pVariant->Get_EffectAsset()->Get_String(name);
}

ID3DX11EffectShaderResourceVariable* CShader::Get_SRV(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_SRV(name);
}

ID3DX11EffectRenderTargetViewVariable* CShader::Get_RTV(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_RTV(name);
}

ID3DX11EffectDepthStencilViewVariable* CShader::Get_DSV(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_DSV(name);
}

ID3DX11EffectUnorderedAccessViewVariable* CShader::Get_UAV(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_UAV(name);
}

ID3DX11EffectConstantBuffer* CShader::Get_ConstantBuffer(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_ConstantBuffer(name);
}

ID3DX11EffectShaderVariable* CShader::Get_Shader(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_Shader(name);
}

ID3DX11EffectBlendVariable* CShader::Get_Blend(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_Blend(name);
}

ID3DX11EffectDepthStencilVariable* CShader::Get_DepthStencil(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_DepthStencil(name);
}

ID3DX11EffectRasterizerVariable* CShader::Get_Rasterizer(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_Rasterizer(name);
}

ID3DX11EffectSamplerVariable* CShader::Get_Sampler(string name)
{
	return m_pVariant->Get_EffectAsset()->Get_Sampler(name);
}

HRESULT CShader::Bind_SRV(EFXSRV eSlot, ID3D11ShaderResourceView* pSRV)
{
	auto* BindingCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	return BindingCache->SRV[ENUM_TO_UINT(eSlot)]->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVArray(EFXSRV eSlot, ID3D11ShaderResourceView** ppSRV, _uint iCount)
{
	auto* BindingCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	return BindingCache->SRV[ENUM_TO_UINT(eSlot)]->SetResourceArray(ppSRV, 0, iCount);
}

HRESULT CShader::Bind_Scalar(EFXScalar eSlot, _uint iValue)
{
	auto* BindingCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	return BindingCache->Scalar[ENUM_TO_UINT(eSlot)]->SetInt((_int)iValue);
}

HRESULT CShader::Bind_Scalar(EFXScalar eSlot, _int iValue)
{
	auto* BindingCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	return BindingCache->Scalar[ENUM_TO_UINT(eSlot)]->SetInt(iValue);
}

HRESULT CShader::Bind_Scalar(EFXScalar eSlot, _float fValue)
{
	auto* BindingCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	return BindingCache->Scalar[ENUM_TO_UINT(eSlot)]->SetFloat(fValue);
}

HRESULT CShader::Bind_Scalar(EFXScalar eSlot, _bool bValue)
{
	auto* BindingCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	return BindingCache->Scalar[ENUM_TO_UINT(eSlot)]->SetBool(bValue);
}

HRESULT CShader::Bind_MaterialData(const SHADER_MATERIALDESC& desc)
{
	return m_pMaterial_CBuffer->Copy_Data(desc);
}

HRESULT CShader::Bind_MaterialInstanceData(const SHADER_MI_DESC& desc)
{
	return m_pMI_CBuffer->Copy_Data(desc);
}

HRESULT CShader::Bind_EffectData(const SHADER_EFFECT_DESC& desc)
{
	return m_pEffect_CBuffer->Copy_Data(desc);
}

HRESULT CShader::Bind_DissolveEffectData(const SHADER_DISSOLVE_EFFECT_DESC& Desc)
{
	return m_pEffect_Dissolve_CBuffer->Copy_Data(Desc);
}

HRESULT CShader::Bind_TransformData(const SHADER_TRANSFORMDESC& trnasformDesc)
{
	return m_pTransform_CBuffer->Copy_Data(trnasformDesc);
}

HRESULT CShader::Bind_TransformData(const Matrix& matTransform)
{
	SHADER_TRANSFORMDESC desc = {};
	desc.matWorld = matTransform;
	return m_pTransform_CBuffer->Copy_Data(desc);
}

HRESULT CShader::Bind_KeyFrameData(const SHADER_KEYFRAMEDESC& keyframeDesc)
{
	return m_pKeyFrame_CBuffer->Copy_Data(keyframeDesc);
}

HRESULT CShader::Bind_ObjectInfoData(const SHADER_OBJECTINFO_DESC& objectInfoDesc)
{
	return m_pObjectInfo_CBuffer->Copy_Data(objectInfoDesc);
}

HRESULT CShader::Bind_RGBColorData(const SHADER_RGBCOLOR_DESC& RGBColorDesc)
{
	return m_pRGB_CBuffer->Copy_Data(RGBColorDesc);
}

HRESULT CShader::Bind_RenderFxData(const SHADER_RENDER_FX_DESC& renderFxDesc)
{
	return m_pRenderFx_CBuffer->Copy_Data(renderFxDesc);
}

HRESULT CShader::Bind_PlayerInfo(const SHADER_PLAYER_INFO& playerInfo)
{
	return m_pPlayerInfo_CBuffer->Copy_Data(playerInfo);
}

HRESULT CShader::Set_ConstantBuffer(EFXCB eSlot, ID3D11Buffer* pBuffer)
{
	auto* BindingCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	return BindingCache->CB[ENUM_TO_UINT(eSlot)]->SetConstantBuffer(pBuffer);
}

HRESULT CShader::Bind_BoneData(const SHADER_BONEDESC& boneDesc)
{
	return m_pBone_CBuffer->Copy_Data(boneDesc);
}

void CShader::Create_ConstantBuffer()
{
	auto *pCache = m_pVariant->Get_EffectAsset()->Get_BindingCache();
	_uint iSlot = ENUM_TO_UINT(EFXCB::Bone);
	if (pCache->CB[iSlot])
	{
		m_pBone_CBuffer = CConstant_Buffer<SHADER_BONEDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pBone_CBuffer->Get_Buffer());
	}
	iSlot = ENUM_TO_UINT(EFXCB::Transform);
	if (pCache->CB[iSlot])
	{
		m_pTransform_CBuffer = CConstant_Buffer<SHADER_TRANSFORMDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pTransform_CBuffer->Get_Buffer());
	}
	iSlot = ENUM_TO_UINT(EFXCB::Material);
	if(pCache->CB[iSlot])
	{
		m_pMaterial_CBuffer = CConstant_Buffer<SHADER_MATERIALDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pMaterial_CBuffer->Get_Buffer());
	}
	iSlot = ENUM_TO_UINT(EFXCB::MaterialInst);
	if (pCache->CB[iSlot])
	{
		m_pMI_CBuffer = CConstant_Buffer<SHADER_MI_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pMI_CBuffer->Get_Buffer());
	}
	iSlot = ENUM_TO_UINT(EFXCB::Keyframe);
	if (pCache->CB[iSlot])
	{
		m_pKeyFrame_CBuffer = CConstant_Buffer<SHADER_KEYFRAMEDESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pKeyFrame_CBuffer->Get_Buffer());
	}
	iSlot = ENUM_TO_UINT(EFXCB::Effect);
	if (pCache->CB[iSlot])
	{
		m_pEffect_CBuffer = CConstant_Buffer<SHADER_EFFECT_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pEffect_CBuffer->Get_Buffer());
	}
	iSlot = ENUM_TO_UINT(EFXCB::ObjectInfo);
	if (pCache->CB[iSlot])
	{
		m_pObjectInfo_CBuffer = CConstant_Buffer<SHADER_OBJECTINFO_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pObjectInfo_CBuffer->Get_Buffer());
	}

	iSlot = ENUM_TO_UINT(EFXCB::RGBMapping);
	if (pCache->CB[iSlot])
	{
		m_pRGB_CBuffer = CConstant_Buffer<SHADER_RGBCOLOR_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pRGB_CBuffer->Get_Buffer());
	}

	iSlot = ENUM_TO_UINT(EFXCB::RenderFx);
	if (pCache->CB[iSlot])
	{
		m_pRenderFx_CBuffer = CConstant_Buffer<SHADER_RENDER_FX_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pRenderFx_CBuffer->Get_Buffer());
	}

	// playerposition
	iSlot = ENUM_TO_UINT(EFXCB::PlayerInfoBuffer);
	if (pCache->CB[iSlot])
	{
		m_pPlayerInfo_CBuffer = CConstant_Buffer<SHADER_PLAYER_INFO>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pPlayerInfo_CBuffer->Get_Buffer());
	}

	// Dissolve
	iSlot = ENUM_TO_UINT(EFXCB::DISSOLVEEFFECT);
	if (pCache->CB[iSlot])
	{
		m_pEffect_Dissolve_CBuffer = CConstant_Buffer<SHADER_DISSOLVE_EFFECT_DESC>::Create(m_pDevice, m_pDeviceContext);
		pCache->CB[iSlot]->SetConstantBuffer(m_pEffect_Dissolve_CBuffer->Get_Buffer());
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
	Safe_Release(m_pObjectInfo_CBuffer);
	Safe_Release(m_pRGB_CBuffer);
	Safe_Release(m_pRenderFx_CBuffer);
	Safe_Release(m_pPlayerInfo_CBuffer);
	Safe_Release(m_pEffect_Dissolve_CBuffer);
}

void CShader::Free()
{
	Safe_Release(m_pVariant);
	Clear_ConstantBuffer();
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}