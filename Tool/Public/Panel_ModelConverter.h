#pragma once
#include "ImGui_Panel.h"


NS_BEGIN(Engine)

class CGameInstance;

NS_END


NS_BEGIN(Tool)

class CPanel_ModelConverter : public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_ModelConverter(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_ModelConverter() = default;

	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;
private:
	void	Open_FolderDialog();
	void	Convert_FbxFolder(const wchar_t* wszFloderPath);
private:
	CGameInstance*		m_pGameInstance;

	IFileOpenDialog*	m_pOpenDialog{nullptr};

	Matrix				m_SRTMatirx{ Matrix::Identity };

	Vec3				m_vTranslation{0,0,0};
	Vec3				m_vRotation{0,0,0};
	Vec3				m_vScale{1,1,1};

public:
	static  CPanel_ModelConverter* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};
NS_END
