#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
class CBone;
class CComputeShader;
NS_END

NS_BEGIN(Client)

class CBody final : public CPartObject
{
	using Super = CPartObject;
public:
	typedef struct tagBodyDesc : public CPartObject::PARTOBJ_DESC
	{
		wstring wstrModelPrototypeName = { L"" };
	}BODY_DESC;
protected:
	CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBody(const CBody& rhs);
	virtual ~CBody() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
public:
	const Matrix* Get_SocketMatrix(const _char* szBoneName);
	const Matrix* Get_SocketMatrix(_uint iIndex);
	const Matrix* Get_PosMatrix(_uint iIndex);
	CBone* Get_CamBone();
	CBone* Get_CamSocketBone();

	CBone* Get_HeadBone();
	CBone* Get_NeckBone();
	CBone* Get_Spine1Bone();
	//CBone* Get_SpineBone();
	CBone* Get_WeaponSocket();
	CBone* Get_RightHandSocket();

public:
	CComputeShader* Get_AnimMixCS() const { return m_pBoneAnimMixCS; }
	CComputeShader* Get_BoneCombineCS() const { return m_pBoneCombineCS; }

private:
	HRESULT Ready_Components(BODY_DESC *pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Ready_ComputeShader();
private:
	_int m_iHead_Index = { 0 };
	_int m_iNeck_Index = { 0 };
	_int m_iSpine1_Index = { 0 };

	_int m_iCamPos_Index	= { 0 }; // cam ¿¬°á »À idx
	_int m_iCamSocket_Index = { 0 }; // cam ¿¬°á »À idx
	_int m_iRootMotion_Index	= { 0 }; // rootmotion »À idx

	_int m_iFaceMesh_Index = { 0 };
private:
	CComputeShader* m_pBoneMeshCS			= { nullptr };
	CComputeShader* m_pBoneCombineCS		= { nullptr };
	CComputeShader* m_pBoneAnimEvaluateCS	= { nullptr };
	CComputeShader* m_pBoneAnimBlendCS		= { nullptr };
	CComputeShader* m_pBoneAnimMixCS		= { nullptr };
	CComputeShader* m_pAdditiveMixCS		= { nullptr };
	CComputeShader* m_pBoneMoveCS = { nullptr };

public:
	static CBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END