#include "Engine_pch.h"
#include "Shader.h"
#include "Engine_Utils.h"
#include "Constant_Buffer.h"
#include "StructuredBuffer.h"
#include "Material.h"
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
	, m_bInit(rhs.m_bInit)
	, m_wstrPath(rhs.m_wstrPath)
	, m_pBlob(rhs.m_pBlob)
	, m_pEffect(rhs.m_pEffect)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_tEffectDesc(rhs.m_tEffectDesc)
	, m_vecTechniques(rhs.m_vecTechniques)
	, m_pGlobalMask_Effect(rhs.m_pGlobalMask_Effect)
	, m_pGlobalEffectBuffer(rhs.m_pGlobalEffectBuffer)
	, m_pGlobalLightEffectBuffer(rhs.m_pGlobalLightEffectBuffer)
	, m_pInvEffectBuffer(rhs.m_pInvEffectBuffer)
	, m_pMaterialSRV_Effect(rhs.m_pMaterialSRV_Effect)
	, m_pMaterialMask_Effect(rhs.m_pMaterialMask_Effect)
	, m_pBone_CBuffer(rhs.m_pBone_CBuffer)
	, m_pBoneEffectBuffer(rhs.m_pBoneEffectBuffer)
	, m_pMaterial_CBuffer(rhs.m_pMaterial_CBuffer)
	, m_pMaterialEffectBuffer(rhs.m_pMaterialEffectBuffer)
	, m_pTransform_CBuffer(rhs.m_pTransform_CBuffer)
	, m_pTransformEffectBuffer(rhs.m_pTransformEffectBuffer)
	, m_pMI_CBuffer(rhs.m_pMI_CBuffer)
	, m_pMI_EffectBuffer(rhs.m_pMI_EffectBuffer)
	, m_pKeyFrame_CBuffer(rhs.m_pKeyFrame_CBuffer)
	, m_pCubeTexture(rhs.m_pCubeTexture)
	, m_pKeyFrameEffectBuffer(rhs.m_pKeyFrameEffectBuffer)
	, m_pTransformTexture(rhs.m_pTransformTexture)
	, m_pRenderTargetTexture(rhs.m_pRenderTargetTexture)
	, m_pRenderTargetDiffuseTexture(rhs.m_pRenderTargetDiffuseTexture)
	, m_pRenderTargetNormalTexture(rhs.m_pRenderTargetNormalTexture)
	, m_pRenderTargetShadeTexture(rhs.m_pRenderTargetShadeTexture)
	, m_pRenderTargetDepthTexture(rhs.m_pRenderTargetDepthTexture)
	, m_prenderTargetSceneTexture(rhs.m_prenderTargetSceneTexture)
	, m_pDefaultTextures(rhs.m_pDefaultTextures)
	, m_pEffect_CBuffer(rhs.m_pEffect_CBuffer)
	, m_pEffectBuffer(rhs.m_pEffectBuffer)
	, m_pEffect_Mutable_Element_CBuffer(rhs.m_pEffect_Mutable_Element_CBuffer)
	, m_pEffect_MutableBuffer(rhs.m_pEffect_MutableBuffer)

{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pBlob);
	Safe_AddRef(m_pEffect);
	Safe_AddRef(m_pGlobalEffectBuffer);
	Safe_AddRef(m_pGlobalLightEffectBuffer);
	Safe_AddRef(m_pInvEffectBuffer);
	Safe_AddRef(m_pMaterialSRV_Effect);
	Safe_AddRef(m_pMaterialMask_Effect);
	Safe_AddRef(m_pBone_CBuffer);
	Safe_AddRef(m_pBoneEffectBuffer);
	Safe_AddRef(m_pMaterial_CBuffer);
	Safe_AddRef(m_pMaterialEffectBuffer);
	Safe_AddRef(m_pTransform_CBuffer);
	Safe_AddRef(m_pTransformEffectBuffer);
	Safe_AddRef(m_pKeyFrame_CBuffer);
	Safe_AddRef(m_pKeyFrameEffectBuffer);
	Safe_AddRef(m_pMI_CBuffer);
	Safe_AddRef(m_pCubeTexture);
	Safe_AddRef(m_pMI_EffectBuffer);
	Safe_AddRef(m_pTransformTexture);
	Safe_AddRef(m_pRenderTargetTexture);
	Safe_AddRef(m_pRenderTargetDiffuseTexture);
	Safe_AddRef(m_pRenderTargetNormalTexture);
	Safe_AddRef(m_pRenderTargetShadeTexture);
	Safe_AddRef(m_pRenderTargetDepthTexture);
	Safe_AddRef(m_prenderTargetSceneTexture);
	Safe_AddRef(m_pEffect_CBuffer);
	Safe_AddRef(m_pEffectBuffer);
	Safe_AddRef(m_pEffect_Mutable_Element_CBuffer);
	Safe_AddRef(m_pEffect_MutableBuffer);
	Safe_AddRef(m_pDefaultTextures);
	Safe_AddRef(m_pGlobalMask_Effect);

	for (auto& Technique : m_vecTechniques)
	{
		for (auto& Pass : Technique.vecPasses)
		{
			Safe_AddRef(Pass.pInputLayout);
		}
	}
}

HRESULT CShader::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	SHADER_ORIGIN_DESC* pDesc = static_cast<SHADER_ORIGIN_DESC*>(pArg);
	m_wstrPath = pDesc->pShaderFilePath;
	if (FAILED(Load_Shader(pDesc->pElements, pDesc->iNumElements)))
		return E_FAIL;

	Create_ConstantBuffer();
	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Create_StructuredBuffer();
	return S_OK;
}

void CShader::Apply()
{
	m_pDeviceContext->IASetInputLayout(m_vecTechniques[0].vecPasses[m_iPass].pInputLayout);
	m_vecTechniques[0].vecPasses[m_iPass].pPass->Apply(0, m_pDeviceContext);
}

void CShader::Dispatch(_uint iX, _uint iY, _uint iZ)
{
	// 1. Hazard 방지용 해제 (기존 코드 유지)
	ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };
	m_pDeviceContext->VSSetShaderResources(0, 4, nullSRVs);

	// [중요] 내 인스턴스의 고유 버퍼를 셰이더 변수에 다시 꽂음
	// 이 작업이 있어야 다른 객체가 덮어쓴 변수 값을 내 것으로 되찾아옴
	if (m_pEffect_Result_UAV)
		m_pEffect_Result_UAV->SetUnorderedAccessView(m_pEffect_Result_SBuffer->Get_UAV());

	if (m_pEffect_Immutable_Element_SRV)
		m_pEffect_Immutable_Element_SRV->SetResource(m_pEffect_Immutable_Element_CBuffer->Get_SRV());

	// 상태 적용 및 실행
	m_vecTechniques[0].vecPasses[m_iPass].pPass->Apply(0, m_pDeviceContext);
	m_pDeviceContext->Dispatch(iX, iY, iZ);

	// 즉시 정리 (다음 렌더링 시 SRV로 바로 쓸 수 있도록)
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

	m_pDeviceContext->CSSetShader(NULL, NULL, 0);
}

ID3DX11EffectVariable* CShader::Get_Variable(string name)
{
	return m_pEffect->GetVariableByName(name.c_str());
}

ID3DX11EffectScalarVariable* CShader::Get_Scalar(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsScalar();
}

ID3DX11EffectVectorVariable* CShader::Get_Vector(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsVector();
}

ID3DX11EffectMatrixVariable* CShader::Get_Matrix(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsMatrix();
}

ID3DX11EffectStringVariable* CShader::Get_String(string name)
{
	return  m_pEffect->GetVariableByName(name.c_str())->AsString();
}

ID3DX11EffectShaderResourceVariable* CShader::Get_SRV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsShaderResource();
}

ID3DX11EffectRenderTargetViewVariable* CShader::Get_RTV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsRenderTargetView();
}

ID3DX11EffectDepthStencilViewVariable* CShader::Get_DSV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsDepthStencilView();
}

ID3DX11EffectUnorderedAccessViewVariable* CShader::Get_UAV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsUnorderedAccessView();
}

ID3DX11EffectConstantBuffer* CShader::Get_ConstantBuffer(string name)
{
	return m_pEffect->GetConstantBufferByName(name.c_str());
}

ID3DX11EffectShaderVariable* CShader::Get_Shader(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsShader();
}

ID3DX11EffectBlendVariable* CShader::Get_Blend(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsBlend();
}

ID3DX11EffectDepthStencilVariable* CShader::Get_DepthStencil(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsDepthStencil();
}

ID3DX11EffectRasterizerVariable* CShader::Get_Rasterizer(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsRasterizer();
}

ID3DX11EffectSamplerVariable* CShader::Get_Sampler(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsSampler();
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

void CShader::Bind_Compute_EffectData(const EFFECT_PARTICLE_MU_ELEMENT& desc)
{
	m_pEffect_Mutable_Element_CBuffer->Copy_Data(desc);
}

void CShader::Bind_Compute_EffectData(const EFFECT_PARTICLE_IMMU_ELEMENT* desc, _uint count)
{
	m_pEffect_Immutable_Element_CBuffer->Copy_Data(desc, count);
}

void CShader::Bind_Compute_EffectSRV()
{
	if (ID3DX11EffectShaderResourceVariable* pResultSRV = Get_SRV("INSTANCE_OUTPUT"))
	{
		pResultSRV->SetResource(m_pEffect_Result_SBuffer->Get_SRV());
	}
}

void CShader::Bind_GlobalMask(_uint iMask)
{
	m_pGlobalMask_Effect->SetInt((_int)iMask);
}

HRESULT CShader::Bind_DefaultTexture(ID3D11ShaderResourceView* pSRV)
{
	return m_pDefaultTextures->SetResource(pSRV);
}

HRESULT CShader::Bind_CubeTexture(ID3D11ShaderResourceView* pSRV)
{
	return m_pCubeTexture->SetResource(pSRV);
}

HRESULT CShader::Bind_DefaultTextures(ID3D11ShaderResourceView** ppSRV, _uint iCount)
{
	return m_pDefaultTextures->SetResourceArray(ppSRV, 0, iCount);
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

	m_pRenderTargetTexture->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetDiffuseTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	m_pRenderTargetDiffuseTexture->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetNormalTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	m_pRenderTargetNormalTexture->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetShadeTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	m_pRenderTargetShadeTexture->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetDepthTexture(ID3D11ShaderResourceView* pTexture)
{
	if (pTexture == nullptr)
		return E_FAIL;

	m_pRenderTargetDepthTexture->SetResource(pTexture);
	return S_OK;
}

HRESULT CShader::Bind_DiffuseTexture(ID3D11ShaderResourceView* pDiffuse)
{
	if (!pDiffuse)
		return E_FAIL;

	m_pMaterialSRV_Effect->SetResourceArray(&pDiffuse, 0, 1);
	m_pMaterialMask_Effect->SetInt(1 << ENUM_TO_UINT(EMaterialTextureType::DIFFUSE));
	return S_OK;
}

HRESULT CShader::Bind_RenderTargetSceneTexture(ID3D11ShaderResourceView* pScene)
{
	if (!pScene)
		return E_FAIL;

	m_prenderTargetSceneTexture->SetResource(pScene);
	return S_OK;
}

void CShader::Bind_MaterialTextures(ID3D11ShaderResourceView** ppSRV, _uint iCount)
{
	m_pMaterialSRV_Effect->SetResourceArray(ppSRV, 0, iCount);
}

void CShader::Bind_MaterialMask(_short iMask)
{
	m_pMaterialMask_Effect->SetInt(iMask);
}

void CShader::Bind_TransformTexture(ID3D11ShaderResourceView* pSRV)
{
	m_pTransformTexture->SetResource(pSRV);
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

void CShader::Resize_Compute_EffectData(_uint count)
{
	m_pEffect_Immutable_Element_CBuffer->Resize(count);
}

HRESULT CShader::Load_Shader(const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements)
{
	// Create Effect
	{
		_int flag = {};
#ifdef _DEBUG
		flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		flag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif   
		ID3DBlob* pBlob = { nullptr };
 		if (FAILED(::D3DX11CompileEffectFromFile(Get_Path().c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, flag, 0, m_pDevice, &m_pEffect, &pBlob)))
		{
			if (pBlob)
			{
				LPVOID pSrc = pBlob->GetBufferPointer();
				wstring wstrhi(static_cast<const _tchar*>(pSrc));
 				_uint i = 0;
			}
			Safe_Release(pBlob);
			return E_FAIL;
		}

		Safe_Release(pBlob);
	}

	// Load Effect
	{
		m_pEffect->GetDesc(&m_tEffectDesc);
		for (_uint t = 0; t < m_tEffectDesc.Techniques; t++)
		{
			TECHNIQUE technique;
			technique.pTechnique = m_pEffect->GetTechniqueByIndex(t);
			technique.pTechnique->GetDesc(&technique.tDesc);
			technique.wstrName = Engine_Utils::ToWString(technique.tDesc.Name);

			for (UINT p = 0; p < technique.tDesc.Passes; p++)
			{
				PASS pass;
				pass.pPass = technique.pTechnique->GetPassByIndex(p);
				pass.pPass->GetDesc(&pass.tDesc);
				pass.wstrName = Engine_Utils::ToWString(pass.tDesc.Name);
				pass.pPass->GetVertexShaderDesc(&pass.tVertexShaderDesc);
				pass.tVertexShaderDesc.pShaderVariable->GetShaderDesc(pass.tVertexShaderDesc.ShaderIndex, &pass.tEffectVsDesc);

				// 버텍스 셰이더 계신가요
				if (pass.tVertexShaderDesc.pShaderVariable->IsValid())
				{
					pass.tVertexShaderDesc.pShaderVariable->GetShaderDesc(pass.tVertexShaderDesc.ShaderIndex, &pass.tEffectVsDesc);

					if (pass.tEffectVsDesc.pBytecode != nullptr)
					{
						if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements,
							pass.tDesc.pIAInputSignature, pass.tDesc.IAInputSignatureSize, &pass.pInputLayout)))
						{
							return E_FAIL;
						}
					}
					else
					{
						// Compute Shader 전용 패스 (VS 코드가 없음)
						// 이펙트 객체가 어엇 너 vertex shader 쓰는구나!!!!!! 하면서 야루~ 하면서 들어올 떄가 있음.
						pass.pInputLayout = nullptr;
					}
				}
				// 안계시네요..
				else
				{
					pass.pInputLayout = nullptr;
				}
				 
				technique.vecPasses.push_back(pass);
			}

			m_vecTechniques.push_back(technique);
		}

		for (UINT i = 0; i < m_tEffectDesc.ConstantBuffers; i++)
		{
			ID3DX11EffectConstantBuffer* iBuffer;
			iBuffer = m_pEffect->GetConstantBufferByIndex(i);

			D3DX11_EFFECT_VARIABLE_DESC vDesc;
			iBuffer->GetDesc(&vDesc);
		}

		for (UINT i = 0; i < m_tEffectDesc.GlobalVariables; i++)
		{
			ID3DX11EffectVariable* effectVariable;
			effectVariable = m_pEffect->GetVariableByIndex(i);

			D3DX11_EFFECT_VARIABLE_DESC vDesc;
			effectVariable->GetDesc(&vDesc);
		}
	}

	return S_OK;
}

void CShader::Create_ConstantBuffer()
{
	// Bone
	{
		if (m_pBoneEffectBuffer = Get_ConstantBuffer("BoneBuffer"))
		{
			m_pBone_CBuffer = CConstant_Buffer<SHADER_BONEDESC>::Create(m_pDevice, m_pDeviceContext);
			m_pBoneEffectBuffer->SetConstantBuffer(m_pBone_CBuffer->Get_Buffer());
		}
	}

	// Transform
	{
		if (m_pTransformEffectBuffer = Get_ConstantBuffer("TransformBuffer"))
		{
			m_pTransform_CBuffer = CConstant_Buffer<SHADER_TRANSFORMDESC>::Create(m_pDevice, m_pDeviceContext);
			m_pTransformEffectBuffer->SetConstantBuffer(m_pTransform_CBuffer->Get_Buffer());
		}
	}

	// Material
	{
		if (m_pMaterialEffectBuffer = Get_ConstantBuffer("MaterialBuffer"))
		{
			m_pMaterial_CBuffer = CConstant_Buffer<SHADER_MATERIALDESC>::Create(m_pDevice, m_pDeviceContext);
			m_pMaterialEffectBuffer->SetConstantBuffer(m_pMaterial_CBuffer->Get_Buffer());
		}
	}

	// MaterialInstance
	{
		if (m_pMI_EffectBuffer = Get_ConstantBuffer("MaterialInstanceBuffer"))
		{
			m_pMI_CBuffer = CConstant_Buffer<SHADER_MI_DESC>::Create(m_pDevice, m_pDeviceContext);
			m_pMI_EffectBuffer->SetConstantBuffer(m_pMI_CBuffer->Get_Buffer());
		}
	}

	// Global
	{
		if (m_pGlobalEffectBuffer = Get_ConstantBuffer("GlobalBuffer"))
		{
			m_pGlobalEffectBuffer->SetConstantBuffer(m_pGameInstance->Get_Global_ConstantBuffer());
		}
		if (m_pGlobalLightEffectBuffer = Get_ConstantBuffer("LightBuffer"))
		{
			m_pGlobalLightEffectBuffer->SetConstantBuffer(m_pGameInstance->Get_Light_ConstantBuffer());
		}
		if (m_pInvEffectBuffer = Get_ConstantBuffer("InvBuffer"))
		{
			m_pInvEffectBuffer->SetConstantBuffer(m_pGameInstance->Get_Inv_ConstantBuffer());
		}

		m_pGlobalMask_Effect = Get_Scalar("g_iGlobalMask");
	}

	// Keyframe
	{
		if (m_pKeyFrameEffectBuffer = Get_ConstantBuffer("KeyframeBuffer"))
		{
			m_pKeyFrame_CBuffer = CConstant_Buffer<SHADER_KEYFRAMEDESC>::Create(m_pDevice, m_pDeviceContext);
			m_pKeyFrameEffectBuffer->SetConstantBuffer(m_pKeyFrame_CBuffer->Get_Buffer());
		}
	}

	// EffectDesc
	{
		if (m_pEffectBuffer = Get_ConstantBuffer("ConstantBuffer_Effect"))
		{
			m_pEffect_CBuffer = CConstant_Buffer<SHADER_EFFECT_DESC>::Create(m_pDevice, m_pDeviceContext);
			m_pEffectBuffer->SetConstantBuffer(m_pEffect_CBuffer->Get_Buffer());
		}
	}

	// Compute_EffectDesc
	{
		if (m_pEffect_MutableBuffer = Get_ConstantBuffer("MU_ParticleUpdate"))
		{
			m_pEffect_Mutable_Element_CBuffer = CConstant_Buffer<EFFECT_PARTICLE_MU_ELEMENT>::Create(m_pDevice, m_pDeviceContext);
			m_pEffect_MutableBuffer->SetConstantBuffer(m_pEffect_Mutable_Element_CBuffer->Get_Buffer());
		}
	}

	// Texture
	{
		m_pTransformTexture = Get_SRV("g_TransformMap");
		m_pDefaultTextures = Get_SRV("g_DefaultTextures");
		m_pMaterialSRV_Effect = Get_SRV("g_MaterialTextures");
		m_pCubeTexture = Get_SRV("g_TextureCube");
		m_pMaterialMask_Effect = Get_Scalar("g_iMaterialMask");
	}

	// RenderTargetTexture
	{
		m_pRenderTargetTexture = Get_SRV("g_RenderTargetTexture");
		m_pRenderTargetDiffuseTexture = Get_SRV("g_RenderTargetDiffuseTexture");
		m_pRenderTargetNormalTexture = Get_SRV("g_RenderTargetNormalTexture");
		m_pRenderTargetShadeTexture = Get_SRV("g_RenderTargetShadeTexture");
		m_pRenderTargetDepthTexture = Get_SRV("g_RenderTargetDepthTexture");
		m_prenderTargetSceneTexture = Get_SRV("g_RenderTargetSceneTexture");
	}
}

void CShader::Create_StructuredBuffer()
{
	// Compute_Effect_SRV
	{
		if (m_pEffect_Immutable_Element_SRV = Get_SRV("IMMU_EFFECT_PARTICLE"))
		{
			m_pEffect_Immutable_Element_CBuffer = StructuredBuffer<EFFECT_PARTICLE_IMMU_ELEMENT>::Create(m_pDevice, m_pDeviceContext, 1000);
			m_pEffect_Immutable_Element_SRV->SetResource(m_pEffect_Immutable_Element_CBuffer->Get_SRV());
		}
	}

	// Compute_Effect_UAV
	{
		if (m_pEffect_Result_UAV = Get_UAV("INSTANCE_OUTPUT"))
		{
			m_pEffect_Result_SBuffer = StructuredBuffer<EFFECT_INSTANCE>::Create(m_pDevice, m_pDeviceContext, 1000);
			m_pEffect_Result_UAV->SetUnorderedAccessView(m_pEffect_Result_SBuffer->Get_UAV());
			Get_SRV("INSTANCE_RESULT_SRV")->SetResource(m_pEffect_Result_SBuffer->Get_SRV());
		}
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
	Safe_Release(m_pGlobalMask_Effect);
	Safe_Release(m_pDefaultTextures);
	Safe_Release(m_pEffect_CBuffer);
	Safe_Release(m_pEffectBuffer);
	Safe_Release(m_pRenderTargetDiffuseTexture);
	Safe_Release(m_pRenderTargetNormalTexture);
	Safe_Release(m_pRenderTargetShadeTexture);
	Safe_Release(m_pRenderTargetDepthTexture);
	Safe_Release(m_prenderTargetSceneTexture);
	Safe_Release(m_pRenderTargetTexture);
	Safe_Release(m_pMaterialSRV_Effect);
	Safe_Release(m_pMaterialMask_Effect);
	Safe_Release(m_pMI_CBuffer);
	Safe_Release(m_pMI_EffectBuffer);
	Safe_Release(m_pKeyFrame_CBuffer);
	Safe_Release(m_pKeyFrameEffectBuffer);
	Safe_Release(m_pMaterial_CBuffer);
	Safe_Release(m_pCubeTexture);
	Safe_Release(m_pMaterialEffectBuffer);
	Safe_Release(m_pTransformTexture);
	Safe_Release(m_pInvEffectBuffer);
	Safe_Release(m_pGlobalEffectBuffer);
	Safe_Release(m_pGlobalLightEffectBuffer);
	Safe_Release(m_pTransform_CBuffer);
	Safe_Release(m_pTransformEffectBuffer);
	Safe_Release(m_pBone_CBuffer);
	Safe_Release(m_pBoneEffectBuffer);
	Safe_Release(m_pEffect_Mutable_Element_CBuffer);
	Safe_Release(m_pEffect_MutableBuffer);
}

void CShader::Clear_StructuredBuffer()
{
	Safe_Release(m_pEffect_Immutable_Element_CBuffer);
	Safe_Release(m_pEffect_Immutable_Element_SRV);
	Safe_Release(m_pEffect_Result_SBuffer);
	Safe_Release(m_pEffect_Result_UAV);
}

void CShader::Free()
{
	for (auto& Technique : m_vecTechniques)
	{
		for (auto& Pass : Technique.vecPasses)
		{
			Safe_Release(Pass.pInputLayout);
		}
	}
	Clear_ConstantBuffer();
	Clear_StructuredBuffer();
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pBlob);
	Safe_Release(m_pEffect);
	Super::Free();
}