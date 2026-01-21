#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CShader;

class CRender_Manager final : public CBase
{
	using Super = CBase;
private:
	CRender_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRender_Manager() = default;

	HRESULT Initialize();
public:
	HRESULT Set_Components();
	HRESULT Render();
	void Push_RenderObject(RENDER_CATEGORY eCategory, CGameObject* pGO);
	void Baek_StaticModelShadow(const wstring& wstrLayerTag);
private:	
	HRESULT Render_Priority();
	HRESULT Render_NoneBlend();
	HRESULT Render_Lights();
	HRESULT Render_Combined();
	HRESULT Render_NonLights();
	HRESULT Render_Blend();
	HRESULT Render_BlendUI();
	HRESULT Render_UI();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	list<class CGameObject*> m_renderObjects[ENUM_TO_UINT(RENDER_CATEGORY::END)];

	class CVIBuffer_Rect_Tex* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };

	_float4x4 m_matWorld_RT = {};
public:
	static CRender_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
#ifdef  _DEBUG
public:
	HRESULT Push_DebugComponent(class CComponent* pComponent);
private:
	_bool							m_bDebug = { false };
	list<class CComponent*>			m_debugComponents;
	HRESULT Ready_Debug();
	HRESULT Render_Debug();
#endif
};

NS_END
