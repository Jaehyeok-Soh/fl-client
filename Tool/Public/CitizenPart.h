#pragma once
#include "Tool_PartObject.h"
#include "CitizenData.h"


NS_BEGIN(Tool)

class CCitizenPart final: public Tool_PartObject
{
	using Super = Tool_PartObject;
public:
	typedef struct tagCitizenPartDesc : public Tool_PartObject::PARTOBJ_DESC
	{
		Vec4				tTintColor{ 1.f,1.f,1.f,1.f };


		const Matrix*				pBoneSocket{nullptr};
		Matrix						ModelPreMatrix{ Matrix::Identity };
		ELevelType					iAddModelPrototypeLevel{ ELevelType::STATIC };
		wstring						wstrPartModelFolderName = { L"" };
	}CITIZENPART_DESC;

private:
	CCitizenPart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCitizenPart(const CCitizenPart& rhs);
	virtual ~CCitizenPart() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void			Set_TintColor(const Vec4& vColor) { m_tMIDesc.vTintColor; }
	HRESULT			Change_Model(const wstring& wstrModelFolderName);
	void			Change_Color(const Vec4& vColor) { m_tMIDesc.vTintColor = vColor; }
public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual void	Ready_Before_Render(_float fTimeDelta) override;
	virtual void	OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void	OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void	OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual HRESULT Render() override;
public:
	void			Update_PartsModel();
private:

	const Matrix*	m_pBoneSocket;

	SHADER_MI_DESC	m_tMIDesc;
private:
	HRESULT					Ready_Components(CITIZENPART_DESC* pDesc);
public:
	static	CCitizenPart*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual void Free() override;
};

NS_END
