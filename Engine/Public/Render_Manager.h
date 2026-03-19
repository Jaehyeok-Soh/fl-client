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
private:
	struct FogPass
	{
		enum
		{
			Distance = 0,
			Height
		};
	};
public:
	HRESULT Set_ShaderResources();
	HRESULT Render();
	void Clear();
	void Push_RenderObject(RENDER_CATEGORY eCategory, CGameObject* pGO);
	HRESULT Set_CascadeShadowConstantBuffer(class CShader* pShader);
	HRESULT Set_BakedShadowConstantBuffer(class CShader* pShader);

	HRESULT Initialize_BakedShadowSections(BoundingBox* pRootBox);
	HRESULT Build_BakedShadowSections();
private:	
	HRESULT Render_Priority();
	HRESULT Render_NoneBlend();
	HRESULT Render_ComputeLight_Blend();	/* Deffered에 들어갈 알파블렌딩 애들 */
	HRESULT Render_SSAO();
	HRESULT Render_Lights();
	HRESULT Render_CombinedHDR();
	HRESULT Render_CascadeShadow();
	HRESULT Render_Environment();
	HRESULT Render_Fog();
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
	HRESULT Create_Perlin_NoiseSRV();
	HRESULT Set_ConstantBuffer();
	HRESULT Ready_RTArray();
	HRESULT Ready_RT();
	HRESULT Ready_MRT();
	HRESULT Create_ShadowResource();
	HRESULT Compute_ShadowCascade();
	

	HRESULT Bind_ActiveBakedSections();
	HRESULT Create_RootBox(OUT BoundingBox& outRootBox);
	// 섹션 정의
	_uint Compute_BakedSectionIndex(_int iSectionX, _int iSectionZ) const;
	BoundingBox Compute_BakedSectionBounds(_int iSectionX, _int iSectionZ) const;
	// 멀티스레드
	HRESULT Build_BakedShadowSectionJobs();
	BAKED_SECTION_BUILD_RESULT Build_BakedSection(const BAKED_SECTION_BUILD_INPUT& input);
	// 렌더 관련
	HRESULT Execute_BakedShadowSectionJobs();
	HRESULT Render_BakedSection_ToArray(const BAKED_SECTION_BUILD_RESULT & job);
	// 런타임 활성 섹션 관리
	HRESULT Update_ActiveBakedSections();
	HRESULT Update_ActiveBakedSectionBuffer();
	_bool Compute_MainCameraSectionIndex(OUT _int& iOutX, OUT _int& iOutZ) const;
	_bool Should_Update_ActiveBakedSections(_int iNewCenterX, _int iNewCenterZ) const;
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
	SHADER_TOON_DESC m_tToonparamDesc{};
	CConstant_Buffer<SHADER_SSAOKERNEL_DESC>* m_pCB_SSAOkernel{ nullptr };
	CConstant_Buffer<SHADER_SSAOPARAM_DESC>* m_pCB_SSAOparam{ nullptr };
	CConstant_Buffer<SHADER_HDRPARAM_DESC>* m_pCB_HDRparam{ nullptr };
	CConstant_Buffer<SHADER_BLOOMPARAM_DESC>* m_pCB_Bloomparam{ nullptr };
	CConstant_Buffer<SHADER_OUTLINE_DESC>* m_pCB_Outlineparam{ nullptr };
	CConstant_Buffer<SHADER_TOON_DESC>* m_pCB_Toonparam{ nullptr };
	CTextureBase* m_pLUTTexture{ nullptr };

	// Fog
	class CShader* m_pFogShader = { nullptr };
	SHADER_FOG_DESC m_tFogDesc{};
	CConstant_Buffer<SHADER_FOG_DESC>* m_pCB_Fog{ nullptr };
	ID3D11ShaderResourceView* m_pPerlinNoiseSRV{ nullptr };

	// Shadow
	D3D11_VIEWPORT m_tShadowViewport{};
	SHADER_CASCADE_SHADOW_DESC m_tCascadeShadowDesc{};
	CConstant_Buffer<SHADER_CASCADE_SHADOW_DESC>* m_pCB_CascadeShadow{ nullptr };
	ID3D11Texture2D* m_pShadowDSTexture{ nullptr };
	ID3D11DepthStencilView* m_pShadowDSV{ nullptr };

	// Shadow_Baked
	// 섹션 데이터
	vector<BAKED_SHADOW_SECTION> m_vecBakedSection;
	// 활성 섹션 상태
	ACTIVE_BAKED_SET m_tActiveBakedSet{};
	_int m_iCurrentCenterSectionX{ INT_MAX };
	_int m_iCurrentCenterSectionZ{ INT_MAX };
	// 월드 분할 정보
	Vec3 m_vBakedSectionOrigin{ Vec3::Zero };
	_float m_fBakedSectionSizeX{ 0.f };
	_float m_fBakedSectionSizeZ{ 0.f };
	BoundingBox m_bakedWorldRootBounds{};
	// 히스테리시스
	_float m_fSectionUpdateHysteresisX{ 0.f };
	_float m_fSectionUpdateHysteresisZ{ 0.f };
	// For. ThreadPool
	// CPU 준비 작업 결과 버퍼
	vector<BAKED_SECTION_BUILD_RESULT> m_vecBakedSectionResults;
	// Resource
	D3D11_VIEWPORT m_tBakedShadowViewport{};
	SHADER_BAKED_SHADOW_DESC m_tBakedShadowDesc{};
	CConstant_Buffer<SHADER_BAKED_SHADOW_DESC>* m_pCB_BakedShadow{ nullptr };
	CConstant_Buffer<SHADER_BAKED_SECTION_DESC>* m_pCB_ActiveBakedSections{ nullptr };
	ID3D11DepthStencilView* m_pBakedShadowDSV{ nullptr };
	// 상태 플래그
	_bool m_bBakedSectionInitialized{ false };
	_bool m_bActiveBakedSectionDirty{ true };
public:
	static CRender_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
	
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

	// Fog
	SHADER_FOG_DESC& Get_FogParamDesc() { return m_tFogDesc; }
	const SHADER_FOG_DESC& Get_FogParamDesc() const { return m_tFogDesc; }
	HRESULT Commit_FogParam();
	
	// Toon
	SHADER_TOON_DESC& Get_ToonParamDesc() { return m_tToonparamDesc; }
	const SHADER_TOON_DESC& Get_ToonParamDesc() const { return m_tToonparamDesc; }
	HRESULT Commit_ToonParam();

	// Cascade
	SHADER_CASCADE_SHADOW_DESC& Get_CascadeParamDesc() { return m_tCascadeShadowDesc; }
	const SHADER_CASCADE_SHADOW_DESC& Get_CascadeParamDesc() const { return m_tCascadeShadowDesc; }
	HRESULT Commit_CascadeParam();

	// Static Baked Shadow
	SHADER_BAKED_SHADOW_DESC& Get_BakedShadowParamDesc() { return m_tBakedShadowDesc; }
	const SHADER_BAKED_SHADOW_DESC& Get_BakedShadowParamDesc() const { return m_tBakedShadowDesc; }
	HRESULT Commit_BakedShadowParam();

	HRESULT Commit_AllPostParams();

	const ACTIVE_BAKED_SET &Get_ActiveBakedSectionSet() const { return m_tActiveBakedSet; }
	void Update_BakedShadowDebugTexture(_uint iSlice);
	ID3D11ShaderResourceView* Get_BakedShadowDebugSRV();
#ifdef  _DEBUG
public:
	HRESULT Push_DebugComponent(class CComponent* pComponent);
private:
	HRESULT Create_BakedShadowSliceSRV();
private:
	_bool							m_bDebug = { false };
	list<class CComponent*>			m_debugComponents;
	ID3D11Texture2D* m_pBakedShadowDebugTex = nullptr;
	ID3D11ShaderResourceView* m_pBakedShadowDebugSRV = nullptr;
	_int m_iBakedShadowDebugSlice = -1;
	HRESULT Render_Debug();
#endif
};

NS_END
