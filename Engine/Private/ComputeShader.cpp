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
	, m_pAnimE_Mutable_Element_CBuffer(rhs.m_pAnimE_Mutable_Element_CBuffer)
	, m_pAnimE_MutableBuffer(rhs.m_pAnimE_MutableBuffer)
	, m_pAnimB_Mutable_Element_CBuffer(rhs.m_pAnimB_Mutable_Element_CBuffer)
	, m_pAnimB_MutableBuffer(rhs.m_pAnimB_MutableBuffer)
	, m_pBone_Mutable_Element_CBuffer(rhs.m_pBone_Mutable_Element_CBuffer)
	, m_pBone_MutableBuffer(rhs.m_pBone_MutableBuffer)
	, m_pBoneMesh_Mutable_Element_CBuffer(rhs.m_pBoneMesh_Mutable_Element_CBuffer)
	, m_pBoneMesh_MutableBuffer(rhs.m_pBoneMesh_MutableBuffer)
	, m_pEffect_CurveInfoBuffer(rhs.m_pEffect_CurveInfoBuffer)
	, m_pEffect_CurveInfo(rhs.m_pEffect_CurveInfo)
	, m_pAnimMix_Mutable_Element_CBuffer(rhs.m_pAnimMix_Mutable_Element_CBuffer)
	, m_pAnimMix_MutableBuffer(rhs.m_pAnimMix_MutableBuffer)

{
	Safe_AddRef(m_pOwner);
	Safe_AddRef(m_pComputeShader);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	Safe_AddRef(m_pEffect_Mutable_Element_CBuffer);
	Safe_AddRef(m_pEffect_MutableBuffer);

	Safe_AddRef(m_pEffect_CurveInfoBuffer);
	Safe_AddRef(m_pEffect_CurveInfo);

	Safe_AddRef(m_pAnimE_Mutable_Element_CBuffer);
	Safe_AddRef(m_pAnimE_MutableBuffer);

	Safe_AddRef(m_pAnimB_Mutable_Element_CBuffer);
	Safe_AddRef(m_pAnimB_MutableBuffer);

	Safe_AddRef(m_pBone_Mutable_Element_CBuffer);
	Safe_AddRef(m_pBone_MutableBuffer);

	Safe_AddRef(m_pBoneMesh_Mutable_Element_CBuffer);
	Safe_AddRef(m_pBoneMesh_MutableBuffer);

	Safe_AddRef(m_pAnimMix_Mutable_Element_CBuffer);
	Safe_AddRef(m_pAnimMix_MutableBuffer);

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

void CComputeShader::Bind_InputStructuredBuffer(_uint Index, ID3DX11EffectShaderResourceVariable* pSRV, StructuredBuffer* pSB)
{
	// 없는 인덱스에 접근할거면 터질거긴한데 일부러 방어코드 작성안함. 디버깅용.
	m_pInputStructuredBuffer[Index] = std::make_pair(pSRV, pSB);
}


void CComputeShader::Dispatch(_uint iX, _uint iY, _uint iZ)
{
	// GPU <-> GPU 통신은 한쪽이 연결을 끊어줘야 다른쪽 연결도 성공한다.
	// 마치 삐삐랄까 무전기?
	// SRV 썼으면 닫아주고 UAV 세팅하고 .. 반복

	m_pDeviceContext->VSSetShaderResources(0, 4, s_pNullCS_SRV);
	m_pDeviceContext->PSSetShaderResources(0, 4, s_pNullCS_SRV);
	m_pDeviceContext->CSSetShaderResources(0, 16, s_pNullCS_SRV);
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 8, s_pNullCS_UAV, nullptr);

	if (m_pOutputStructedBuffer_UAV)
		m_pOutputStructedBuffer_UAV->SetUnorderedAccessView(m_pOutputStructedBuffer->Get_UAV());

	for (auto SB : m_pInputStructuredBuffer)
	{
		if (SB.first)
			SB.first->SetResource(SB.second->Get_SRV());
	}

	m_pOwner->Get_Pass(m_iPass)->Apply(0, m_pDeviceContext);

	m_pDeviceContext->CSSetShader(m_pComputeShader, nullptr, 0);
	m_pDeviceContext->Dispatch(iX, iY, iZ);

	m_pDeviceContext->CSSetUnorderedAccessViews(0, 8, s_pNullCS_UAV, nullptr);
	m_pDeviceContext->CSSetShader(nullptr, nullptr, 0);
}

void CComputeShader::Resize_InputStruct(_uint iIndex, void* pArg, _uint iElementSize, _uint iNumElements)
{
	m_pInputStructuredBuffer[iIndex].second->Resize(pArg, iElementSize, iNumElements);
}

void CComputeShader::Resize_OutputStruct(_uint iIndex, void* pArg, _uint iElementSize, _uint iNumElements)
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

StructuredBuffer* CComputeShader::Get_Input_Buffer(_uint iIndex)
{
	return m_pInputStructuredBuffer[iIndex].second;
}

StructuredBuffer* CComputeShader::Get_Output_Buffer()
{
	return m_pOutputStructedBuffer;
}

void CComputeShader::Set_OutputStructuredBuffer(StructuredBuffer* pSB)
{
	m_pOutputStructedBuffer = pSB;
	m_pOutputStructedBuffer_UAV->SetUnorderedAccessView(pSB->Get_UAV());

	//// SRV도 같이 교체
	//m_pOutputSRV->SetResource(pSB->Get_SRV());
}

void CComputeShader::Bind_InputStructuredBuffer_Data(_uint iIndex, void* pArg, _uint iElementSize, _uint iNumElements)
{
	m_pInputStructuredBuffer[iIndex].second->Copy_Data(pArg, iElementSize, iNumElements);
}

#pragma region BINDING_CONSTANTBUFFER

void CComputeShader::Bind_Compute_EffectData(const EFFECT_PARTICLE_MU_ELEMENT& desc)
{
	m_pEffect_Mutable_Element_CBuffer->Copy_Data(desc);
}

void CComputeShader::Bind_Compute_EffectCurveData(const EFFECT_CURVEINFO& desc)
{
	m_pEffect_CurveInfo->Copy_Data(desc);
}

void CComputeShader::Bind_Compute_Track(const CS_MU_TRACK& desc)
{
	m_pAnimE_Mutable_Element_CBuffer->Copy_Data(desc);
}

void CComputeShader::Bind_Compute_BlendMu(const CS_MU_ANIMB& desc)
{
	m_pAnimB_Mutable_Element_CBuffer->Copy_Data(desc);
}

void CComputeShader::Bind_Compute_BoneMuCB(const CS_MU_GROUPNUMS& desc)
{
	m_pBone_Mutable_Element_CBuffer->Copy_Data(desc);
}

void CComputeShader::Bind_Compute_BoneMeshCB(const CS_CB_MU_BONEMESH& desc)
{
	m_pBoneMesh_Mutable_Element_CBuffer->Copy_Data(desc);
}

void CComputeShader::Bind_Compute_AnimMixCB(const CS_MU_ANIMMIX& desc)
{
	m_pAnimMix_Mutable_Element_CBuffer->Copy_Data(desc);
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

	if (m_pEffect_CurveInfoBuffer = Get_ConstantBuffer("CurveInfo"))
	{
		m_pEffect_CurveInfo = CConstant_Buffer<EFFECT_CURVEINFO>::Create(m_pDevice, m_pDeviceContext);
		m_pEffect_CurveInfoBuffer->SetConstantBuffer(m_pEffect_CurveInfo->Get_Buffer());
	}

	// AnimE 전용
	if (m_pAnimE_MutableBuffer = Get_ConstantBuffer("MU_Track"))
	{
		m_pAnimE_Mutable_Element_CBuffer = CConstant_Buffer<CS_MU_TRACK>::Create(m_pDevice, m_pDeviceContext);
		m_pAnimE_MutableBuffer->SetConstantBuffer(m_pAnimE_Mutable_Element_CBuffer->Get_Buffer());
	}

	// AnimB 전용
	if (m_pAnimB_MutableBuffer = Get_ConstantBuffer("MU_RATIO"))
	{
		m_pAnimB_Mutable_Element_CBuffer = CConstant_Buffer<CS_MU_ANIMB>::Create(m_pDevice, m_pDeviceContext);
		m_pAnimB_MutableBuffer->SetConstantBuffer(m_pAnimB_Mutable_Element_CBuffer->Get_Buffer());
	}

	// BoneCombine 전용
	if (m_pBone_MutableBuffer = Get_ConstantBuffer("MU_BONENUMS"))
	{
		m_pBone_Mutable_Element_CBuffer = CConstant_Buffer<CS_MU_GROUPNUMS>::Create(m_pDevice, m_pDeviceContext);
		m_pBone_MutableBuffer->SetConstantBuffer(m_pBone_Mutable_Element_CBuffer->Get_Buffer());
	}

	// BoneMesh 전용
	if (m_pBoneMesh_MutableBuffer = Get_ConstantBuffer("MU_MESHBONENUMS"))
	{
		m_pBoneMesh_Mutable_Element_CBuffer = CConstant_Buffer<CS_CB_MU_BONEMESH>::Create(m_pDevice, m_pDeviceContext);
		m_pBoneMesh_MutableBuffer->SetConstantBuffer(m_pBoneMesh_Mutable_Element_CBuffer->Get_Buffer());
	}

	// AnimMix 전용

	/*
	m_pAnimMix_Mutable_Element_CBuffer
	m_pAnimMix_MutableBuffer		
	
	*/
	if (m_pAnimMix_MutableBuffer = Get_ConstantBuffer("MU_MIX"))
	{
		m_pAnimMix_Mutable_Element_CBuffer = CConstant_Buffer<CS_MU_ANIMMIX>::Create(m_pDevice, m_pDeviceContext);
		m_pAnimMix_MutableBuffer->SetConstantBuffer(m_pAnimMix_Mutable_Element_CBuffer->Get_Buffer());
	}

	return S_OK;
}

HRESULT CComputeShader::Create_StructBuffer(void* pArg)
{
	COMSHADER_COPY_DESC* pDesc = static_cast<COMSHADER_COPY_DESC*>(pArg);
	m_pInputStructuredBuffer.resize(pDesc->InputBufferNum);

	if (pDesc == nullptr)
	{
		MSG_BOX("Compute Shader Desc is NULL : COPY");
		return E_FAIL;
	}

	// ======   Input Data 생성   ======
	{
		// 단일 버퍼인 사람을 위한 것.

		// sb를 처음에 만들어서 쓰겠다면
		m_bHas_OwnSRV = pDesc->bMakeSB;
		if (m_bHas_OwnSRV)
		{
			if (m_pInputStructuredBuffer[0].first = Get_SRV(pDesc->Input_StructBuffer.sBufferName))
			{
				m_pInputStructuredBuffer[0].second = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, pDesc->Input_StructBuffer.iElementSize, pDesc->Input_StructBuffer.iNumElements);
				m_pInputStructuredBuffer[0].first->SetResource(m_pInputStructuredBuffer[0].second->Get_SRV());
			}
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

	Safe_Release(m_pAnimE_Mutable_Element_CBuffer);
	Safe_Release(m_pAnimE_MutableBuffer);

	Safe_Release(m_pAnimB_Mutable_Element_CBuffer);
	Safe_Release(m_pAnimB_MutableBuffer);

	Safe_Release(m_pBone_Mutable_Element_CBuffer);
	Safe_Release(m_pBone_MutableBuffer);

	Safe_Release(m_pBoneMesh_Mutable_Element_CBuffer);
	Safe_Release(m_pBoneMesh_MutableBuffer);

	Safe_Release(m_pEffect_CurveInfo);
	Safe_Release(m_pEffect_CurveInfoBuffer);

	Safe_Release(m_pAnimMix_Mutable_Element_CBuffer);
	Safe_Release(m_pAnimMix_MutableBuffer);
}

void CComputeShader::Clear_StructBuffer()
{
	// SB는 clone때만 생성 & 0번만 bool값에 따라 생성하므로
	// todo : 만약 벡터에 있는 srv들을 초기에 여기 컴포넌트 안에서 생성한다면 구조 변경 필요
	if (IsClone() && m_bHas_OwnSRV)
	//	if (m_bHas_OwnSRV)
	{
		Safe_Release(m_pInputStructuredBuffer[0].first);
		Safe_Release(m_pInputStructuredBuffer[0].second);
		//for (auto SB : m_pInputStructuredBuffer)
		//{
		//	Safe_Release(SB.first);
		//	Safe_Release(SB.second);
		//}
	}

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