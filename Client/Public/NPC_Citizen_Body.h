#pragma once
#include "NPC_Body_Base.h"

NS_BEGIN(Engine)

class CComputeShader;

NS_END


NS_BEGIN(Client)

class CNPC_Citizen_Body : public CNPC_Body_Base
{
	using Super = CNPC_Body_Base;
public:
	typedef struct tagNPC_Citizen_Body : public CNPC_Body_Base::NPCBODY_DESC
	{
		SHADER_RGBCOLOR_DESC	tRGBColorData{};
		string					strLoopAnimName{""};
	}NPC_CITIZEN_BODY;
protected:
	CNPC_Citizen_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNPC_Citizen_Body(const CNPC_Citizen_Body& rhs);
	virtual ~CNPC_Citizen_Body() = default;

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelIndex) override;
	virtual void			Update_Priority(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Update_Late(_float fTimeDelta) override;
	virtual void			Ready_Before_Render(_float fTimeDelta) override;
	virtual void			OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool			On_Hit(const HIT_DESC& hitDesc) override;
	virtual HRESULT			Render() override;
protected:
	HRESULT					Ready_Component(NPC_CITIZEN_BODY* pDesc);
	HRESULT					Ready_Animation(NPC_CITIZEN_BODY* pDesc);
	HRESULT					Ready_ShaderPass(NPC_CITIZEN_BODY* pDesc);
private:
	CComputeShader*			m_pBoneMeshCS;
	CComputeShader*			m_pBoneCombineCS;
	CComputeShader*			m_pAnimECS;
	CComputeShader*			m_pAnimBlendCS;
	CComputeShader*			m_pAnimMix;

	SHADER_RGBCOLOR_DESC	m_tRGBColorDesc;

	vector<EAnimShaderPass>	m_vecShaderPass;

	string					m_strLoopAninName;
public:
	static CNPC_Citizen_Body*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END

