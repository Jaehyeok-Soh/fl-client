#pragma once
#include "UIObject.h"
#include "Tool_Defines.h"
#include "UIData_Repository.h"

NS_BEGIN(Tool)

class CToolUI final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagToolUIDesc : public Super::UIOBJECT_DESC
	{
		_string strName;
		_string strInitTextureTag;
		uint32_t iInitTextureIndex;

	}TOOLUI_DESC;

private:
	CToolUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CToolUI(const CToolUI& rhs);
	virtual ~CToolUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(TOOLUI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	const _string& Get_Name()const { return m_strName; }
	void Set_Name(const _string& str) { m_strName = str; }

	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	void Set_RectTransformType(ERectTransform value) { m_eRectTransformType = value; }
	EUiType Get_UIType() const { return m_eUIType; }
	void Set_UIType(EUiType value) { m_eUIType = value; }

	const _wstring& Get_TextureTag() const { return m_wstrTextureTag; }
	void Set_TextureTag(const _wstring& value) { m_wstrTextureTag = value; }
	uint32_t Get_TextureIndex() const { return m_iTextureIndex; }
	void Set_TextureIndex(uint32_t value) { m_iTextureIndex = value; }


	_float* Get_WIdth_Ptr() { return &m_fWidth; }
	_float* Get_Height_Ptr() { return &m_fHeight; }
	_float* Get_PosX_Ptr() { return &m_fX; }
	_float* Get_PosY_Ptr() { return &m_fY; }
	_float* Get_PosZ_Ptr() { return &m_fZ; }

private:
	_string m_strName = {};

	GENERIC_UI_DATA m_tUIData = {};

	ERectTransform m_eRectTransformType = { ERectTransform::CENTER };
	EUiType m_eUIType = { EUiType::IMAGE_2D };
	
	_wstring m_wstrTextureTag = {};
	uint32_t m_iTextureIndex = {};

public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

