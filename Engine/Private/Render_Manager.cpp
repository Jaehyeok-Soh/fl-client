#include "Engine_pch.h"
#include "Render_Manager.h"
#include "Constant_Buffer.h"
#include "TextureBase.h"
#include "GameObject.h"
#include "Camera.h"
#include "VIBuffer_Rect_Tex.h"
#include "ThreadPool.h"
#include "Shader.h"
#include "Bounds.h"
#include "CameraMan.h"
#include "Light.h"
#include "RenderTarget.h"
#include "RenderTargetArray.h"
#include "Octree_Manager.h"
#include "EngineConsole.h"
#include "UIObject.h"
#include "Texture.h"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"
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
	if (FAILED(Ready_RT()))
		return E_FAIL;

	if (FAILED(Ready_RTArray()))
		return E_FAIL;

	if (FAILED(Ready_MRT()))
		return E_FAIL;	

	if (FAILED(Create_ShadowResource()))
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

HRESULT CRender_Manager::Set_CascadeShadowConstantBuffer(CShader* pShader)
{
	if (pShader == nullptr)
		return E_FAIL;

	return pShader->Set_ConstantBuffer(EFXCB::Cascadeparam, m_pCB_CascadeShadow->Get_Buffer());
}

HRESULT CRender_Manager::Set_BakedShadowConstantBuffer(CShader* pShader)
{
	if (pShader == nullptr)
		return E_FAIL;

	return pShader->Set_ConstantBuffer(EFXCB::BakedShadowparam, m_pCB_BakedShadow->Get_Buffer());
}

HRESULT CRender_Manager::Initialize_BakedShadowSections(BoundingBox* pRootBox)
{
	if (pRootBox == nullptr)
		return E_FAIL;

	m_vecBakedSection.clear();
	m_vecBakedSectionResults.clear();
	m_tActiveBakedSet = {};

	m_iCurrentCenterSectionX = INT_MAX;
	m_iCurrentCenterSectionZ = INT_MAX;

	m_bakedWorldRootBounds = *pRootBox;
	//===========================================================
	// 옥트리에 등록된 Object 기준 그림자 뽑는 녀석들로 RootBox 재형성
	//===========================================================
	//if (FAILED(Create_RootBox(m_bakedWorldRootBounds)))
	//	return E_FAIL;

	Vec3 vCenter = Vec3(
		m_bakedWorldRootBounds.Center.x,
		m_bakedWorldRootBounds.Center.y,
		m_bakedWorldRootBounds.Center.z);
	Vec3 vExtents = Vec3(
		m_bakedWorldRootBounds.Extents.x,
		m_bakedWorldRootBounds.Extents.y,
		m_bakedWorldRootBounds.Extents.z);

	if (vExtents.x <= 0.f || vExtents.y <= 0.f || vExtents.z <= 0.f)
		return E_FAIL;

	Vec3 vMin = vCenter - vExtents;
	Vec3 vMax = vCenter + vExtents;

	m_vBakedSectionOrigin = vMin;
	m_fBakedSectionSizeX = (vMax.x - vMin.x) / BAKED_SECTION_COUNT_X;
	m_fBakedSectionSizeZ = (vMax.z - vMin.z) / BAKED_SECTION_COUNT_Z;

	m_fSectionUpdateHysteresisX = m_fBakedSectionSizeX * 0.3f;
	m_fSectionUpdateHysteresisZ = m_fBakedSectionSizeZ * 0.3f;

	m_bBakedSectionInitialized = false;
	m_bActiveBakedSectionDirty = true;

#ifdef _DEBUG
	{
		string strLog{
			"[BakedRoot] "
			"RootMin(" + std::to_string(vMin.x) + ", " +
						  std::to_string(vMin.y) + ", " +
						  std::to_string(vMin.z) + ") " +
			"RootMax(" + std::to_string(vMax.x) + ", " +
						  std::to_string(vMax.y) + ", " +
						  std::to_string(vMax.z) + ") " +
			"SizeXZ(" + std::to_string(vMax.x - vMin.x) + ", " +
						 std::to_string(vMax.z - vMin.z) + ") " +
			"SectionXZ(" + std::to_string(m_fBakedSectionSizeX) + ", " +
							std::to_string(m_fBakedSectionSizeZ) + ") " +
			"Hysteresis(" + std::to_string(m_fSectionUpdateHysteresisX) + ", "
							+ std::to_string(m_fSectionUpdateHysteresisZ) + ")"
		};
		CLOG_INFO(strLog);
	}
#endif
	return S_OK;
}

HRESULT CRender_Manager::Build_BakedShadowSections()
{
	if (FAILED(Build_BakedShadowSectionJobs()))
		return E_FAIL;

	if (FAILED(Execute_BakedShadowSectionJobs()))
		return E_FAIL;

	m_bBakedSectionInitialized = true;
	m_bActiveBakedSectionDirty = true;

#ifdef _DEBUG
	if (FAILED(Create_BakedShadowSliceSRV()))
		return E_FAIL;

#endif
	return S_OK;
}

HRESULT CRender_Manager::Set_ShaderResources()
{
	{
		if (!(m_pVIBuffer = CVIBuffer_Rect_Tex::Create(m_pDevice, m_pDeviceContext, nullptr)))
			return E_FAIL;
	}
	// For. MainShader
	{
		CShader::SHADER_ORIGIN_DESC desc = {};
		desc.pShaderFilePath = L"../../Shaders/Shader_Deffered.hlsl";
		desc.eLayout = EVtxLayout::VTXPOSTEX;
		if (!(m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, &desc)))
			return E_FAIL;
	}
	// For. FogShader
	{
		CShader::SHADER_ORIGIN_DESC desc = {};
		desc.pShaderFilePath = L"../../Shaders/Shader_Fog.hlsl";
		desc.eLayout = EVtxLayout::VTXPOSTEX;
		if (!(m_pFogShader = CShader::Create(m_pDevice, m_pDeviceContext, &desc)))
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

	if (FAILED(Create_Perlin_NoiseSRV()))
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
	// FogDesc
	{
		m_tFogDesc.vColor = Vec4(0.35f, 0.35f, 0.35f, 1.f);
		m_tFogDesc.vHighColor = Vec4(0.31f, 0.31f, 0.31f, 1.f);

		// Distance
		m_tFogDesc.fFogStart = 100.f;     // 20m부터 시작
		m_tFogDesc.fFogEnd = 430.f;     // 80m에서 최대
		m_tFogDesc.fFogDensity = 0.f;      // linear (0이면 linear)
		m_tFogDesc.fFogMaxOpacity = 0.2f;    // 최대 55% - 멀어도 어느정도 보임

		// Height
		m_tFogDesc.fFogBaseHeight = -9.f;     // 지면 약간 아래
		m_tFogDesc.fFogHeightFalloff = 0.08f;    // 천천히 감소 - 낮은 곳에 안개 깔림
		m_tFogDesc.fFogHeightDensity = 0.015f;   // 옅게

		// Noise
		m_tFogDesc.fFogNoiseScale = 0.15f;    // 미세한 변동만
		m_tFogDesc.fFogNoiseSpeed = 0.2f;     // 느리게 흐름

		if (FAILED(m_pCB_Fog->Copy_Data(m_tFogDesc)))
			return E_FAIL;
	}
	// ToonDesc
	{
		m_tToonparamDesc.fWrap = 0.65f;
		m_tToonparamDesc.fShadowMid = 0.58f;
		m_tToonparamDesc.fShadowSoftness = 0.15f;
		m_tToonparamDesc.fShadowStrength = 0.45f;
		m_tToonparamDesc.fDiffuseStrength = 1.0f;
		m_tToonparamDesc.fRimThreshold = 0.65f;
		m_tToonparamDesc.fRimSoftness = 0.125f;
		m_tToonparamDesc.fRimStrength = 0.7f;
		if (FAILED(m_pCB_Toonparam->Copy_Data(m_tToonparamDesc)))
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

	if (FAILED(Render_CascadeShadow()))
		return E_FAIL;

	if (FAILED(Render_NoneBlend()))
		return E_FAIL;

	if (FAILED(Render_ComputeLight_Blend()))
		return E_FAIL;

	m_pGameInstance->Setup_UIViewProj_ToCBuffer();

	if (FAILED(Render_SSAO()))
		return E_FAIL;

	if (FAILED(Update_ActiveBakedSections()))
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

			if (FAILED(Render_Fog()))
				return E_FAIL;

			m_pGameInstance->Setup_ViewProj_ToCBuffer();

			if (FAILED(m_pGameInstance->End_MRT())) return E_FAIL;
		}

		// WBOIT 정보 수집
		{
			// Accum:0, Reveal:1
			if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::OIT_Render, true, true)))
				return E_FAIL;

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

			// 가중치 Blend 기법
			if (FAILED(Render_WBOIT())) return E_FAIL;

			m_pGameInstance->Setup_ViewProj_ToCBuffer();
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

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::ObjectInfo, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Diffuse, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SSAO_Full, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Cascade_0, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Cascade_1, m_pShader)))
		return E_FAIL;

	if (FAILED(Bind_ActiveBakedSections()))
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

HRESULT CRender_Manager::Render_Fog()
{
	if (FAILED(m_pFogShader->Bind_TransformData(m_matWorld_RT)))
		return E_FAIL;

	if(FAILED(m_pFogShader->Bind_SRV(EFXSRV::PerlinNoise, m_pPerlinNoiseSRV)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pFogShader)))
		return E_FAIL;

	m_pFogShader->Set_Pass(0);
	m_pFogShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();
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

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Emissive, m_pShader)))
		return E_FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::COMBINED));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Render_CascadeShadow()
{
	if (FAILED(Compute_ShadowCascade()))
		return E_FAIL;

	m_pDeviceContext->RSSetViewports(1, &m_tShadowViewport);

	for (_uint i = 0; i < SHADOW_CASCADE_COUNT; ++i)
	{
		EMRTLayer eMRT = (i == 0)
			? EMRTLayer::Shadow_Cascade0
			: EMRTLayer::Shadow_Cascade1;

		if(FAILED(m_pGameInstance->Begin_MRT(eMRT, true, m_pShadowDSV)))
			goto FAIL;

		// CascadeIndex
		m_tCascadeShadowDesc.fCascadeIndex = (_float)i;
		if (FAILED(m_pCB_CascadeShadow->Copy_Data(m_tCascadeShadowDesc)))
			goto FAIL;

		// 동적 오브젝터만 렌더
		for (auto& pObject : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::SHADOW_DYNAMIC)])
			pObject->Render_Shadow();

		if (FAILED(m_pGameInstance->End_MRT()))
			goto FAIL;
	}

	m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);

	// Shadow카테고리 안전하게 일괄 정리
	for (auto& pObject : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::SHADOW_DYNAMIC)])
		Safe_Release(pObject);

	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::SHADOW_DYNAMIC)].clear();
	return S_OK;
FAIL:
	m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
	return E_FAIL;	
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

HRESULT CRender_Manager::Create_Perlin_NoiseSRV()
{
	constexpr _uint iSize = 64;
	vector<_float> vecData(iSize * iSize * iSize);

	constexpr _int  iTile = 4;  // 타일링 주기

	for (_uint z = 0; z < iSize; ++z)
	{
		for (_uint y = 0; y < iSize; ++y)
		{
			for (_uint x = 0; x < iSize; ++x)
			{
				_float fx = (_float)x / (_float)iSize * (_float)iTile;
				_float fy = (_float)y / (_float)iSize * (_float)iTile;
				_float fz = (_float)z / (_float)iSize * (_float)iTile;

				// 수동 fBM with wrap
				_float fValue = 0.f;
				_float fAmplitude = 1.f;
				_float fFrequency = 1.f;
				_float fMax = 0.f;

				for (_uint oct = 0; oct < 4; ++oct)
				{
					_int iWrap = iTile * (_int)fFrequency;

					fValue += stb_perlin_noise3_seed(
						fx * fFrequency,
						fy * fFrequency,
						fz * fFrequency,
						iWrap, iWrap, iWrap,   // ★ 각 축 wrap
						0                       // seed
					) * fAmplitude;

					fMax += fAmplitude;
					fAmplitude *= 0.5f;     // persistence
					fFrequency *= 2.f;      // lacunarity
				}

				fValue /= fMax;

				// [-1, 1] → [0, 1]
				vecData[z * iSize * iSize + y * iSize + x] = fValue * 0.5f + 0.5f;
			}
		}
	}

	D3D11_TEXTURE3D_DESC desc{};
	desc.Width = iSize;
	desc.Height = iSize;
	desc.Depth = iSize;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = vecData.data();
	init.SysMemPitch = sizeof(_float) * iSize;
	init.SysMemSlicePitch = sizeof(_float) * iSize * iSize;

	ID3D11Texture3D* pTexture = nullptr;
	if (FAILED(m_pDevice->CreateTexture3D(&desc, &init, &pTexture)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, &m_pPerlinNoiseSRV)))
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
	m_pCB_Fog = CConstant_Buffer<SHADER_FOG_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_Toonparam = CConstant_Buffer<SHADER_TOON_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_CascadeShadow = CConstant_Buffer<SHADER_CASCADE_SHADOW_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_BakedShadow = CConstant_Buffer< SHADER_BAKED_SHADOW_DESC>::Create(m_pDevice, m_pDeviceContext);
	m_pCB_ActiveBakedSections = CConstant_Buffer<SHADER_BAKED_SECTION_DESC>::Create(m_pDevice, m_pDeviceContext);
	if (m_pCB_SSAOkernel == nullptr || m_pCB_SSAOparam == nullptr || m_pCB_HDRparam == nullptr ||
		m_pCB_Bloomparam == nullptr || m_pCB_Outlineparam == nullptr || m_pCB_Fog == nullptr ||
		m_pCB_Toonparam == nullptr || m_pCB_CascadeShadow == nullptr || m_pCB_BakedShadow == nullptr ||
		m_pCB_ActiveBakedSections == nullptr)
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::SSAOkernal, m_pCB_SSAOkernel->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::SSAOparam, m_pCB_SSAOparam->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::HDRparam, m_pCB_HDRparam->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::Bloomparam, m_pCB_Bloomparam->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::Outlineparam, m_pCB_Outlineparam->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::Toonparam, m_pCB_Toonparam->Get_Buffer())))
		return E_FAIL;

	// Fog
	if (FAILED(m_pFogShader->Set_ConstantBuffer(EFXCB::Fogparam, m_pCB_Fog->Get_Buffer())))
		return E_FAIL;

	// Shadow
	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::Cascadeparam, m_pCB_CascadeShadow->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::BakedShadowparam, m_pCB_BakedShadow->Get_Buffer())))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ConstantBuffer(EFXCB::SectionShadowparam, m_pCB_ActiveBakedSections->Get_Buffer())))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Ready_RTArray()
{
	{
		CRenderTargetArray::RENDERTARGET_ARR_DESC desc{};
		desc.ePixelFormat = DXGI_FORMAT_R32_FLOAT;
		desc.iWidth = SHADOW_BAKE_SIZE;
		desc.iHeight = SHADOW_BAKE_SIZE;
		desc.iArraySize = BAKED_SECTION_COUNT_X * BAKED_SECTION_COUNT_Z;
		desc.vClearColor = Vec4::One;

		if (FAILED(m_pGameInstance->Add_RenderTargetArray(ERenderTarget::Shadow_Baked, &desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRender_Manager::Ready_RT()
{
	//========================
	// Viewport Save / Set Half
	//========================
	_uint iViewportsCount = { 1 };
	m_pDeviceContext->RSGetViewports(&iViewportsCount, &m_defaultViewport);
	m_halfViewport = m_defaultViewport;
	m_halfViewport.Width *= 0.5f;
	m_halfViewport.Height *= 0.5f;

	m_matWorld_RT = Matrix::CreateScale(m_defaultViewport.Width, m_defaultViewport.Height, 1.f);

	const _uint& iWidth = (_uint)m_defaultViewport.Width;
	const _uint& iHeight = (_uint)m_defaultViewport.Height;
	const _uint& iHalfWidth = (_uint)m_halfViewport.Width;
	const _uint& iHalfHeight = (_uint)m_halfViewport.Height;


	// For. Target_Diffuse
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
	// For. Target_Emissive
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Emissive, &desc)))
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
	// For. Target_Shadow_Cascade_0
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R32_FLOAT;
		desc.iWidth = SHADOW_MAP_SIZE;
		desc.iHeight = SHADOW_MAP_SIZE;
		desc.vClearColor = Vec4::One;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Cascade_0, &desc)))
			return E_FAIL;
	}

	// For. Target_Shadow_Cascade_1
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R32_FLOAT;
		desc.iWidth = SHADOW_MAP_SIZE;
		desc.iHeight = SHADOW_MAP_SIZE;
		desc.vClearColor = Vec4::One;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Cascade_1, &desc)))
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
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::OIT_Accum, &desc)))
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
	return S_OK;
}

HRESULT CRender_Manager::Ready_MRT()
{
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
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::Emissive)))
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

	// For. MRT_Shadow_Cascade_0
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::Shadow_Cascade0, ERenderTarget::Cascade_0)))
			return E_FAIL;
	}

	// For. MRT_Shadow_Cascade_1
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::Shadow_Cascade1, ERenderTarget::Cascade_1)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CRender_Manager::Ready_Dissolve()
{
	for (auto Dissolve : m_pDissolveTextures)
	{
		Safe_Release(Dissolve);
	}
	m_pDissolveTextures.clear();

	CTextureBase* pTexture = { nullptr };

	// 일반 몬스터용
	{
		if (!(pTexture = m_pGameInstance->Get_Resource<CTextureBase>(L"Texture_T_TurbulenceMm010_Mask")))
		{
			MSG_BOX("Dissolve Texture가 바인딩 되지 않았습니다. - Render Manager - MONSTER");
			Safe_Release(pTexture);
			return E_FAIL;
		}

		else
			m_pDissolveTextures.push_back(pTexture);
	}

	// 보스 몬스터용
	{
		if (!(pTexture = m_pGameInstance->Get_Resource<CTextureBase>(L"Texture_T_TurbulenceBx006_M")))
		{
			MSG_BOX("Dissolve Texture가 바인딩 되지 않았습니다. - Render Manager - BOSS");
			Safe_Release(pTexture);
			return E_FAIL;
		}

		else
			m_pDissolveTextures.push_back(pTexture);
	}

	// 칼
	{
		if (!(pTexture = m_pGameInstance->Get_Resource<CTextureBase>(L"Texture_T_TurbulenceBx004_M")))
		{
			MSG_BOX("Dissolve Texture가 바인딩 되지 않았습니다. - Render Manager - SWORD");
			Safe_Release(pTexture);
			return E_FAIL;
		}

		else
			m_pDissolveTextures.push_back(pTexture);
	}

	// NPC
	{
		if (!(pTexture = m_pGameInstance->Get_Resource<CTextureBase>(L"Texture_T_TurbulenceFz001_Mask")))
		{
			MSG_BOX("Dissolve Texture가 바인딩 되지 않았습니다. - Render Manager - NPC");
			Safe_Release(pTexture);
			return E_FAIL;
		}

		else
			m_pDissolveTextures.push_back(pTexture);
	}

	return S_OK;
}

HRESULT CRender_Manager::Bind_DissolveTexture(CShader* pShader)
{
#define DISSOLVE_MAX 4

	if (pShader == nullptr) return E_FAIL;

	ID3D11ShaderResourceView* pSRVs[DISSOLVE_MAX] = {
	m_pDissolveTextures[0]->Get_SRV(),
	m_pDissolveTextures[1]->Get_SRV(),
	m_pDissolveTextures[2]->Get_SRV(),
	m_pDissolveTextures[3]->Get_SRV(),
	};

	return pShader->Bind_SRVArray(EFXSRV::DISSOLVE, pSRVs, DISSOLVE_MAX);
}

HRESULT CRender_Manager::Create_ShadowResource()
{
	// ViewPort
	{
		::ZeroMemory(&m_tShadowViewport, sizeof(D3D11_VIEWPORT));
		m_tShadowViewport.Width = (_float)SHADOW_MAP_SIZE;
		m_tShadowViewport.Height = (_float)SHADOW_MAP_SIZE;
		m_tShadowViewport.MinDepth = 0.f;
		m_tShadowViewport.MaxDepth = 1.f;

		::ZeroMemory(&m_tBakedShadowViewport, sizeof(D3D11_VIEWPORT));
		m_tBakedShadowViewport.Width = (_float)SHADOW_BAKE_SIZE;
		m_tBakedShadowViewport.Height = (_float)SHADOW_BAKE_SIZE;
		m_tBakedShadowViewport.MinDepth = 0.f;
		m_tBakedShadowViewport.MaxDepth = 1.f;
	}

	// DSV
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = SHADOW_MAP_SIZE;
		desc.Height = SHADOW_MAP_SIZE;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pShadowDSTexture)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateDepthStencilView(m_pShadowDSTexture, nullptr, &m_pShadowDSV)))
			return E_FAIL;
	}

	// DSV
	{
		D3D11_TEXTURE2D_DESC dsDesc = {};
		dsDesc.Width = SHADOW_BAKE_SIZE;
		dsDesc.Height = SHADOW_BAKE_SIZE;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* pDSTex = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&dsDesc, nullptr, &pDSTex)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateDepthStencilView(pDSTex, nullptr, &m_pBakedShadowDSV)))
		{
			Safe_Release(pDSTex);
			return E_FAIL;
		}

		Safe_Release(pDSTex);
	}
	return S_OK;
}

HRESULT CRender_Manager::Compute_ShadowCascade()
{
	// 매 프레임 시행
	CCameraMan* pCamera = m_pGameInstance->Get_MainCamera();
	
	// MainCamera 세팅안됬을때 스킵
	if (pCamera == nullptr)
		return S_OK;

	// MainCamera View, Proj
	CCamera* pCameraComponent = pCamera->Get_Component<CCamera>();
	Matrix matView = pCameraComponent->Get_ViewMatrix();
	Matrix matProj = pCameraComponent->Get_ProjectionMatrix();
	_float fNear = pCameraComponent->Get_Near();

	CLight* pDirLight = m_pGameInstance->Get_Light(LIGHT_TYPE::DIRECTIONAL);
	// DirLight 세팅안됬을때 스킵
	if (pDirLight == nullptr)
		return S_OK;
	Vec3 vLightDir = pDirLight->Get_LightDesc().vDirection;
	vLightDir.Normalize();

	_float fSplits[3] =
	{
		fNear,
		m_tCascadeShadowDesc.fCascadeEnd0,
		m_tCascadeShadowDesc.fCascadeEnd1
	};

	for (_uint i = 0; i < SHADOW_CASCADE_COUNT; ++i)
	{
		//===========================================
		// Cascade View Frustum 코너 (NDC에서 World로)
		//===========================================
		_float fSplitNear = fSplits[i];
		_float fSplitFar = fSplits[i + 1];

		// NDC z를 구하기 위해 임시 Proj
		_float fNdcNear = (fSplitNear * matProj._33 + matProj._43) / fSplitNear;
		_float fNdcFar = (fSplitFar * matProj._33 + matProj._43) / fSplitFar;

		// NDC 8코너
		Vec3 corners[8] =
		{
			{ -1, -1, fNdcNear }, {  1, -1, fNdcNear },
			{ -1,  1, fNdcNear }, {  1,  1, fNdcNear },
			{ -1, -1, fNdcFar  }, {  1, -1, fNdcFar  },
			{ -1,  1, fNdcFar  }, {  1,  1, fNdcFar  },
		};

		Matrix matInvVP = (matView * matProj).Invert();

		Vec3 vCenter = Vec3::Zero;
		for (_int c = 0; c < 8; ++c)
		{
			Vec4 vWorld = Vec4::Transform(Vec4(corners[c].x, corners[c].y, corners[c].z, 1.f), matInvVP);
			corners[c] = Vec3(vWorld.x, vWorld.y, vWorld.z) / vWorld.w;
			vCenter += corners[c];
		}
		vCenter /= 8.f;

		//================
		// Light View 행렬
		//================
		Matrix matLightView = ::XMMatrixLookAtLH(
			vCenter - vLightDir * 50.f,  // 뒤로 50m 물러남
			vCenter,
			Vec3::Up
		);

		//=================================
		// Light Space AABB에서 Ortho Proj로
		//=================================
		Vec3 vMin(FLT_MAX), vMax(-FLT_MAX);
		for (_int c = 0; c < 8; ++c)
		{
			Vec3 vLS = Vec3::Transform(corners[c], matLightView);
			vMin = Vec3::Min(vMin, vLS);
			vMax = Vec3::Max(vMax, vLS);
		}

		// 약간 여유
		_float fZPad = 20.f;
		vMin.z -= fZPad;
		vMax.z += fZPad;

		Matrix matLightProj = XMMatrixOrthographicOffCenterLH(
			vMin.x, vMax.x, vMin.y, vMax.y, vMin.z, vMax.z
		);

		m_tCascadeShadowDesc.matLightVP[i] = matLightView * matLightProj;
	}

	m_tCascadeShadowDesc.vShadowMapInvSize = { 1.f / SHADOW_MAP_SIZE, 1.f / SHADOW_MAP_SIZE };
	return S_OK;
}

HRESULT CRender_Manager::Bind_ActiveBakedSections()
{
	if (m_bBakedSectionInitialized == false)
		return S_OK;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Shadow_Baked, m_pShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Create_RootBox(OUT BoundingBox &outRootBox)
{
	vector<CGameObject*> vecStatics;
	m_pGameInstance->m_pOctree_Manager->Query_All(RENDER_CATEGORY::NONEBLEND, vecStatics);
	if (vecStatics.size() <= 0)
		return E_FAIL;

	vector<CGameObject*> vecFiltered;
	vecFiltered.reserve(vecStatics.size());

	Vec3 vMinWS(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 vMaxWS(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (auto& pObj : vecStatics)
	{
		if (pObj->Is_BakedShadow() == false)
			continue;

		CBounds* pBounds = pObj->Get_Component<CBounds>();
		if (pBounds == nullptr || pBounds->Get_WolrdAABB() == nullptr)
			continue;

		vecFiltered.push_back(pObj);

		const BoundingBox& AABB = *pBounds->Get_WolrdAABB();
		Vec3 vObjMin = AABB.Center - AABB.Extents;
		Vec3 vObjMax = AABB.Center + AABB.Extents;

		vMinWS = Vec3::Min(vMinWS, vObjMin);
		vMaxWS = Vec3::Max(vMaxWS, vObjMax);
	}

	if (vecFiltered.empty() == true)
		return E_FAIL;

	constexpr _float fMargin = 1.f;
	vMinWS -= Vec3(fMargin, fMargin, fMargin);
	vMaxWS += Vec3(fMargin, fMargin, fMargin);

	outRootBox = BoundingBox(
		(vMinWS + vMaxWS) * 0.5f,
		(vMaxWS - vMinWS) * 0.5f
	);
	return S_OK;
}

_uint CRender_Manager::Compute_BakedSectionIndex(_int iSectionX, _int iSectionZ) const
{
	return static_cast<_uint>(iSectionZ * BAKED_SECTION_COUNT_X + iSectionX);
}

BoundingBox CRender_Manager::Compute_BakedSectionBounds(_int iSectionX, _int iSectionZ) const
{
	_float fMinY = m_bakedWorldRootBounds.Center.y - m_bakedWorldRootBounds.Extents.y;
	_float fMaxY = m_bakedWorldRootBounds.Center.y + m_bakedWorldRootBounds.Extents.y;

	Vec3 vMin = {
		m_vBakedSectionOrigin.x + iSectionX * m_fBakedSectionSizeX,
		fMinY,
		m_vBakedSectionOrigin.z + iSectionZ * m_fBakedSectionSizeZ
	};

	Vec3 vMax = {
		m_vBakedSectionOrigin.x + (iSectionX + 1) * m_fBakedSectionSizeX,
		fMaxY,
		m_vBakedSectionOrigin.z + (iSectionZ + 1) * m_fBakedSectionSizeZ
	};

	return BoundingBox((vMin + vMax) * 0.5f, (vMax - vMin) * 0.5f);
}

HRESULT CRender_Manager::Build_BakedShadowSectionJobs()
{
	m_vecBakedSectionResults.clear();

	vector<CGameObject*> vecStatics;
	m_pGameInstance->m_pOctree_Manager->Query_All(RENDER_CATEGORY::NONEBLEND, vecStatics);

	vector<CGameObject*> vecFiltered;
	vecFiltered.reserve(vecStatics.size());

	for (auto* pObj : vecStatics)
	{
		if (pObj == nullptr)
			continue;

		if (pObj->Is_BakedShadow() == false)
			continue;

		CBounds* pBounds = pObj->Get_Component<CBounds>();
		if (pBounds == nullptr || pBounds->Get_WolrdAABB() == nullptr)
			continue;

		vecFiltered.push_back(pObj);
	}

	CLight* pDirLight = m_pGameInstance->Get_Light(LIGHT_TYPE::DIRECTIONAL);
	if (pDirLight == nullptr)
		return E_FAIL;

	Vec3 vLightDir = pDirLight->Get_LightDesc().vDirection;
	vLightDir.Normalize();

	vector<future<BAKED_SECTION_BUILD_RESULT>> vecFutures;
	vecFutures.reserve(BAKED_SECTION_COUNT_X * BAKED_SECTION_COUNT_Z);

	for (_int z = 0; z < BAKED_SECTION_COUNT_Z; ++z)
	{
		for (_int x = 0; x < BAKED_SECTION_COUNT_X; ++x)
		{
			BAKED_SECTION_BUILD_INPUT input = {};
			input.iSectionX = x;
			input.iSectionZ = z;
			input.sectionBounds = Compute_BakedSectionBounds(x, z);
			input.vLightDir = vLightDir;
			input.pStaticCasters = &vecFiltered;
			vecFutures.push_back(
				m_pGameInstance->m_pThreadPool->AddTask(
					[this](BAKED_SECTION_BUILD_INPUT input)
					{
						return this->Build_BakedSection(input);
					},
					input
				)
			);
		}
	}

	m_vecBakedSectionResults.reserve(vecFutures.size());

	for (auto& fut : vecFutures)
	{
		BAKED_SECTION_BUILD_RESULT tResult = fut.get();
		if (tResult.bValid)
			m_vecBakedSectionResults.push_back(std::move(tResult));
	}

	return S_OK;
}

BAKED_SECTION_BUILD_RESULT CRender_Manager::Build_BakedSection(const BAKED_SECTION_BUILD_INPUT& input)
{
	BAKED_SECTION_BUILD_RESULT tOut = {};
	tOut.iSectionX = input.iSectionX;
	tOut.iSectionZ = input.iSectionZ;
	tOut.sectionBounds = input.sectionBounds;

	if (input.pStaticCasters == nullptr || input.pStaticCasters->empty())
	{
#if _DEBUG
		string strLog{
			"[BakedSection][Skip] "
			"Grid(" + std::to_string(input.iSectionX) + ", " + std::to_string(input.iSectionZ) + ") " +
			"Reason(EmptyStaticCasters)"
		};
		CLOG_INFO(strLog);
#endif
		return tOut;
	}

	// ===========================================================
	// 0) section footprint는 유지
	//    receiver fit은 XZ는 section 그대로, Y만 타이트하게 계산
	// ===========================================================
	const _float fSectionMinX = input.sectionBounds.Center.x - input.sectionBounds.Extents.x;
	const _float fSectionMaxX = input.sectionBounds.Center.x + input.sectionBounds.Extents.x;
	const _float fSectionMinY = input.sectionBounds.Center.y - input.sectionBounds.Extents.y;
	const _float fSectionMaxY = input.sectionBounds.Center.y + input.sectionBounds.Extents.y;
	const _float fSectionMinZ = input.sectionBounds.Center.z - input.sectionBounds.Extents.z;
	const _float fSectionMaxZ = input.sectionBounds.Center.z + input.sectionBounds.Extents.z;

	_float fReceiverMinY = FLT_MAX;
	_float fReceiverMaxY = -FLT_MAX;
	_bool  bHasReceiver = false;
	_uint  iReceiverCount = 0;

	// receiver 후보:
	// "현재 section footprint(XZ)에 걸치고, section의 Y band 안에도 실제로 걸치는 baked static"
	for (auto* pObj : *input.pStaticCasters)
	{
		if (pObj == nullptr)
			continue;

		CBounds* pBounds = pObj->Get_Component<CBounds>();
		if (pBounds == nullptr || pBounds->Get_WolrdAABB() == nullptr)
			continue;

		const BoundingBox& AABB = *pBounds->Get_WolrdAABB();

		Vec3 vObjMin = AABB.Center - AABB.Extents;
		Vec3 vObjMax = AABB.Center + AABB.Extents;

		const _bool bOverlapX = !(vObjMax.x < fSectionMinX || vObjMin.x > fSectionMaxX);
		const _bool bOverlapZ = !(vObjMax.z < fSectionMinZ || vObjMin.z > fSectionMaxZ);

		if (bOverlapX == false || bOverlapZ == false)
			continue;

		// section의 원래 높이 band 안으로 Y를 clip
		const _float fClampedMinY = (std::max)(vObjMin.y, fSectionMinY);
		const _float fClampedMaxY = (std::min)(vObjMax.y, fSectionMaxY);

		if (fClampedMinY > fClampedMaxY)
			continue;

		fReceiverMinY = (std::min)(fReceiverMinY, fClampedMinY);
		fReceiverMaxY = (std::max)(fReceiverMaxY, fClampedMaxY);
		bHasReceiver = true;
		++iReceiverCount;
	}

	// fallback: receiver를 못 찾으면 기존 sectionBounds 사용
	BoundingBox receiverFitBounds = input.sectionBounds;

	if (bHasReceiver)
	{
		constexpr _float fReceiverPadY = 2.f;

		Vec3 vFitMin(
			fSectionMinX,
			fReceiverMinY - fReceiverPadY,
			fSectionMinZ
		);

		Vec3 vFitMax(
			fSectionMaxX,
			fReceiverMaxY + fReceiverPadY,
			fSectionMaxZ
		);

		receiverFitBounds = BoundingBox(
			(vFitMin + vFitMax) * 0.5f,
			(vFitMax - vFitMin) * 0.5f
		);
	}

	tOut.receiverBounds = receiverFitBounds;

	// ===============================================
	// Receiver = receiverFitBounds 기준으로 LightView 생성
	// XZ footprint는 section 그대로, Y만 타이트하게
	// ===============================================
	Vec3 vReceiverCenter = Vec3(
		receiverFitBounds.Center.x,
		receiverFitBounds.Center.y,
		receiverFitBounds.Center.z
	);

	Vec3 vReceiverExtents = Vec3(
		receiverFitBounds.Extents.x,
		receiverFitBounds.Extents.y,
		receiverFitBounds.Extents.z
	);

	Vec3 vUp = Vec3::Up;
	if (fabs(input.vLightDir.Dot(vUp)) > 0.98f)
		vUp = Vec3::Backward;

	_float fViewBackDist = sqrtf(
		vReceiverExtents.x * vReceiverExtents.x +
		vReceiverExtents.z * vReceiverExtents.z
	) * 2.f;

	Matrix matLightView = ::XMMatrixLookAtLH(
		vReceiverCenter - input.vLightDir * fViewBackDist,
		vReceiverCenter,
		vUp
	);

	// =========================
	// receiver LS 범위 계산
	// XY는 여기서 확정
	// =========================
	Vec3 vReceiverCorners[8];
	receiverFitBounds.GetCorners(vReceiverCorners);

	Vec3 vReceiverLSMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 vReceiverLSMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (_int i = 0; i < 8; ++i)
	{
		Vec3 vLS = Vec3::Transform(vReceiverCorners[i], matLightView);
		vReceiverLSMin = Vec3::Min(vReceiverLSMin, vLS);
		vReceiverLSMax = Vec3::Max(vReceiverLSMax, vLS);
	}

	const _float fPadXY = 0.f;
	_float minX = vReceiverLSMin.x - fPadXY;
	_float maxX = vReceiverLSMax.x + fPadXY;
	_float minY = vReceiverLSMin.y - fPadXY;
	_float maxY = vReceiverLSMax.y + fPadXY;

	// ===========================================================
	// caster query 범위
	// world-space broad phase
	// ===========================================================
	Vec3 vCasterQueryExtents = Vec3(
		input.sectionBounds.Extents.x + m_fBakedSectionSizeX * 0.15f,
		input.sectionBounds.Extents.y + 5.f,
		input.sectionBounds.Extents.z + m_fBakedSectionSizeZ * 0.15f
	);

	BoundingBox tCasterQueryBounds(
		input.sectionBounds.Center,
		vCasterQueryExtents
	);

	Vec3 vCasterUnionMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 vCasterUnionMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	Vec3 vCasterLSMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 vCasterLSMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	_bool bHasCaster = false;

	_uint iBroadPhaseHitCount = 0;
	_uint iAcceptedCasterCount = 0;

	// ===========================================================
	// caster Z slab 제한
	// - receiver와 너무 멀리 떨어진 caster는 제외
	// - accepted caster의 Z union도 clamp
	// ===========================================================
	const _float fCasterPadFront = 120.f; // light 진행 방향 앞쪽 여유
	const _float fCasterPadBack = 40.f;  // 뒤쪽 여유

	const _float fReceiverMinZ = vReceiverLSMin.z;
	const _float fReceiverMaxZ = vReceiverLSMax.z;

	for (auto* pObj : *input.pStaticCasters)
	{
		if (pObj == nullptr)
			continue;

		CBounds* pBounds = pObj->Get_Component<CBounds>();
		if (pBounds == nullptr || pBounds->Get_WolrdAABB() == nullptr)
			continue;

		const BoundingBox& AABB = *pBounds->Get_WolrdAABB();

		// 1) broad-phase
		if (AABB.Intersects(tCasterQueryBounds) == false)
			continue;

		++iBroadPhaseHitCount;

		// 2) light-space narrow phase
		Vec3 vObjLSMin(FLT_MAX, FLT_MAX, FLT_MAX);
		Vec3 vObjLSMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		Vec3 vCasterCorners[8];
		AABB.GetCorners(vCasterCorners);

		for (_int i = 0; i < 8; ++i)
		{
			Vec3 vLS = Vec3::Transform(vCasterCorners[i], matLightView);
			vObjLSMin = Vec3::Min(vObjLSMin, vLS);
			vObjLSMax = Vec3::Max(vObjLSMax, vLS);
		}

		const _bool bOverlapX = !(vObjLSMax.x < minX || vObjLSMin.x > maxX);
		const _bool bOverlapY = !(vObjLSMax.y < minY || vObjLSMin.y > maxY);
		const _bool bOverlapZ =
			!(vObjLSMax.z < (fReceiverMinZ - fCasterPadBack) ||
				vObjLSMin.z >(fReceiverMaxZ + fCasterPadFront));

		if (bOverlapX == false || bOverlapY == false || bOverlapZ == false)
			continue;

		// 여기까지 와야 실제로 이 section receiver에 영향 주는 caster
		++iAcceptedCasterCount;
		tOut.vecCasters.push_back(pObj);

		Vec3 vObjMin = AABB.Center - AABB.Extents;
		Vec3 vObjMax = AABB.Center + AABB.Extents;

		vCasterUnionMin = Vec3::Min(vCasterUnionMin, vObjMin);
		vCasterUnionMax = Vec3::Max(vCasterUnionMax, vObjMax);

		// accepted caster의 light-space Z도 slab로 clamp
		Vec3 vObjLSMinClamped = vObjLSMin;
		Vec3 vObjLSMaxClamped = vObjLSMax;

		vObjLSMinClamped.z = (std::max)(vObjLSMinClamped.z, fReceiverMinZ - fCasterPadBack);
		vObjLSMaxClamped.z = (std::min)(vObjLSMaxClamped.z, fReceiverMaxZ + fCasterPadFront);

		vCasterLSMin = Vec3::Min(vCasterLSMin, vObjLSMinClamped);
		vCasterLSMax = Vec3::Max(vCasterLSMax, vObjLSMaxClamped);

		bHasCaster = true;
	}

	if (bHasCaster == false)
	{
#if _DEBUG
		string strLog{
			"[BakedSection][Skip] "
			"Grid(" + std::to_string(input.iSectionX) + ", " + std::to_string(input.iSectionZ) + ") " +
			"Reason(NoValidCasterInQueryBounds)"
		};
		CLOG_INFO(strLog);
#endif
		return tOut;
	}

	Vec3 vSecMin = Vec3(
		fSectionMinX,
		vCasterUnionMin.y,
		fSectionMinZ
	);

	Vec3 vSecMax = Vec3(
		fSectionMaxX,
		vCasterUnionMax.y,
		fSectionMaxZ
	);

	tOut.casterBounds = BoundingBox(
		(vSecMin + vSecMax) * 0.5f,
		(vSecMax - vSecMin) * 0.5f
	);

	// ===========================================================
	// Z는 receiver + clamped caster 기준으로 가변
	// ===========================================================
	const _float fPadZ = 3.f;

	_float minZ = (std::min)(vReceiverLSMin.z, vCasterLSMin.z) - fPadZ;
	_float maxZ = (std::max)(vReceiverLSMax.z, vCasterLSMax.z) + fPadZ;

	Matrix matLightProj = ::XMMatrixOrthographicOffCenterLH(
		minX, maxX,
		minY, maxY,
		minZ, maxZ
	);

	tOut.matLightVP = matLightView * matLightProj;
	tOut.vShadowParams = Vec4(
		0.0006f,
		0.6f,
		1.f / SHADOW_BAKE_SIZE,
		1.f / SHADOW_BAKE_SIZE
	);

	tOut.bValid = true;

#if _DEBUG
	{
		_float fOrthoWidth = maxX - minX;
		_float fOrthoHeight = maxY - minY;
		_float fOrthoDepth = maxZ - minZ;

		_float fTexelWorldX = fOrthoWidth / (_float)SHADOW_BAKE_SIZE;
		_float fTexelWorldY = fOrthoHeight / (_float)SHADOW_BAKE_SIZE;

		string strLog{
			"[BakedSection] "
			"Grid(" + std::to_string(input.iSectionX) + ", " + std::to_string(input.iSectionZ) + ") " +

			"SectionMin(" + std::to_string(fSectionMinX) + ", " +
							 std::to_string(fSectionMinY) + ", " +
							 std::to_string(fSectionMinZ) + ") " +
			"SectionMax(" + std::to_string(fSectionMaxX) + ", " +
							 std::to_string(fSectionMaxY) + ", " +
							 std::to_string(fSectionMaxZ) + ") " +

			"ReceiverLS_Min(" + std::to_string(vReceiverLSMin.x) + ", " +
								 std::to_string(vReceiverLSMin.y) + ", " +
								 std::to_string(vReceiverLSMin.z) + ") " +
			"ReceiverLS_Max(" + std::to_string(vReceiverLSMax.x) + ", " +
								 std::to_string(vReceiverLSMax.y) + ", " +
								 std::to_string(vReceiverLSMax.z) + ") " +

			"CasterCount(" + std::to_string(tOut.vecCasters.size()) + ") " +

			"CasterLS_Min(" + std::to_string(vCasterLSMin.x) + ", " +
							   std::to_string(vCasterLSMin.y) + ", " +
							   std::to_string(vCasterLSMin.z) + ") " +
			"CasterLS_Max(" + std::to_string(vCasterLSMax.x) + ", " +
							   std::to_string(vCasterLSMax.y) + ", " +
							   std::to_string(vCasterLSMax.z) + ") " +

			"ReceiverZ(" + std::to_string(vReceiverLSMin.z) + "~" + std::to_string(vReceiverLSMax.z) + ") " +
			"CasterZ(" + std::to_string(vCasterLSMin.z) + "~" + std::to_string(vCasterLSMax.z) + ") " +

			"FinalXY(" + std::to_string(minX) + "~" + std::to_string(maxX) + ", " +
						   std::to_string(minY) + "~" + std::to_string(maxY) + ") " +
			"FinalZ(" + std::to_string(minZ) + "~" + std::to_string(maxZ) + ") " +

			"OrthoWHD(" + std::to_string(fOrthoWidth) + ", " +
						   std::to_string(fOrthoHeight) + ", " +
						   std::to_string(fOrthoDepth) + ") " +

			"TexelWS(" + std::to_string(fTexelWorldX) + ", " +
						  std::to_string(fTexelWorldY) + ") " +

			"BroadPhase(" + std::to_string(iBroadPhaseHitCount) + ") " +
			"Accepted(" + std::to_string(iAcceptedCasterCount) + ") " +

			"ReceiverFitMin(" + std::to_string(receiverFitBounds.Center.x - receiverFitBounds.Extents.x) + ", " +
							  std::to_string(receiverFitBounds.Center.y - receiverFitBounds.Extents.y) + ", " +
							  std::to_string(receiverFitBounds.Center.z - receiverFitBounds.Extents.z) + ") " +
			"ReceiverFitMax(" + std::to_string(receiverFitBounds.Center.x + receiverFitBounds.Extents.x) + ", " +
							  std::to_string(receiverFitBounds.Center.y + receiverFitBounds.Extents.y) + ", " +
							  std::to_string(receiverFitBounds.Center.z + receiverFitBounds.Extents.z) + ") " +

			"SectionHeight(" + std::to_string(input.sectionBounds.Extents.y * 2.f) + ") " +
			"ReceiverHeight(" + std::to_string(receiverFitBounds.Extents.y * 2.f) + ") " +
			"ReceiverCount(" + std::to_string(iReceiverCount) + ") "
		};
		CLOG_INFO(strLog);
	}
#endif

	return tOut;
}

HRESULT CRender_Manager::Execute_BakedShadowSectionJobs()
{
	m_vecBakedSection.clear();
	m_vecBakedSection.resize(BAKED_SECTION_COUNT_X * BAKED_SECTION_COUNT_Z);

	for (const auto& job : m_vecBakedSectionResults)
	{
		if (!job.bValid)
			continue;

		if (FAILED(Render_BakedSection_ToArray(job)))
			return E_FAIL;

		_uint iIndex = Compute_BakedSectionIndex(job.iSectionX, job.iSectionZ);

		auto& tSection = m_vecBakedSection[iIndex];
		tSection.iSectionX = job.iSectionX;
		tSection.iSectionZ = job.iSectionZ;
		tSection.sectionBounds = job.sectionBounds;
		tSection.casterBounds = job.casterBounds;
		tSection.matLightVP = job.matLightVP;
		tSection.vShadowParams = job.vShadowParams;
		tSection.iArraySlice = iIndex;
		tSection.bValid = true;
	}

	return S_OK;
}

HRESULT CRender_Manager::Render_BakedSection_ToArray(const BAKED_SECTION_BUILD_RESULT& job)
{
	_uint iSlice = Compute_BakedSectionIndex(job.iSectionX, job.iSectionZ);

	m_tBakedShadowViewport.Width = static_cast<_float>(SHADOW_BAKE_SIZE);
	m_tBakedShadowViewport.Height = static_cast<_float>(SHADOW_BAKE_SIZE);
	m_tBakedShadowViewport.MinDepth = 0.f;
	m_tBakedShadowViewport.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &m_tBakedShadowViewport);

	SHADER_BAKED_SHADOW_DESC tDesc = {};
	tDesc.matLightVP = job.matLightVP;
	tDesc.fShadowBias = job.vShadowParams.x;
	tDesc.fShadowStrength = job.vShadowParams.y;
	tDesc.vShadowMapInvSize = Vec2(job.vShadowParams.z, job.vShadowParams.w);

#if _DEBUG
	{
		string strLog{
			"[BakedRender] "
			"Grid(" + std::to_string(job.iSectionX) + ", " + std::to_string(job.iSectionZ) + ") " +
			"Slice(" + std::to_string(iSlice) + ") " +
			"CasterCount(" + std::to_string(job.vecCasters.size()) + ") " +
			"Bias(" + std::to_string(job.vShadowParams.x) + ") " +
			"Strength(" + std::to_string(job.vShadowParams.y) + ") " +
			"InvSize(" + std::to_string(job.vShadowParams.z) + ", " +
						 std::to_string(job.vShadowParams.w) + ")"
		};
		CLOG_INFO(strLog);
	}
#endif

	if (FAILED(m_pCB_BakedShadow->Copy_Data(tDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_RTArraySlice(
		ERenderTarget::Shadow_Baked,
		iSlice,
		true,
		m_pBakedShadowDSV)))
	{
		m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
		return E_FAIL;
	}

	for (auto* pObj : job.vecCasters)
	{
		if (pObj == nullptr)
			continue;

		if (FAILED(pObj->Render_Shadow()))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
		return E_FAIL;
	}

	m_pDeviceContext->RSSetViewports(1, &m_defaultViewport);
	return S_OK;
}

HRESULT CRender_Manager::Update_ActiveBakedSections()
{
	if (m_bBakedSectionInitialized == false)
		return S_OK;

	_int iCenterX = 0;
	_int iCenterZ = 0;

	if (Compute_MainCameraSectionIndex(iCenterX, iCenterZ) == false)
		return E_FAIL;

	if (Should_Update_ActiveBakedSections(iCenterX, iCenterZ) == false)
		return S_OK;

	m_iCurrentCenterSectionX = iCenterX;
	m_iCurrentCenterSectionZ = iCenterZ;

	m_tActiveBakedSet = {};

	for (_int dz = -1; dz <= 1; ++dz)
	{
		for (_int dx = -1; dx <= 1; ++dx)
		{
			_int sx = iCenterX + dx;
			_int sz = iCenterZ + dz;

			if (sx < 0 || sx >= BAKED_SECTION_COUNT_X || sz < 0 || sz >= BAKED_SECTION_COUNT_Z)
				continue;

			_uint iIndex = Compute_BakedSectionIndex(sx, sz);
			if (iIndex >= m_vecBakedSection.size())
				continue;

			const auto& tSection = m_vecBakedSection[iIndex];
			if (!tSection.bValid)
				continue;

			m_tActiveBakedSet.sections[m_tActiveBakedSet.iCount++] = tSection;
		}
	}

	m_bActiveBakedSectionDirty = false;

#ifdef _DEBUG
	{
		CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
		if (pMainCamera)
		{
			CTransform* pTransform = pMainCamera->Get_Component<CTransform>();
			Vec3 vCamPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

			string strLog{
				"[BakedActive] "
				"CamPos(" + std::to_string(vCamPos.x) + ", " +
							 std::to_string(vCamPos.y) + ", " +
							 std::to_string(vCamPos.z) + ") " +
				"MainSection(" + std::to_string(iCenterX) + ", " +
								 std::to_string(iCenterZ) + ") " +
				"ActiveCount(" + std::to_string(m_tActiveBakedSet.iCount) + ")"
			};
			CLOG_INFO(strLog);
		}

		for (_uint i = 0; i < m_tActiveBakedSet.iCount; ++i)
		{
			const auto& tSection = m_tActiveBakedSet.sections[i];

			string strLog{
				"[BakedActive][Slot] "
				"Slot(" + std::to_string(i) + ") " +
				"Grid(" + std::to_string(tSection.iSectionX) + ", " +
						   std::to_string(tSection.iSectionZ) + ") " +
				"Slice(" + std::to_string(tSection.iArraySlice) + ")"
			};
			CLOG_INFO(strLog);
		}
	}
#endif
	return Update_ActiveBakedSectionBuffer();
}

HRESULT CRender_Manager::Update_ActiveBakedSectionBuffer()
{
	SHADER_BAKED_SECTION_DESC tDesc = {};
	tDesc.iActiveCount = m_tActiveBakedSet.iCount;

	for (_uint i = 0; i < m_tActiveBakedSet.iCount; ++i)
	{
		const auto& src = m_tActiveBakedSet.sections[i];
		auto& dst = tDesc.sections[i];

		dst.matLightVP = src.matLightVP;
		dst.vShadowParams = src.vShadowParams;

		Vec3 vMin = Vec3(src.sectionBounds.Center.x, src.sectionBounds.Center.y, src.sectionBounds.Center.z) -
			Vec3(src.sectionBounds.Extents.x, src.sectionBounds.Extents.y, src.sectionBounds.Extents.z);

		Vec3 vMax = Vec3(src.sectionBounds.Center.x, src.sectionBounds.Center.y, src.sectionBounds.Center.z) +
			Vec3(src.sectionBounds.Extents.x, src.sectionBounds.Extents.y, src.sectionBounds.Extents.z);

		dst.vBoundsMin = Vec4(vMin.x, vMin.y, vMin.z, 0.f);
		dst.vBoundsMax = Vec4(vMax.x, vMax.y, vMax.z, 0.f);
		dst.iArraySlice = src.iArraySlice;
	}

	if (FAILED(m_pCB_ActiveBakedSections->Copy_Data(tDesc)))
		return E_FAIL;

	return S_OK;
}

_bool CRender_Manager::Compute_MainCameraSectionIndex(OUT _int& iOutX, OUT _int& iOutZ) const
{
	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return false;

	CTransform* pTransform = pMainCamera->Get_Component<CTransform>();
	
	Vec3 vPosition = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	_float fLocalX = vPosition.x - m_vBakedSectionOrigin.x;
	_float fLocalZ = vPosition.z - m_vBakedSectionOrigin.z;

	iOutX = static_cast<_int>(floor(fLocalX / m_fBakedSectionSizeX));
	iOutZ = static_cast<_int>(floor(fLocalZ / m_fBakedSectionSizeZ));

	iOutX = std::clamp(iOutX, 0, BAKED_SECTION_COUNT_X - 1);
	iOutZ = std::clamp(iOutZ, 0, BAKED_SECTION_COUNT_Z - 1);
	return true;
}

_bool CRender_Manager::Should_Update_ActiveBakedSections(_int iNewCenterX, _int iNewCenterZ) const
{
	if (m_iCurrentCenterSectionX == INT_MAX || m_iCurrentCenterSectionZ == INT_MAX)
		return true;

	if (iNewCenterX != m_iCurrentCenterSectionX || iNewCenterZ != m_iCurrentCenterSectionZ)
		return true;

	return false;
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
	Safe_Release(m_pBakedShadowDebugTex);
	Safe_Release(m_pBakedShadowDebugSRV);
#endif
	for (auto& RenderObjects : m_renderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	for (auto& DissolveTexture : m_pDissolveTextures)
	{
		Safe_Release(DissolveTexture);
	}
	m_pDissolveTextures.clear();

	//
	Safe_Release(m_pLUTTexture);
	Safe_Release(m_pSSAONoiseSRV);
	Safe_Release(m_pPerlinNoiseSRV);
	Safe_Release(m_pShadowDSTexture);
	Safe_Release(m_pShadowDSV);
	Safe_Release(m_pBakedShadowDSV);
	Safe_Release(m_pCB_Outlineparam);
	Safe_Release(m_pCB_Bloomparam);
	Safe_Release(m_pCB_HDRparam);
	Safe_Release(m_pCB_SSAOkernel);
	Safe_Release(m_pCB_SSAOparam);
	Safe_Release(m_pCB_Fog);
	Safe_Release(m_pCB_Toonparam);
	Safe_Release(m_pCB_CascadeShadow);
	Safe_Release(m_pCB_BakedShadow);
	Safe_Release(m_pCB_ActiveBakedSections);
	Safe_Release(m_pFogShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
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

HRESULT CRender_Manager::Commit_FogParam()
{
	return m_pCB_Fog ? m_pCB_Fog->Copy_Data(m_tFogDesc) : E_FAIL;
}

HRESULT CRender_Manager::Commit_ToonParam()
{
	return m_pCB_Toonparam ? m_pCB_Toonparam->Copy_Data(m_tToonparamDesc) : E_FAIL;
}
HRESULT CRender_Manager::Commit_CascadeParam()
{
	return m_pCB_CascadeShadow ? m_pCB_CascadeShadow->Copy_Data(m_tCascadeShadowDesc) : E_FAIL;
}
HRESULT CRender_Manager::Commit_BakedShadowParam()
{
	return m_pCB_BakedShadow ? m_pCB_BakedShadow->Copy_Data(m_tBakedShadowDesc) : E_FAIL;
}
HRESULT CRender_Manager::Commit_AllPostParams()
{
	if (FAILED(Commit_SSAOParam()))			return E_FAIL;
	if (FAILED(Commit_HDRParam()))			return E_FAIL;
	if (FAILED(Commit_BloomParam()))		return E_FAIL;
	if (FAILED(Commit_OutlineParam()))		return E_FAIL;
	if (FAILED(Commit_FogParam()))			return E_FAIL;
	if (FAILED(Commit_ToonParam()))			return E_FAIL;
	if (FAILED(Commit_CascadeParam()))		return E_FAIL;
	if (FAILED(Commit_BakedShadowParam()))	return E_FAIL;
	return S_OK;
}

#ifdef _DEBUG

HRESULT CRender_Manager::Push_DebugComponent(CComponent* pComponent)
{
	m_debugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);
	return S_OK;
}

ID3D11ShaderResourceView* CRender_Manager::Get_BakedShadowDebugSRV()
{
	return m_pBakedShadowDebugSRV;
}

void CRender_Manager::Update_BakedShadowDebugTexture(_uint iSlice)
{
	if (m_pBakedShadowDebugTex == nullptr)
		return;

	CRenderTargetArray* pRTArray = m_pGameInstance->m_pRenderTarget_Manager->Get_RenderTargetArray(ERenderTarget::Shadow_Baked);
	if (pRTArray == nullptr)
		return;

	ID3D11Texture2D* pTextureArray = pRTArray->Get_Texture2D();
	if (pTextureArray == nullptr)
		return;

	D3D11_TEXTURE2D_DESC srcDesc{};
	pTextureArray->GetDesc(&srcDesc);

	if (iSlice >= srcDesc.ArraySize)
		return;

	const UINT srcSubresource = D3D11CalcSubresource(0, iSlice, 1);

	m_pDeviceContext->CopySubresourceRegion(
		m_pBakedShadowDebugTex,
		0,
		0, 0, 0,
		pTextureArray,
		srcSubresource,
		nullptr
	);

	m_iBakedShadowDebugSlice = (_int)iSlice;
}

HRESULT CRender_Manager::Create_BakedShadowSliceSRV()
{
	CRenderTargetArray* pRTArray = m_pGameInstance->m_pRenderTarget_Manager->Get_RenderTargetArray(ERenderTarget::Shadow_Baked);
	if (pRTArray == nullptr)
		return E_FAIL;

	ID3D11Texture2D* pTextureArray = pRTArray->Get_Texture2D();
	if (pTextureArray == nullptr)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC srcDesc{};
	pTextureArray->GetDesc(&srcDesc);

	if (m_pBakedShadowDebugSRV == nullptr)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = srcDesc.Width;
		desc.Height = srcDesc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = srcDesc.Format;
		desc.SampleDesc = srcDesc.SampleDesc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pBakedShadowDebugTex)))
			return E_FAIL;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pBakedShadowDebugTex, &srvDesc, &m_pBakedShadowDebugSRV)))
			return E_FAIL;
	}

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
