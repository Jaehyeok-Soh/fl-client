#include "Engine_pch.h"
#include "ComputeShader.h"
#include "FxEffectAsset.h"
#include "Engine_Utils.h"

// has class
#include "StructuredBuffer.h"
#include "Constant_Buffer.h"
#include "GameInstance.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CComputeShader::CComputeShader(const CComputeShader& rhs)
	: Super(rhs)
	, m_pOwner(rhs.m_pOwner)
	, m_pComputeShader(rhs.m_pComputeShader)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pEffect_Mutable_Element_CBuffer(rhs.m_pEffect_Mutable_Element_CBuffer)
	, m_pEffect_MutableBuffer(rhs.m_pEffect_MutableBuffer)

{
	Safe_AddRef(m_pOwner);
	Safe_AddRef(m_pComputeShader);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pEffect_Mutable_Element_CBuffer);
	Safe_AddRef(m_pEffect_MutableBuffer);
}

HRESULT CComputeShader::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	COMSHADER_ORIGIN_DESC* pDesc = static_cast<COMSHADER_ORIGIN_DESC*>(pArg);

	if (FAILED(Ready_ComputeShader(pDesc)))
		return E_FAIL;
		
	Create_ConstantBuffer();
	return S_OK;
}

HRESULT CComputeShader::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Create_StructBuffer(pArg);
	return S_OK;
}

void CComputeShader::Dispatch(_uint iX, _uint iY, _uint iZ)
{
	// GPU <-> GPU 통신은 한쪽이 연결을 끊어줘야 다른쪽 연결도 성공한다.
	// 마치 삐삐랄까 무전기?
	// SRV 썼으면 닫아주고 UAV 세팅하고 .. 반복

	ID3D11ShaderResourceView* nullSRVs[4] = { nullptr }; 
	m_pDeviceContext->VSSetShaderResources(0, 4, nullSRVs);

	if (m_pOutputStructedBuffer_UAV)
		m_pOutputStructedBuffer_UAV->SetUnorderedAccessView(m_pOutputStructedBuffer->Get_UAV());
	if (m_pInputStructedBuffer_SRV)
		m_pInputStructedBuffer_SRV->SetResource(m_pInputStructedBuffer->Get_SRV());

	m_pOwner->Get_Pass(m_iPass)->Apply(0, m_pDeviceContext);

	m_pDeviceContext->CSSetShader(m_pComputeShader, nullptr, 0);
	m_pDeviceContext->Dispatch(iX, iY, iZ);

	// 즉시 리소스 해제 (Resource Hazard 방지)
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	m_pDeviceContext->CSSetShader(nullptr, nullptr, 0);
}

void CComputeShader::Resize_InputStruct(void* pArg, _uint iElementSize, _uint iNumElements)
{
	m_pInputStructedBuffer->Resize(pArg, iElementSize, iNumElements);
}

void CComputeShader::Resize_OutputStruct(void* pArg, _uint iElementSize, _uint iNumElements)
{
	m_pOutputStructedBuffer->Resize(pArg, iElementSize, iNumElements);
}

HRESULT CComputeShader::Ready_ComputeShader(COMSHADER_ORIGIN_DESC* pDesc)
{
	m_pOwner = m_pGameInstance->GetOrCreate_FxEffectAsset(pDesc->pShaderFilePath);
	if (m_pOwner == nullptr)
		return E_FAIL;
	Safe_AddRef(m_pOwner);
	ID3DX11EffectPass* pPass = m_pOwner->Get_Pass(0);
	if (pPass == nullptr)
		return E_FAIL;

	{
		D3DX11_PASS_SHADER_DESC csDesc;
		// 패스로부터 Compute Shader 정보를 가져온다
		if (FAILED(pPass->GetComputeShaderDesc(&csDesc)))
			return E_FAIL;

		if (csDesc.pShaderVariable->IsValid())
		{
			D3DX11_EFFECT_SHADER_DESC effectCsDesc;
			csDesc.pShaderVariable->GetShaderDesc(csDesc.ShaderIndex, &effectCsDesc);

			// EFFECT 변수 내부에 있는 실제 ID3D11ComputeShader 객체 가져오기.
			if (FAILED(csDesc.pShaderVariable->GetComputeShader(csDesc.ShaderIndex, &m_pComputeShader)))
				return E_FAIL;
		}
		else
		{
			MSG_BOX("이 셰이더에는 Compute Shader가 정의되어 있지 않습니다.");
			return E_FAIL;
		}
	}

	return S_OK;
}

#pragma region EffectVariable
ID3DX11EffectVariable* CComputeShader::Get_Variable(string name)
{
	return m_pOwner->Get_Variable(name);
}

ID3DX11EffectScalarVariable* CComputeShader::Get_Scalar(string name)
{
	return m_pOwner->Get_Scalar(name);
}

ID3DX11EffectVectorVariable* CComputeShader::Get_Vector(string name)
{
	return m_pOwner->Get_Vector(name);
}

ID3DX11EffectMatrixVariable* CComputeShader::Get_Matrix(string name)
{
	return m_pOwner->Get_Matrix(name);
}

ID3DX11EffectStringVariable* CComputeShader::Get_String(string name)
{
	return m_pOwner->Get_String(name);
}

ID3DX11EffectShaderResourceVariable* CComputeShader::Get_SRV(string name)
{
	return m_pOwner->Get_SRV(name);
}

ID3DX11EffectRenderTargetViewVariable* CComputeShader::Get_RTV(string name)
{
	return m_pOwner->Get_RTV(name);
}

ID3DX11EffectDepthStencilViewVariable* CComputeShader::Get_DSV(string name)
{
	return m_pOwner->Get_DSV(name);
}

ID3DX11EffectUnorderedAccessViewVariable* CComputeShader::Get_UAV(string name)
{
	return m_pOwner->Get_UAV(name);
}

ID3DX11EffectConstantBuffer* CComputeShader::Get_ConstantBuffer(string name)
{
	return m_pOwner->Get_ConstantBuffer(name);
}

ID3DX11EffectShaderVariable* CComputeShader::Get_Shader(string name)
{
	return m_pOwner->Get_Shader(name);
}

ID3DX11EffectBlendVariable* CComputeShader::Get_Blend(string name)
{
	return m_pOwner->Get_Blend(name);
}

ID3DX11EffectDepthStencilVariable* CComputeShader::Get_DepthStencil(string name)
{
	return m_pOwner->Get_DepthStencil(name);
}

ID3DX11EffectRasterizerVariable* CComputeShader::Get_Rasterizer(string name)
{
	return m_pOwner->Get_Rasterizer(name);
}

ID3DX11EffectSamplerVariable* CComputeShader::Get_Sampler(string name)
{
	return m_pOwner->Get_Sampler(name);
}

#pragma endregion


void CComputeShader::Bind_InputStructuredBuffer_Data(void* pArg, _uint iElementSize, _uint iNumElements)
{
	m_pInputStructedBuffer->Copy_Data(pArg, iElementSize, iNumElements);
}

#pragma region BINDING_CONSTANTBUFFER

void CComputeShader::Bind_Compute_EffectData(const EFFECT_PARTICLE_MU_ELEMENT& desc)
{
	m_pEffect_Mutable_Element_CBuffer->Copy_Data(desc);
}


#pragma endregion

HRESULT CComputeShader::Create_ConstantBuffer()
{
	// Particle 전용
	if (m_pEffect_MutableBuffer = Get_ConstantBuffer("MU_ParticleUpdate"))
	{
		m_pEffect_Mutable_Element_CBuffer = CConstant_Buffer<EFFECT_PARTICLE_MU_ELEMENT>::Create(m_pDevice, m_pDeviceContext);
		m_pEffect_MutableBuffer->SetConstantBuffer(m_pEffect_Mutable_Element_CBuffer->Get_Buffer());
	}

	return S_OK;
}

HRESULT CComputeShader::Create_StructBuffer(void* pArg)
{
	COMSHADER_COPY_DESC* pDesc = static_cast<COMSHADER_COPY_DESC*>(pArg);
	
	if (pDesc == nullptr)
	{
		MSG_BOX("Compute Shader Desc is NULL : COPY");
		return E_FAIL;
	}

	// ======   Input Data 생성   ======
	{
		if (m_pInputStructedBuffer_SRV = Get_SRV(pDesc->Input_StructBuffer.sBufferName))
		{
			m_pInputStructedBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, pDesc->Input_StructBuffer.iElementSize, pDesc->Input_StructBuffer.iNumElements);
			m_pInputStructedBuffer_SRV->SetResource(m_pInputStructedBuffer->Get_SRV());
		}
	}

	// ======   OutPut Data 생성   ======
	{
		if (m_pOutputStructedBuffer_UAV = Get_UAV(pDesc->OutPut_StructBuffer.sBufferName))
		{
			m_pOutputStructedBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, pDesc->OutPut_StructBuffer.iElementSize, pDesc->OutPut_StructBuffer.iNumElements);
			m_pOutputStructedBuffer_UAV->SetUnorderedAccessView(m_pOutputStructedBuffer->Get_UAV());
		}

		// output 통로 이름
		if (!Get_SRV(pDesc->Output_SRVBuffer_Name))
		{
			MSG_BOX("너님 쉐이더에 OUTPUT Buffer 이름이 그게 아닌뎁쇼");
			return E_FAIL;
		}
		else
			Get_SRV(pDesc->Output_SRVBuffer_Name)->SetResource(m_pOutputStructedBuffer->Get_SRV());
	}
	return S_OK;
}


CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CComputeShader* pInstance = new CComputeShader(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("CComputeShader::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CComputeShader::Clone(void* pArg)
{
	CComputeShader* pInstance = new CComputeShader(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CComputeShader::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CComputeShader::Clear_ConstantBuffer()
{
	Safe_Release(m_pEffect_Mutable_Element_CBuffer);
	Safe_Release(m_pEffect_MutableBuffer);

}

void CComputeShader::Clear_StructBuffer()
{
	Safe_Release(m_pInputStructedBuffer);
	Safe_Release(m_pInputStructedBuffer_SRV);
	Safe_Release(m_pOutputStructedBuffer);
	Safe_Release(m_pOutputStructedBuffer_UAV);
}

void CComputeShader::Free()
{
	Clear_ConstantBuffer();
	Clear_StructBuffer();

	Safe_Release(m_pOwner);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}