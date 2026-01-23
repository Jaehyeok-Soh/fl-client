#include "Render_Manager.h"
#include "Constant_Buffer.h"
#include "GameObject.h"
#include "Camera.h"
#include "VIBuffer_Rect_Tex.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "GameInstance.h"

CRender_Manager::CRender_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRender_Manager::Initialize()
{
	_uint iViewportsCount = { 1 };
	D3D11_VIEWPORT viewportDesc = {};
	m_pDeviceContext->RSGetViewports(&iViewportsCount, &viewportDesc);

	const _uint& iWidth = (_uint)viewportDesc.Width;
	const _uint& iHeight = (_uint)viewportDesc.Height;

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
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Normal, &desc)))
			return E_FAIL;
	}
	// For. Target_Depth
	{
		CRenderTarget::RENDERTARGET_DESC desc = {};
		desc.ePixelFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.iWidth = iWidth;
		desc.iHeight = iHeight;
		desc.vClearColor = Vec4::Zero;
		if (FAILED(m_pGameInstance->Add_RenderTarget(ERenderTarget::Depth, &desc)))
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

	// For. MRT_GameObjects
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::Diffuse)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::Normal)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::GameObjects, ERenderTarget::Depth)))
			return E_FAIL;
	}

	// For. MRT_LightAcc
	{
		if (FAILED(m_pGameInstance->Add_MRT(EMRTLayer::LightAcc, ERenderTarget::Shade)))
			return E_FAIL;
	}

	// For. MRT_Shadow
	{
	}

	m_matWorld_RT = Matrix::CreateScale(viewportDesc.Width, viewportDesc.Height, 1.f);

#ifdef _DEBUG
	if (FAILED(Ready_Debug()))
		return E_FAIL;
#endif
	return S_OK;
}

void CRender_Manager::Push_RenderObject(RENDER_CATEGORY eCategory, CGameObject* pGO)
{
	if (eCategory >= RENDER_CATEGORY::END)
		return;

	if (pGO)
		m_renderObjects[ENUM_TO_UINT(eCategory)].push_back(pGO);

	Safe_AddRef(pGO);
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

HRESULT CRender_Manager::Set_Components()
{
	{
		if (!(m_pVIBuffer = CVIBuffer_Rect_Tex::Create(m_pDevice, m_pDeviceContext, nullptr)))
			return E_FAIL;

		CShader::SHADER_ORIGIN_DESC desc = {};
		desc.pShaderFilePath = L"../../Shaders/Shader_Deffered.hlsl";
		desc.iNumElements = Engine::VTXPOSTEX::iNumElements;
		desc.pElements = Engine::VTXPOSTEX::Elements;
		if (!(m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, &desc)))
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

	m_pGameInstance->Setup_UIViewProj_ToCBuffer();

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Combined()))
		return E_FAIL;

	m_pGameInstance->Setup_ViewProj_ToCBuffer();
	
	if (FAILED(Render_NonLights()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	m_pGameInstance->Setup_UIViewProj_ToCBuffer();
	
	if (FAILED(Render_UI()))
		return E_FAIL;

	if (FAILED(Render_BlendUI()))
		return E_FAIL;

#ifdef _DEBUG
	if (m_pGameInstance->KeyButton_Down(DIK_F9))
		m_bDebug = !m_bDebug;

	if (m_bDebug == true)
	{
		if (FAILED(Render_Debug()))
			return E_FAIL;
	}
#endif

	return S_OK;
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

	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::NONEBLEND)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::NONEBLEND)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Render_Lights()
{
	if (FAILED(m_pGameInstance->Begin_MRT(EMRTLayer::LightAcc)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Normal, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRender_Manager::Render_Combined()
{
	if (FAILED(m_pShader->Bind_TransformData(m_matWorld_RT)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Diffuse, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Shade, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
		return E_FAIL;

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::COMBINED));
	m_pShader->Apply();
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();
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
	for (CGameObject* pElement : m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::UI)])
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;

		Safe_Release(pElement);
	}
	m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::UI)].clear();

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

HRESULT CRender_Manager::Ready_Debug()
{
	if (FAILED(m_pGameInstance->Ready_RT_Debug(ERenderTarget::Diffuse, 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(ERenderTarget::Normal, 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(ERenderTarget::Shade, 450.f, 150.f, 300.f, 300.f)))
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

	m_pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::DEBUG));

	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Debug_RT_Render(EMRTLayer::GameObjects, m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Debug_RT_Render(EMRTLayer::LightAcc, m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif