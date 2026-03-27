#pragma once
#include "UIProgress_Bar.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUILoading_Progress final : public CUIProgress_Bar
{
	using Super = CUIProgress_Bar;
public:
	typedef struct tagLoadingProgressDesc : public PROGRESS_BAR_DESC
	{
		const _float* pLoadingRatio;
	}LOADING_PROGRESS_DESC;

private:
	CUILoading_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUILoading_Progress(const CUILoading_Progress& rhs);
	virtual ~CUILoading_Progress() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Components(LOADING_PROGRESS_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual void Bind_Events()override { Super::Bind_Events(); };
private:
	const _float* m_pLoadingRatio = { nullptr };
public:
	static CUILoading_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END