#pragma once
#include "UIObject.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CGenericUI abstract : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagGenericUIDesc : public UIOBJECT_DESC
	{
		uint32_t iUIType;
		uint32_t iRectTransformType;
		_wstring wstrTextureTag;
		uint32_t iTextureIndex;

		CCanvas* pCanvasCache = { nullptr };
	}GENERIC_UI_DESC;

protected :
	CGenericUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGenericUI(const CGenericUI& rhs);
	virtual ~CGenericUI() = default;

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

	_bool Calc_HitEvent();
	void Acting_By_InteractState();

private:
	HRESULT Ready_Components(GENERIC_UI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	void Set_RectPos(const Vec3& pos) { m_vRectPos = pos; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	void Set_TextureIndex(_uint index) { m_iTextureIndex = index; }
	const _string& Get_Tag() { return m_strName; }

protected:
	ERectTransform m_eRectTransformType = { ERectTransform::C };
	_wstring m_wstrTextureTag = {};
	uint32_t m_iTextureIndex = {};
	Vec3 m_vRectPos = {};
	Vec3 m_vRenderPos = {};
	RECT m_tRenderRect = {};
	CCanvas* m_pParentCanvasCache = { nullptr };
	Vec3 m_vMoveOffset = {};

public:
	virtual void Free()override;

};

NS_END