#include "Engine_pch.h"
#include "FxEffectAsset.h"
#include "Engine_Utils.h"

CFxEffectAsset::CFxEffectAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CFxEffectAsset::Initialize()
{
	return S_OK;
}

HRESULT CFxEffectAsset::Load_EffectFromFile(const std::wstring& path)
{
	if (path.empty())
		return E_FAIL;

	m_wstrPath = path;

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
		m_vecTechniques.reserve(m_tEffectDesc.Techniques);
		for (_uint t = 0; t < m_tEffectDesc.Techniques; t++)
		{
			TECHNIQUE technique;
			technique.pTechnique = m_pEffect->GetTechniqueByIndex(t);
			technique.pTechnique->GetDesc(&technique.tDesc);
			technique.wstrName = Engine_Utils::ToWString(technique.tDesc.Name);

			technique.vecPasses.reserve(technique.tDesc.Passes);
			for (UINT p = 0; p < technique.tDesc.Passes; p++)
			{
				PASS pass;
				pass.pPass = technique.pTechnique->GetPassByIndex(p);
				pass.pPass->GetDesc(&pass.tDesc);
				pass.wstrName = Engine_Utils::ToWString(pass.tDesc.Name);
				pass.pPass->GetVertexShaderDesc(&pass.tVertexShaderDesc);
				pass.tVertexShaderDesc.pShaderVariable->GetShaderDesc(pass.tVertexShaderDesc.ShaderIndex, &pass.tEffectVsDesc);
				// >> CreateInputLayout ªË¡¶ << //
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

	Binding_Cache();
	return S_OK;
}

ID3DX11EffectPass* CFxEffectAsset::Get_Pass(_uint iPass, _uint iTechnique)
{
	return m_vecTechniques[iTechnique].vecPasses[iPass].pPass;
}

ID3DX11EffectVariable* CFxEffectAsset::Get_Variable(string name)
{
	return m_pEffect->GetVariableByName(name.c_str());
}

ID3DX11EffectScalarVariable* CFxEffectAsset::Get_Scalar(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsScalar();
}

ID3DX11EffectVectorVariable* CFxEffectAsset::Get_Vector(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsVector();
}

ID3DX11EffectMatrixVariable* CFxEffectAsset::Get_Matrix(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsMatrix();
}

ID3DX11EffectStringVariable* CFxEffectAsset::Get_String(string name)
{
	return  m_pEffect->GetVariableByName(name.c_str())->AsString();
}

ID3DX11EffectShaderResourceVariable* CFxEffectAsset::Get_SRV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsShaderResource();
}

ID3DX11EffectRenderTargetViewVariable* CFxEffectAsset::Get_RTV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsRenderTargetView();
}

ID3DX11EffectDepthStencilViewVariable* CFxEffectAsset::Get_DSV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsDepthStencilView();
}

ID3DX11EffectUnorderedAccessViewVariable* CFxEffectAsset::Get_UAV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsUnorderedAccessView();
}

ID3DX11EffectConstantBuffer* CFxEffectAsset::Get_ConstantBuffer(string name)
{
	return m_pEffect->GetConstantBufferByName(name.c_str());
}

ID3DX11EffectShaderVariable* CFxEffectAsset::Get_Shader(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsShader();
}

ID3DX11EffectBlendVariable* CFxEffectAsset::Get_Blend(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsBlend();
}

ID3DX11EffectDepthStencilVariable* CFxEffectAsset::Get_DepthStencil(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsDepthStencil();
}

ID3DX11EffectRasterizerVariable* CFxEffectAsset::Get_Rasterizer(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsRasterizer();
}

ID3DX11EffectSamplerVariable* CFxEffectAsset::Get_Sampler(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsSampler();
}

void CFxEffectAsset::Binding_Cache()
{
	m_tBindingCache.pCB_Global = m_pEffect->GetConstantBufferByName("GlobalBuffer");
	m_tBindingCache.pCB_Inv = m_pEffect->GetConstantBufferByName("InvBuffer");
	m_tBindingCache.pCB_Transform = m_pEffect->GetConstantBufferByName("TransformBuffer");

	m_tBindingCache.pCB_Light = m_pEffect->GetConstantBufferByName("LightBuffer");
	m_tBindingCache.pCB_Material = m_pEffect->GetConstantBufferByName("MaterialBuffer");
	m_tBindingCache.pCB_MaterialInst = m_pEffect->GetConstantBufferByName("MaterialInstanceBuffer");

	m_tBindingCache.pCB_Keyframe = m_pEffect->GetConstantBufferByName("KeyframeBuffer");
	m_tBindingCache.pCB_Bone = m_pEffect->GetConstantBufferByName("BoneBuffer");	
	m_tBindingCache.pCB_Effect = m_pEffect->GetConstantBufferByName("ConstantBuffer_Effect");

	m_tBindingCache.pG_MaterialMask = m_pEffect->GetVariableByName("g_iMaterialMask")->AsScalar();
	m_tBindingCache.pG_TextureMask = m_pEffect->GetVariableByName("g_iGlobalMask")->AsScalar();

	m_tBindingCache.pSRV_RT = m_pEffect->GetVariableByName("g_RenderTargetTexture")->AsShaderResource();
	m_tBindingCache.pSRV_RT_Diffuse = m_pEffect->GetVariableByName("g_RenderTargetDiffuseTexture")->AsShaderResource();
	m_tBindingCache.pSRV_RT_Normal = m_pEffect->GetVariableByName("g_RenderTargetNormalTexture")->AsShaderResource();
	m_tBindingCache.pSRV_RT_Shade = m_pEffect->GetVariableByName("g_RenderTargetShadeTexture")->AsShaderResource();
	m_tBindingCache.pSRV_RT_Depth = m_pEffect->GetVariableByName("g_RenderTargetDepthTexture")->AsShaderResource();
	m_tBindingCache.pSRV_RT_Scene = m_pEffect->GetVariableByName("g_RenderTargetSceneTexture")->AsShaderResource();
		
	m_tBindingCache.pSRV_Transform = m_pEffect->GetVariableByName("g_TransformMap")->AsShaderResource();
	m_tBindingCache.pSRV_Material = m_pEffect->GetVariableByName("g_MaterialTextures")->AsShaderResource();

	m_tBindingCache.pSRV_Textures = m_pEffect->GetVariableByName("g_DefaultTextures")->AsShaderResource();
	m_tBindingCache.pSRV_Cube = m_pEffect->GetVariableByName("g_TextureCube")->AsShaderResource();	
}

void CFxEffectAsset::Clear_Cache()
{
	Safe_Release(m_tBindingCache.pCB_Global);
	Safe_Release(m_tBindingCache.pCB_Inv);
	Safe_Release(m_tBindingCache.pCB_Transform);
	Safe_Release(m_tBindingCache.pCB_Light);
	Safe_Release(m_tBindingCache.pCB_Material);
	Safe_Release(m_tBindingCache.pCB_MaterialInst);
	Safe_Release(m_tBindingCache.pCB_Keyframe);
	Safe_Release(m_tBindingCache.pCB_Bone);
	Safe_Release(m_tBindingCache.pCB_Effect);
	Safe_Release(m_tBindingCache.pG_MaterialMask);
	Safe_Release(m_tBindingCache.pG_TextureMask);
	Safe_Release(m_tBindingCache.pSRV_RT);
	Safe_Release(m_tBindingCache.pSRV_RT_Diffuse);
	Safe_Release(m_tBindingCache.pSRV_RT_Normal);
	Safe_Release(m_tBindingCache.pSRV_RT_Shade);
	Safe_Release(m_tBindingCache.pSRV_RT_Depth);
	Safe_Release(m_tBindingCache.pSRV_RT_Scene);
	Safe_Release(m_tBindingCache.pSRV_Transform);
	Safe_Release(m_tBindingCache.pSRV_Material);
	Safe_Release(m_tBindingCache.pSRV_Textures);
	Safe_Release(m_tBindingCache.pSRV_Cube);
}

CFxEffectAsset* CFxEffectAsset::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFxEffectAsset* pInstance = new CFxEffectAsset(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CFxEffectAsset::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFxEffectAsset::Free()
{
	Clear_Cache();
	Safe_Release(m_pEffect);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}