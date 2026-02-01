#pragma once
#include "ImGui_Panel.h"

// 로드한 모델의 정보를 보여주는 패널

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CPanel_Model : public CImGui_Panel
{
	using Super = CImGui_Panel;
private:
	explicit CPanel_Model(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_Model() = default;

public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void	Update(const _float fTimeDelta)override;

private:
	CGameInstance* m_pGameInstance = { nullptr };

	// 창
private:
	HRESULT Render_LoadFile();

	HRESULT Render_ModelInfo();
	HRESULT Render_Animations();
	HRESULT Render_Bones();
	HRESULT Render_PreTransform();

private:
	HRESULT Make_Model();

public:
	static  CPanel_Model* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END