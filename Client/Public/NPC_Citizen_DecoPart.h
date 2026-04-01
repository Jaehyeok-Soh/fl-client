#pragma once
#include "PartObject.h"


NS_BEGIN(Client)
class CNPC_Citizen_DecoPart final: public CPartObject
{
public:
	using Super = CPartObject;
public:
	typedef struct tagCNPC_Citizen_DecoPartDesc : public CPartObject::PARTOBJ_DESC
	{
		Vec4				tTintColor{ 1.f,1.f,1.f,1.f };

		const Matrix*		pBoneSocket{ nullptr };
		Matrix				ModelPreMatrix{ Matrix::Identity };
		wstring				wstrPartModelFolderName = { L"" };
	}NPC_CITIZEN_DECOPARTDESC;

private:
	CNPC_Citizen_DecoPart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNPC_Citizen_DecoPart(const CNPC_Citizen_DecoPart& rhs);
	virtual ~CNPC_Citizen_DecoPart() = default;

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
	virtual HRESULT Render_Shadow() override;
public:
	void			Update_PartsModel();
private:
	const Matrix*	m_pBoneSocket;
	SHADER_MI_DESC	m_tMIDesc{};
private:
	HRESULT Ready_Components(NPC_CITIZEN_DECOPARTDESC* pDesc);
public:
	static	CNPC_Citizen_DecoPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};

NS_END

