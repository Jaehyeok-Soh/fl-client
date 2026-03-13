#include "Engine_pch.h"
#include "Render_Manager.h"
#include "Constant_Buffer.h"
#include "TextureBase.h"
#include "GameObject.h"
#include "Camera.h"
#include "VIBuffer_Rect_Tex.h"
#include "Shader.h"
#include "Bounds.h"
#include "RenderTarget.h"
#include "Octree_Manager.h"
#include "EngineConsole.h"
#include "UIObject.h"
#include "GameInstance.h"

CRender_Manager::CRender_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	m_filteredRenderObjects.reserve(10000);
	m_visibleNear.reserve(10000);
	m_visibleMid.reserve(10000);
	m_visibleFar.reserve(10000);
}

HRESULT CRender_Manager::Initialize()
{
	//========================
	// Viewport Save / Set Half
	//========================
	_uint iViewportsCount = { 1 };
	m_pDeviceContext->RSGetViewports(&iViewportsCount, &m_defaultViewport);
	m_halfViewport = m_defaultViewport;
	m_halfViewport.Width *= 0.5f;
	m_halfViewport.Height *= 0.5f;

	const _uint& iWidth = (_uint)m_defaultViewport.Width;
	const _uint& iHeight = (_uint)m_defaultViewport.Height;
	const _uint& iHalfWidth = (_uint)m_halfViewport.Width;
	const _uint& iHalfHeight = (_uint)m_halfViewport.Height;


	// For. Target_Diffuse
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Diffuse, &desc)))
			return E_FAIL;
	}
	// For. Target_Normal
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4{ 0.f,0.f,0.f,1.f };
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Normal, &desc)))
			return E_FAIL;
	}
	// For. Target_SpecularMask
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::SpecularMask, &desc)))
			return E_FAIL;
	}
	// For. Target_Depth
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R32G32_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Depth, &desc)))
			return E_FAIL;
	}
	// For. Target_ObjectInfo
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R32_UINT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::ObjectInfo, &desc)))
			return E_FAIL;
	}
	// For. Target_AO_Ping
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16_FLOAT;
		desc.iWidth = iHalfWidth;
		desc.iHeight = iHalfHeight;
		desc.vClearColor = Vec4::One;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::SSAO_Ping, &desc)))
			return E_FAIL;
	}
	// For. Target_AO_Pong
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16_FLOAT;
		desc.iWidth = iHalfWidth;
		desc.iHeight = iHalfHeight;
		desc.vClearColor = Vec4::One;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::SSAO_Pong, &desc)))
			return E_FAIL;
	}
	// For. Target_AO_Full
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::One;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::SSAO_Full, &desc)))
			return E_FAIL;
	}
	// For. Target_Shade
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Shade, &desc)))
			return E_FAIL;
	}
	// For. Target_Specular
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Specular, &desc)))
			return E_FAIL;
	}
	// For. Target_SceneHDR
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::SceneHDR, &desc)))
			return E_FAIL;
	}

	// For. Targert_OIT_ACCUM
	{
		// 누적 색상 버퍼
		// 역할 : 화면의 모든 픽셀에 겹쳐진 모든 투명 물체들의 색상을 다 더해놓는 곳.
		// 수식 : 시그마(Color * Alpha * Weight)
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if(FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::OIT_Accum, &desc)))
			return E_FAIL;
	}

	// For. Target_OIT_Reveal
	{
		// 배경 투과율 버퍼
		// 역할 : 배경이 이 투명 물체들에 의해 얼마나 가려졌는가? // 얼마나 살아있는가?를 저장하는 곳.
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16_FLOAT; // 투과율만 저장하므로 단일 채널에 저장을 한다.
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::One;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::OIT_Reveal, &desc)))
			return E_FAIL;
	}

	// For. Target_Scene
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::SceneHDR_Copy, &desc)))
			return E_FAIL;
	}
	// For. Target_Bloom_Ping
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iHalfWidth;
		desc.iHeight = iHalfHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Bloom_Ping, &desc)))
			return E_FAIL;
	}
	// For. Target_Bloom_Pong
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iHalfWidth;
		desc.iHeight = iHalfHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Bloom_Pong, &desc)))
			return E_FAIL;
	}

	// For. MRT_GameObjects
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::Diffuse)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::Normal)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::SpecularMask)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::Depth)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::ObjectInfo)))
			return E_FAIL;
	}

	// For. MRT_LightAcc
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::LightAcc, ERenderTarget::Shade)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::LightAcc, ERenderTarget::Specular)))
			return E_FAIL;
	}

	// For. MRT_SSAO_Gen
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::SSAO_Gen, ERenderTarget::SSAO_Ping)))
			return E_FAIL;
	}

	// For. MRT_SSAO_BlurH
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::SSAO_BlurH, ERenderTarget::SSAO_Pong)))
			return E_FAIL;
	}

	// For. MRT_SSAO_BlurV
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::SSAO_BlurV, ERenderTarget::SSAO_Ping)))
			return E_FAIL;
	}

	// For. MRT_SSAO_Upsample
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::SSAO_Upsample, ERenderTarget::SSAO_Full)))
			return E_FAIL;
	}
	
	// For. MRT_CombinedHDR
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::CombineHDR, ERenderTarget::SceneHDR)))
			return E_FAIL;
	}

	// For. MRT_SceneHDR_Acc
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::SceneHDR_Acc, ERenderTarget::SceneHDR)))
			return E_FAIL;
	}

	// For. MRT_OIT_RENDER
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::OIT_Render, ERenderTarget::OIT_Accum)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::OIT_Render, ERenderTarget::OIT_Reveal)))
			return E_FAIL;
	}

	// For. MRT_Bloom_extract
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::Bloom_Extract, ERenderTarget::Bloom_Ping)))
			return E_FAIL;
	}

	// For. MRT_Bloom_BlurH
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::Bloom_BlurH, ERenderTarget::Bloom_Pong)))
			return E_FAIL;
	}

	// For. MRT_Bloom_BlurV
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::Bloom_BlurV, ERenderTarget::Bloom_Ping)))
			return E_FAIL;
	}


	// For. MRT_Shadow
	{
	}

	// ========== WBOIT 전용 BlendState ==========
	{
		if (FAILED(Ready_BlendStates()))
			return E_FAIL;
	}

	m_matWorld_RT = Matrix::CreateScale(m_defaultViewport.Width, m_defaultViewport.Height, 1.f);
#ifdef _DEBUG
	if (FAILED(Ready_Debug()))
		return E_FAIL;
#endif
	return S_OK;
}

HRESULT CRender_Manager::Ready_BlendStates()
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = TRUE;

	//---------------------------------------------------------
	// WBOIT Accumulate State (정보 수집용)
	//---------------------------------------------------------
	// RT 0: Accumulation (D = S + D)
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// RT 1: Revealage (D = D * (1 - S.a))
	blendDesc.RenderTarget[1].BlendEnable = TRUE;
	blendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;

	if (FAILED(m_pDevice->CreateBlendState(&blendDesc, &m_pWBOIT_AccumulateBS)))
		return E_FAIL;

	//---------------------------------------------------------
	// Alpha Blend State (WBOIT 합성 및 일반 UI용)
	//---------------------------------------------------------
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.IndependentBlendEnable = FALSE; // 모든 RT 공통 적용
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendBS)))
		return E_FAIL;

	return S_OK;
}


void CRender_Manager::Push_RenderObject(RENDER_CATEGORY eCategory, CGameObject* pGO)
{
	if (pGO == nullptr)
		return;

	if (eCategory >= RENDER_CATEGORY::END)
		return;

	Safe_AddRef(pGO);
	m_renderObjects[ENUM_TO_UINT(eCategory)].push_back(pGO);
}

#pragma region Legacy
//if (CMesh_Renderer* pMesh_Renderer = pElement->Get_MeshRenderer())
//{
//	Bind_TransformData(pElement->Get_Transform()->Get_WorldMatrix());
//	pMesh_Renderer->Render();
//	//	if (CAnimator* pAnimator = pElement->Get_Animator())
//	//	{
//	//		const ANIM_KEYFRAME& Keyframe = pAnimator->Get_CurrentAnimationKeyFrame();
//	//		m_tAnimationData.vSpriteOffset = Keyframe.vOffset;
//	//		m_tAnimationData.vSpriteSize = Keyframe.vSize;
//	//		m_tAnimationData.vTextureSize = pAnimator->Get_CurrentAnimation()->Get_TextureSize();
//	//		m_tAnimationData.fUseAnimation = 1.f;
//	//		Push_AnimationData();

//	//		m_pPipeline->Set_ConstantBuffer(SS_VertexShader, 2, m_pAnimation_CBuffer);
//	//		m_pPipeline->Set_Texture(SS_PixelShader, 0, pAnimator->Get_CurrentAnimation()->Get_Texture());

//	//		if (!m_bAnimDataDirty) m_bAnimDataDirty = true;
//	//	}
//	//	else
//	//	{
//	//		if (m_bAnimDataDirty)
//	//		{
//	//			m_tAnimationData.vSpriteOffset = { 0.0f, 0.0f };
//	//			m_tAnimationData.vSpriteSize = { 0.0f, 0.0f };
//	//			m_tAnimationData.vTextureSize = { 0.0f, 0.0f };
//	//			m_tAnimationData.fUseAnimation = 0.0f;
//	//			Push_AnimationData();
//	//			m_bAnimDataDirty = false;
//	//		}

//	//		m_pPipeline->Set_Texture(SS_PixelShader, 0, pMesh_Renderer->Get_Texture());
//	//	}

//	//	PIPELINE_INFO info;
//	//	info.pInputLayout = pMesh_Renderer->Get_InputLayout();
//	//	info.pVertexShader = pMesh_Renderer->Get_VertexShader();
//	//	info.pPixelShader = pMesh_Renderer->Get_PixelShader();
//	//	info.pRasterizerState = m_pRasterizerState;
//	//	info.pBlendState = m_pBlendState;
//	//	m_pPipeline->Update_Pipeline(info);

//	//	m_pPipeline->Set_VertexBuffer(pMesh_Renderer->Get_VertexBuffer());
//	//	m_pPipeline->Set_IndexBuffer(pMesh_Renderer->Get_IndexBuffer());
//	//	m_pPipeline->Set_ConstantBuffer(SS_VertexShader, 1, m_pTransform_CBuffer);
//	//	m_pPipeline->Set_ConstantBuffer(SS_VertexShader, 0, m_pCamera_CBuffer);
//	//	//m_pPipeline->Set_Texture(SS_PixelShader, 0, pMesh_Renderer->Get_Texture());
//	//	m_pPipeline->Set_SamplerState(SS_PixelShader, 0, m_pSamplerState);

//	//	m_pPipeline->Draw_Indexed(pMesh_Renderer->Get_Mesh()->Get_IndicesCount(), 0, 0);
//}
//else if(CModel_Renderer* pModel_Renderer = pElement->Get_ModelRenderer())
//{
//	Bind_TransformData(pElement->Get_Transform()->Get_WorldMatrix());
//	pModel_Renderer->Render();
//}
#pragma endregion

HRESULT CRender_Manager::Set_ShaderResources()
{
	{
		if (!(m_pVIBuffer = CVIBuffer_Rect_Tex::Create(m_pDevice, m_pDeviceContext, nullptr)))
			return E_FAIL;

		CShader::SHADER_ORIGIN_DESC desc = {};
		desc.pShaderFilePath = L"../../Shaders/Shader_Deffered.hlsl";
		desc.eLayout = EVtxLayout::VTXPOSTEX;
		if (!(m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, &desc)))
			return E_FAIL;
	}

	{
		CTextureBase::RESOURCE_BASE_DESC desc{};
		desc.wstrName = L"T_LUT_Stand";
		desc.wstrPath = L"../../Resources/Textures/T_LUT_Stand.png";
		m_pLUTTexture = m_pGameInstance->GetOrAddTexture(desc.wstrName, &desc);
		if (m_pLUTTexture == nullptr)
			return E_FAIL;
	}

	if (FAILED(Set_ConstantBuffer()))
		return E_FAIL;

	if (FAILED(Create_SSAO_NoiseSRV()))
		return E_FAIL;

	// SSAOkernelDesc
	{
		array<Vec4, SSAO_KERNAL> arrKernal = Build_SSAO_Kernal16();
		::memcpy(m_tSSAOkernelDesc.vKernel, arrKernal.data(), sizeof(Vec4) * SSAO_KERNAL);
		m_tSSAOkernelDesc.vNoiseScale.x = m_halfViewport.Width / 4;
		m_tSSAOkernelDesc.vNoiseScale.y = m_halfViewport.Height / 4;

		if (FAILED(m_pCB_SSAOkernel->Copy_Data(m_tSSAOkernelDesc)))
			return E_FAIL;
	}
	
	// SSAOparamDesc
	{
		m_tSSAOparamDesc.vInvSize = { 1.0f / m_halfViewport.Width, 1.0f / m_halfViewport.Height };
		m_tSSAOparamDesc.fRadius = 1.5f;
		m_tSSAOparamDesc.fBias = 0.05f;
		m_tSSAOparamDesc.fIntensity = 1.3f;
		m_tSSAOparamDesc.fPower = 1.0f;
		m_tSSAOparamDesc.fFadeStart = 20.f;
		m_tSSAOparamDesc.fFadeEnd = 80.f;

		if (FAILED(m_pCB_SSAOparam->Copy_Data(m_tSSAOparamDesc)))
			return E_FAIL;
	}

	// HDRparamDesc
	{
		m_tHDRparamDesc.fExposure = 0.9f;
		// ContrastGamma
		m_tHDRparamDesc.fGamma = 1.5f;

		if(FAILED(m_pCB_HDRparam->Copy_Data(m_tHDRparamDesc)))
			return E_FAIL;
	}

	// BloomparamDesc
	{
		m_tBloomparamDesc.fThreshold = 1.2f;
		m_tBloomparamDesc.fKnee = 0.4f;
		m_tBloomparamDesc.fIntensity = 1.4f;
		m_tBloomparamDesc.vInvSize = { 1.0f / m_halfViewport.Width, 1.0f / m_halfViewport.Height };
		
		if (FAILED(m_pCB_Bloomparam->Copy_Data(m_tBloomparamDesc)))
			return E_FAIL;
	}

	// OutlineDesc
	{
		m_tOutlineparamDesc.vColor = { 0.f, 0.f, 0.f, 1.f };
		m_tOutlineparamDesc.vInvSize = { 1.0f / m_defaultViewport.Width, 1.0f / m_defaultViewport.Height };
		m_tOutlineparamDesc.fThicknessPx = 0.6f;
		m_tOutlineparamDesc.fOpacity = 0.7f;
		m_tOutlineparamDesc.fNormalThreshold = 1.f;
		m_tOutlineparamDesc.fDepthThreshold = 0.015f;
		m_tOutlineparamDesc.fNormalStrength = 1.5f;
		m_tOutlineparamDesc.fDepthStrength = 10.f;
		m_tOutlineparamDesc.fFadeStart = 20.f;
		m_tOutlineparamDesc.fFadeEnd = 40.f;

		if (FAILED(m_pCB_Outlineparam->Copy_Data(m_tOutlineparamDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CRender_Manager::Render()
{
	m_pGameInstance->Setup_ViewProj_ToCBuffer();
	m_pGameInstance->Setup_Inv_ToCBuffer();

	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NoneBlend()))
		return E_FAIL;

	if (FAILED(Render_ComputeLight_Blend()))
		return E_FAIL;

	m_pGameInstance->Setup_UIViewProj_ToCBuffer();

	if (FAILED(Render_SSAO()))
		return E_FAIL;

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_CombinedHDR()))
		return E_FAIL;

	m_pGameInstance->Setup_ViewProj_ToCBuffer();

	// SceneHDR에 누적
	{
		// SceneHDR 연산 - 1
		{
			if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::SceneHDR_Acc, false, true)))
				return E_FAIL;

			if (FAILED(Render_Environment()))
				return E_FAIL;

			m_pGameInstance->Setup_UIViewProj_ToCBuffer();

			if (FAILED(Render_Outline()))
				return E_FAIL;

			m_pGameInstance->Setup_ViewProj_ToCBuffer();

			if (FAILED(m_pGameInstance->End_MRT())) return E_FAIL;
		}

		// WBOIT 정보 수집
		{
			// Accum:0, Reveal:1
			if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::OIT_Render, true, true)))
				return E_FAIL;

			//// 블랜드 State 바인딩
			m_pDeviceContext->OMSetBlendState(m_pWBOIT_AccumulateBS, nullptr, 0xffffffff);

			// 투명 물체들 값 저장.
			if (FAILED(Render_NonLights())) return E_FAIL;
			if (FAILED(Render_Blend())) return E_FAIL;

			if (FAILED(m_pGameInstance->End_MRT())) return E_FAIL;
		}

		// SceneHDR + WBOIT 합성 연산
		{
			m_pGameInstance->Setup_UIViewProj_ToCBuffer();

			if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::SceneHDR_Acc, false, true)))
				return E_FAIL;

			//// 블랜드 State 바인딩
			m_pDeviceContext->OMSetBlendState(m_pAlphaBlendBS, nullptr, 0xffffffff);

			// 가중치 Blend 기법
			if (FAILED(Render_WBOIT())) return E_FAIL;

			m_pGameInstance->Setup_ViewProj_ToCBuffer();
			//// 블렌드 해제
			m_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
		}

		// SceneHDR Copy하기
		{
			if (FAILED(Render_Distotion()))
				return E_FAIL;
		}

		// SceneHDR 종료
		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}

	m_pGameInstance->Setup_UIViewProj_ToCBuffer();

	if (FAILED(Render_Bloom()))
		return E_FAIL;

	if (FAILED(Render_ToneMap()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	if (FAILED(Render_BlendUI()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Fonts()))
		return E_FAIL;

#ifdef _DEBUG
	if (m_pGameInstance->KeyButton_Down(DIK_F9))
		m_bDebug = !m_bDebug;

	if (m_bDebug == true)
	{
		if (FAILED(Render_Debug()))
			return E_FAIL;
	}
	else
	{
		for (auto& pDebugCom : m_debugComponents)
			Safe_Release(pDebugCom);

		m_debugComponents.clear();
	}
#endif

	return S_OK;
}

void CRender_Manager::Clear()
{
	for (auto& list : m_renderObjects)
	{
		list.clear();
	}
}

HRESULT CRender_Manager::Render_Priority()
{
	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::PRIORITY)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::PRIORITY)].clear();

	return S_OK;
}

HRESULT CRender_Manager::Render_Blend()
{
	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::BLEND)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	} 
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::BLEND)].clear();

#pragma region Weighted OIT에 관해서
// [TODO: Weighted OIT (가중 누적 기반 Order-Independent Transparency)]
//
// 목적:
//  - 투명 오브젝트(이펙트/파티클/투명 메쉬)를 "알파 정렬(Alpha Sorting)" 없이도
//    비교적 안정적으로(근사적으로) 합성하기 위한 방식.
//  - 정렬을 완전히 대체하는 "근사"이며, 구현 난이도/성능/품질 밸런스가 좋아 이펙트 쪽에 유효.
//
// 핵심 아이디어(2개의 RT로 누적 후, 1번의 Fullscreen Composite):
//  1) Transparent들을 OIT 버퍼에 "누적(accumulate)"한다.
//  2) 누적 결과를 SceneHDR(누적용, NoClear)에 "합성(composite)"한다.
//  3) 그 다음에 Bloom / ToneMap을 수행해야(=후처리에 투명도 포함) 전체 파이프라인이 일관적
//
// ------------------------------------------------------------
// 0) 파이프라인 위치(매우 중요)
// ------------------------------------------------------------
//  - Opaque 결과가 SceneHDR에 만들어진 이후(= CombineHDR 이후)
//  - Transparent_WOIT 누적(Accum/Reveal) -> Composite로 SceneHDR(NoClear)에 반영
//  - 이후에 Bloom(Extract/Blur) -> ToneMap(BackBuffer 출력)
//  => 반드시 "ToneMap & Bloom BEFORE" 가 아니라,
//     "WOIT Composite가 ToneMap & Bloom보다 BEFORE" 여야 함.
//     (투명 이펙트가 Bloom에 기여해야 자연스럽다)
//
// ------------------------------------------------------------
// 1) 필요 RenderTarget(권장 포맷) 및 Clear 규칙
// ------------------------------------------------------------
//  - OIT_Accum  (RGBA16F 권장):
//      * 누적 색(accum.rgb) + 누적 분모(accum.a)
//      * 매 프레임 Clear: (0,0,0,0)
//
//  - OIT_Reveal (R16F 또는 R8_UNORM):
//      * revealage(가시성) 누적: 최종 알파를 구하기 위한 값
//      * 매 프레임 Clear: 1.0 (흰색)
//      * 의미: 겹칠수록 1 -> 0으로 내려가며 "가려짐/불투명화"가 누적됨
//
// ------------------------------------------------------------
// 2) 누적 패스(Transparent_WOIT) : per-object draw
// ------------------------------------------------------------
//  입력:
//   - src.rgb, src.a (투명 오브젝트 최종 색/알파)
//   - depth(또는 viewZ) : 가중치(weight)에 활용 (원거리 영향 ↓)
//
//  가중치 w (개념):
//   - 가까운 픽셀이 더 강하게 누적되도록 depth 기반 가중치를 준다.
//   - 예시(개념): 
//       w = clamp( pow(1 - depth01, k), 0, 1 ) * max(src.a, eps)
//     * depth01: [0..1]로 정규화된 depth (가까울수록 0)
//     * k: 2~8 범위에서 튜닝(가까운 투명 강조 정도)
//   - 이 w는 "정확성"보다는 "안정적 근사"를 위한 파라미터.
//
//  Accum 누적(개념 식):
//   - premultipliedColor = src.rgb * src.a
//   - accum.rgb += premultipliedColor * w
//   - accum.a   += src.a * w                 // (분모 역할)
//
//  Reveal 누적(개념 식):
//   - reveal *= (1 - src.a)
//   - 겹치면 겹칠수록 reveal이 0으로 수렴 -> 최종 알파가 커짐
//
//  메모:
//   - 이 단계는 MRT(Accum + Reveal)로 동시에 렌더하는 형태가 일반적.
//   - BlendState는 "Accum은 Additive", "Reveal은 Multiplicative(곱 누적)"로 세팅.
//     (구체 블렌드 팩터는 엔진 스타일에 맞춰 설정)
//
// ------------------------------------------------------------
// 3) 합성 패스(Composite) : fullscreen 1회, SceneHDR(NoClear)에 반영
// ------------------------------------------------------------
//  누적 결과로 투명 레이어의 최종 색/알파를 복원:
//
//   transparentColor = accum.rgb / max(accum.a, eps)
//   transparentAlpha = 1 - reveal
//
//  SceneHDR에 합성(개념 식):
//   sceneHDR.rgb = lerp(sceneHDR.rgb, transparentColor, transparentAlpha)
//   // 동치 형태:
//   // sceneHDR.rgb = sceneHDR.rgb * (1 - transparentAlpha) + transparentColor * transparentAlpha
//
//  주의:
//   - Composite는 "SceneHDR 누적 레이어(NoClear)"에 그려야 한다.
//   - 그래야 이후 Bloom/ToneMap이 SceneHDR 하나만 보면 된다.
//
// ------------------------------------------------------------
// 4) Render_Category 설계 힌트(추후)
// ------------------------------------------------------------
//  - RENDER_CATEGORY::TRANSPARENT_WOIT (추후 추가)
//      * 투명 메쉬 이펙트 / 반투명 파티클 등 "일반 투명"은 여기로 모아 누적
//
//  - RENDER_CATEGORY::ADDITIVE_EFFECT (선택)
//      * 불꽃/빛줄기 같은 Additive 파티클은 WOIT보다
//        sceneHDR에 직접 Additive가 더 자연스러울 때가 많음.
//      * 팀이 선택할 수 있게 카테고리 분리 여지 남겨두기.
//
// ------------------------------------------------------------
// 5) 한계/주의사항(팀이 오해 안 하게)
// ------------------------------------------------------------
//  - Weighted OIT는 "정렬을 완전 대체"하는 정확 해법이 아니라 근사 해법.
//    * 두꺼운 유리, 다층 굴절, 내부 산란 같은 케이스에는 부정확할 수 있음.
//  - 그래도 이펙트(연기/먼지/마법/반투명 메쉬 이펙트)에는 품질 대비 비용이 좋음.
//  - D3D11 제약: 동일 리소스 SRV/RTV 동시 바인딩 불가.
//    => Accum/Reveal은 출력용 RTV, Composite에서는 입력용 SRV로 분리해서 사용.
// ------------------------------------------------------------
#pragma endregion

	return S_OK;
}

HRESULT CRender_Manager::Render_Bloom()
{
	m_pDeviceContext->RSSetViewports(1, &m_halfViewport);

	// Extract
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::Bloom_Extract, true, false)))
		goto FAIL;

	if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SceneHDR, m_pShader)))
		goto FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::BLOOM_EXTRACT));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		goto FAIL;

	// Ping
	if(FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::Bloom_BlurH, true, false)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Bloom_Ping, m_pShader)))
		goto FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::BLOOM_BLURH));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		goto FAIL;
	
	// Pong
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::Bloom_BlurV, true, false)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Bloom_Pong, m_pShader)))
		goto FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::BLOOM_BLURV));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		goto FAIL;

	m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
	return S_OK;
FAIL:
	m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
	return E_FAIL;
}

HRESULT CRender_Manager::Render_ToneMap()
{
	if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SceneHDR, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV(EFXSRV::LUT_Stand, m_pLUTTexture->Get_SRV())))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Bloom_Ping, m_pShader)))
		return E_FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::TONEMAP));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRender_Manager::Render_Distotion()
{
	if (m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::DISTOTION)].size() != 0)
	{
		m_pGameInstance->Copy_SceneHDRResource(ERenderTarget::SceneHDR_Copy);
	}

	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::DISTOTION)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::DISTOTION)].clear();

	return S_OK;
}

HRESULT CRender_Manager::Render_WBOIT()
{
	//if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
	//	return E_FAIL;

	// OIT 텍스처들을 셰이더 SRV로 바인딩.
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::OIT_Accum, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::OIT_Reveal, m_pShader)))
		return E_FAIL;

	// WBOIT 합성 패스
	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::WBOIT));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRender_Manager::Render_NonLights()
{
	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::NONELIGHT)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::NONELIGHT)].clear();

	return S_OK;
}

HRESULT CRender_Manager::Render_NoneBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::GameObjects)))
		return E_FAIL;

	BoundingFrustum* pFrustrum = m_pGameInstance->Get_BoundingFrustrum_World();
	// 디버그용
	OCTREE_QUERY_STATS tStats{};
	m_pGameInstance->m_pOctree_Manager->Query_Visible(*pFrustrum, RENDER_CATEGORY::NONEBLEND, m_filteredRenderObjects, &tStats);

#ifdef _DEBUG
	static uint32_t sFrame = 0;
	if ((sFrame++ % 60) == 0)
	{
		// 없으면 m_umapEntries.size()를 출력하거나 getter 추가
		string strLog{ "OCTREELOG, visitedNodes: " + std::to_string(tStats.iVisitedNodes) +
			" tested: " + std::to_string(tStats.iTestedEntries) +
			" visible: " + std::to_string(tStats.iVisibleOut) };
		CLOG_INFO(strLog);
	}
#endif

	//for (CGameObject* pElement : m_filteredRenderObjects)
	//{
	//	if (FAILED(pElement->Render()))
	//	{
	//		m_filteredRenderObjects.clear();
	//		return E_FAIL;
	//	}
	//}
	//m_filteredRenderObjects.clear();

	for (CGameObject* pElement : m_filteredRenderObjects)
	{
		if (pElement == nullptr)
			continue;

		CBounds *pBounds = pElement->Get_Component<CBounds>();
		BoundingBox *pAABB = pBounds->Get_WolrdAABB();
		const EFrustrumTier eTier = m_pGameInstance->Classify_BySplitFrustrum(*pAABB);
		switch (eTier)
		{
		case EFrustrumTier::Near: m_visibleNear.push_back(pElement); break;
		case EFrustrumTier::Mid:  m_visibleMid.push_back(pElement);  break;
		case EFrustrumTier::Far:  m_visibleFar.push_back(pElement);  break;
		default:
			break;
		}
	}
	m_filteredRenderObjects.clear();


	for (CGameObject* pElement : m_visibleNear)
	{
		if (FAILED(pElement->Render()))
		{
			m_visibleNear.clear();
			m_visibleMid.clear();
			m_visibleFar.clear();
			return E_FAIL;
		}
	}

	for (CGameObject* pElement : m_visibleMid)
	{
		if (FAILED(pElement->Render()))
		{
			m_visibleNear.clear();
			m_visibleMid.clear();
			m_visibleFar.clear();
			return E_FAIL;
		}
	}

	m_visibleNear.clear();
	m_visibleMid.clear();
	m_visibleFar.clear();
	

	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::NONEBLEND)])
	{
		if (FAILED(pElement->Render()))
		{
			Safe_Release(pElement);
			m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::NONEBLEND)].clear();
			return E_FAIL;
		}

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::NONEBLEND)].clear();
	

	return S_OK;
}

HRESULT CRender_Manager::Render_ComputeLight_Blend()
{

	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::COMPUTELIGHT_BLEND)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::COMPUTELIGHT_BLEND)].clear();


	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Render_SSAO()
{
	m_pDeviceContext->RSSetViewports(1, &m_halfViewport);

	//========================
	// SSAO Gen -> AO_Ping
	//========================
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::SSAO_Gen, true, false)))
		goto FAIL;

	if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
		goto FAIL;

	if (FAILED(m_pShader->Bind_SRV(EFXSRV::SSAONoise, m_pSSAONoiseSRV)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Normal, m_pShader)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		goto FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::SSAO_GEN));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		goto FAIL;

	//========================
	// BLUR H -> AO_Pong
	//========================
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::SSAO_BlurH, true, false)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SSAO_Ping, m_pShader)))
		goto FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::SSAO_BLURH));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		goto FAIL;

	//========================
	// BLUR V -> AO_Ping
	//========================
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::SSAO_BlurV, true, false)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SSAO_Pong, m_pShader)))
		goto FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::SSAO_BLURV));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		goto FAIL;


	//========================
	// Upsample -> AO_Full
	//========================
	m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::SSAO_Upsample, true, false)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SSAO_Ping, m_pShader)))
		goto FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		goto FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::SSAO_UPSAMPLE));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		goto FAIL;

	return S_OK;
FAIL:
	m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
	return E_FAIL;
}

HRESULT CRender_Manager::Render_Lights()
{
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::LightAcc)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Normal, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SpecularMask, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Diffuse, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SSAO_Full, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Render_Environment()
{
	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::ENVIRONMENT)])
	{
		if (FAILED(pElement->Render()))
		{
			Safe_Release(pElement);
			m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::ENVIRONMENT)].clear();
			return E_FAIL;
		}

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::ENVIRONMENT)].clear();
	return S_OK;
}

HRESULT CRender_Manager::Render_Outline()
{
	m_pShader->Bind_TransformData(m_matWorld_RT);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::ObjectInfo, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		return E_FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::OUTLINE));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRender_Manager::Render_CombinedHDR()
{
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::CombineHDR)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Diffuse, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Shade, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Specular, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		return E_FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::COMBINED));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Render_BlendUI()
{
	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::BLENDUI)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::BLENDUI)].clear();

	return S_OK;
}

HRESULT CRender_Manager::Render_UI()
{
	Sort_UI();
	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::UI)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::UI)].clear();

	return S_OK;
}

void CRender_Manager::Sort_UI()
{
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::UI)].sort(
		[](const CGameObject* a, const CGameObject* b)
		{
			return static_cast<const CUIObject*>(a)->Get_PosZ() > static_cast<const CUIObject*>(b)->Get_PosZ(); /* 매 프레임 Static Cast / 나중에 수정해야될 때 수정하기 */
		});
}

array<Vec4, SSAO_KERNAL> CRender_Manager::Build_SSAO_Kernal16()
{
	array<Vec4, SSAO_KERNAL> arrKernel;
	for (_uint i = 0; i < SSAO_KERNAL; ++i)
	{
		Vec3 vRand
		{
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			m_pGameInstance->Rand_Float(0.f, 1.f)
		};
		vRand.Normalize();

		_float fScale = m_pGameInstance->Rand_Float(0.f, 1.f);
		_float fT = (_float)i / 15.f;
		_float fBias = 0.1f;
		_float fGain = 1.0f;
		_float fLerpScale = std::_Linear_for_lerp(fBias, fGain, fT * fT);
		vRand *= fScale * fLerpScale;

		arrKernel[i] = Vec4(vRand.x, vRand.y, vRand.z, 0.f);
	}

	return arrKernel;
}

HRESULT CRender_Manager::Create_SSAO_NoiseSRV()
{
	Safe_Release(m_pSSAONoiseSRV);

	// 4x4 RGBA float
	Vec4 noiseData[16]{Vec4::Zero};

	for (_int i = 0; i < 16; ++i)
	{
		Vec3 vRand
		{
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			0.f
		};

		vRand.Normalize();
		noiseData[i] = Vec4(vRand.x, vRand.y, 0.f, 0.f);
	}

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = 4;
	desc.Height = 4;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = noiseData;
	init.SysMemPitch = sizeof(Vec4) * 4;

	ID3D11Texture2D* pTexture{ nullptr };
	if (FAILED(m_pDevice->CreateTexture2D(&desc, &init, &pTexture)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, &m_pSSAONoiseSRV)))
	{
		Safe_Release(pTexture);
		return E_FAIL;
	}

	Safe_Release(pTexture);
	return S_OK;
}

HRESULT CRender_Manager::Set_ConstantBuffer()
{
	m_pCB_SSAOkernel = CConstant_Buffer<SHADER_SSAOKERNEL_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_SSAOparam = CConstant_Buffer<SHADER_SSAOPARAM_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_HDRparam = CConstant_Buffer<SHADER_HDRPARAM_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_Bloomparam = CConstant_Buffer<SHADER_BLOOMPARAM_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_Outlineparam = CConstant_Buffer<SHADER_OUTLINE_DESC>::Create(m_pDevice, m_pDeviceContext);
	if (m_pCB_SSAOkernel == nullptr || m_pCB_SSAOparam == nullptr || m_pCB_HDRparam == nullptr ||
		m_pCB_Bloomparam == nullptr || m_pCB_Outlineparam == nullptr)
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::SSAOkernal, m_pCB_SSAOkernel->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::SSAOparam, m_pCB_SSAOparam->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::HDRparam, m_pCB_HDRparam->Get_Buffer())))
		return E_FAIL;

	if(FAILED(m_pShader->Set_ConstantBuffer(EFXCB::Bloomparam, m_pCB_Bloomparam->Get_Buffer())))
		return E_FAIL;

	if(FAILED(m_pShader->Set_ConstantBuffer(EFXCB::Outlineparam, m_pCB_Outlineparam->Get_Buffer())))
		return E_FAIL;

	return S_OK;
}

CRender_Manager* CRender_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRender_Manager* pInstance = new CRender_Manager(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CRender_Manager::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRender_Manager::Free()
{
#ifdef _DEBUG
	for (auto& pDebugCom : m_debugComponents)
		Safe_Release(pDebugCom);
	m_debugComponents.clear();
#endif
	for (auto& RenderObjects : m_renderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	// === WBOIT 전용 blend 캐싱 변수 === 
	Safe_Release(m_pWBOIT_AccumulateBS);
	Safe_Release(m_pAlphaBlendBS);
	//
	Safe_Release(m_pLUTTexture);
	Safe_Release(m_pSSAONoiseSRV);
	Safe_Release(m_pCB_Outlineparam);
	Safe_Release(m_pCB_Bloomparam);
	Safe_Release(m_pCB_HDRparam);
	Safe_Release(m_pCB_SSAOkernel);
	Safe_Release(m_pCB_SSAOparam);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}

#ifdef _DEBUG

HRESULT CRender_Manager::Push_DebugComponent(CComponent* pComponent)
{
	m_debugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);
	return S_OK;
}

HRESULT CRender_Manager::Commit_SSAOParam()
{
	m_tSSAOparamDesc.vInvSize = { 1.f / m_halfViewport.Width, 1.f / m_halfViewport.Height };
	return m_pCB_SSAOparam ? m_pCB_SSAOparam->Copy_Data(m_tSSAOparamDesc) : E_FAIL;
}

HRESULT CRender_Manager::Commit_HDRParam()
{
	return m_pCB_HDRparam ? m_pCB_HDRparam->Copy_Data(m_tHDRparamDesc) : E_FAIL;
}

HRESULT CRender_Manager::Commit_BloomParam()
{
	m_tBloomparamDesc.vInvSize = { 1.f / m_halfViewport.Width, 1.f / m_halfViewport.Height };
	return m_pCB_Bloomparam ? m_pCB_Bloomparam->Copy_Data(m_tBloomparamDesc) : E_FAIL;
}

HRESULT CRender_Manager::Commit_OutlineParam()
{
	m_tOutlineparamDesc.vInvSize = { 1.f / m_defaultViewport.Width, 1.f / m_defaultViewport.Height };
	return m_pCB_Outlineparam ? m_pCB_Outlineparam->Copy_Data(m_tOutlineparamDesc) : E_FAIL;
}

HRESULT CRender_Manager::Commit_AllPostParams()
{
	if (FAILED(Commit_SSAOParam()))    return E_FAIL;
	if (FAILED(Commit_HDRParam()))     return E_FAIL;
	if (FAILED(Commit_BloomParam()))   return E_FAIL;
	if (FAILED(Commit_OutlineParam())) return E_FAIL;
	return S_OK;
}

HRESULT CRender_Manager::Ready_Debug()
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(ERenderTarget::Normal, 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRender_Manager::Render_Debug()
{
	for (auto& pDebugCom : m_debugComponents)
	{
		if (pDebugCom)
		{
			pDebugCom->Render();
			Safe_Release(pDebugCom);
		}
	}
	m_debugComponents.clear();
	return S_OK;
}
#endif