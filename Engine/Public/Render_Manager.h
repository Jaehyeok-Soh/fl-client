#pragma once
#include "Base.h"

#define SSAO_KERNAL 16

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CShader;
class CTextureBase;

template<typename T>
class CConstant_Buffer;

/*
* NONEBLEND = Opaque
* BLEND = Transparent
* NONELIGHT = Forward Special Pass
* UI/BLENDUI = UI Queue
* PRIORITY = Pre-pass
*/

class CRender_Manager final : public CBase
{
	using Super = CBase;
private:
	CRender_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRender_Manager() = default;

	HRESULT Initialize();
public:
	HRESULT Set_ShaderResources();
	HRESULT Render();
	void Clear();
	void Push_RenderObject(RENDER_CATEGORY eCategory, CGameObject* pGO);
private:	
	HRESULT Render_Priority();
	HRESULT Render_NoneBlend();
	HRESULT Render_ComputeLight_Blend();	/* Deffered에 들어갈 알파블렌딩 애들 */
	HRESULT Render_SSAO();
	HRESULT Render_Lights();
	HRESULT Render_CombinedHDR();
	HRESULT Render_Environment();
	HRESULT Render_Outline();
	HRESULT Render_NonLights();
	// 이펙트 전용 (디스토션)
	HRESULT Render_Distotion();
	HRESULT Render_WBOIT(); // 가중치 블랜딩 (Weighted OIT)
	HRESULT Render_Blend();
	HRESULT Render_Bloom();
	HRESULT Render_ToneMap();
	HRESULT Render_BlendUI();
	HRESULT Render_UI();
	void Sort_UI();

private:
	HRESULT Ready_BlendStates(); // (Weighted OIT) 전용.
private:
	array<Vec4, SSAO_KERNAL> Build_SSAO_Kernal16();
	HRESULT Create_SSAO_NoiseSRV();
	HRESULT Set_ConstantBuffer();
	void Request_SortUI();
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	list<class CGameObject*> m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::END)];
	vector<class CGameObject*> m_filteredRenderObjects;
	vector<class CGameObject*> m_visibleNear;
	vector<class CGameObject*> m_visibleMid;
	vector<class CGameObject*> m_visibleFar;

	class CVIBuffer_Rect_Tex* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };
	Matrix m_matWorld_RT = Matrix::Identity;
	D3D11_VIEWPORT m_defaultViewport{};
	D3D11_VIEWPORT m_halfViewport{};
	ID3D11ShaderResourceView* m_pSSAONoiseSRV{ nullptr };

	// WBOIT 누적용 스테이트
	ID3D11BlendState* m_pWBOIT_AccumulateBS = { nullptr };
	ID3D11BlendState* m_pAlphaBlendBS = { nullptr };

	SHADER_SSAOKERNEL_DESC m_tSSAOkernelDesc{};
	SHADER_SSAOPARAM_DESC m_tSSAOparamDesc{};
	SHADER_HDRPARAM_DESC m_tHDRparamDesc{};
	SHADER_BLOOMPARAM_DESC m_tBloomparamDesc{};
	SHADER_OUTLINE_DESC m_tOutlineparamDesc{};
	CConstant_Buffer<SHADER_SSAOKERNEL_DESC>* m_pCB_SSAOkernel{ nullptr };
	CConstant_Buffer<SHADER_SSAOPARAM_DESC>* m_pCB_SSAOparam{ nullptr };
	CConstant_Buffer<SHADER_HDRPARAM_DESC>* m_pCB_HDRparam{ nullptr };
	CConstant_Buffer<SHADER_BLOOMPARAM_DESC>* m_pCB_Bloomparam{ nullptr };
	CConstant_Buffer<SHADER_OUTLINE_DESC>* m_pCB_Outlineparam{ nullptr };
	CTextureBase* m_pLUTTexture{ nullptr };
public:
	static CRender_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
#ifdef  _DEBUG
public:
	HRESULT Push_DebugComponent(class CComponent* pComponent);
	
	// SSAO
	SHADER_SSAOPARAM_DESC& Get_SSAOParamDesc() { return m_tSSAOparamDesc; }
	const SHADER_SSAOPARAM_DESC& Get_SSAOParamDesc() const { return m_tSSAOparamDesc; }
	HRESULT Commit_SSAOParam();

	// HDR
	SHADER_HDRPARAM_DESC& Get_HDRParamDesc() { return m_tHDRparamDesc; }
	const SHADER_HDRPARAM_DESC& Get_HDRParamDesc() const { return m_tHDRparamDesc; }
	HRESULT Commit_HDRParam();

	// Bloom
	SHADER_BLOOMPARAM_DESC& Get_BloomParamDesc() { return m_tBloomparamDesc; }
	const SHADER_BLOOMPARAM_DESC& Get_BloomParamDesc() const { return m_tBloomparamDesc; }
	HRESULT Commit_BloomParam();
	
	// Outline
	SHADER_OUTLINE_DESC& Get_OutlineParamDesc() { return m_tOutlineparamDesc; }
	const SHADER_OUTLINE_DESC& Get_OutlineParamDesc() const { return m_tOutlineparamDesc; }
	HRESULT Commit_OutlineParam();

	HRESULT Commit_AllPostParams();
private:
	_bool							m_bDebug = { false };
	list<class CComponent*>			m_debugComponents;
	HRESULT Ready_Debug();
	HRESULT Render_Debug();
#endif
};

NS_END
