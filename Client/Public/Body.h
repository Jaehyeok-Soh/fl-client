#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
class CBone;
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
	virtual void OnCollision(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual _bool On_Hit(_uint iCollideMyLayer, ATTACK_DESC* pDesc, CGameObject* pOther) override;
	virtual HRESULT Render() override;
public:
	const Matrix* Get_SocketMatrix(const _char* szBoneName);
	const Matrix* Get_SocketMatrix(_uint iIndex);
	CBone* Get_CamBone();
	CBone* Get_CamSocketBone();

	CBone* Get_HeadBone();
	CBone* Get_NeckBone();
	CBone* Get_Spine1Bone();
	//CBone* Get_SpineBone();
	//CBone* Get_SwordSocket();
	//CBone* Get_RightHandWeaponSocket();
	//CBone* Get_LeftHandSocket();
	//CBone* Get_RightHandSocket();
	//CBone* Get_LeftFootSocket();
	//CBone* Get_RightFootSocket();
	//CBone* Get_LeftShoulderSocket();
	//CBone* Get_RightShoulderSocket();
	//CBone* Get_EffectMouseSocket();
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
	//_int m_iSpine_Index = { 0 };
	//_int m_iLeftHand_Index = { 0 };
	//_int m_iRightHand_Index = { 0 };
	//_int m_iLeftFoot_Index = { 0 };
	//_int m_iRightFoot_Index = { 0 };
	//_int m_iSwordSocket_Index = { 0 };
	//_int m_iLeftShoulderSocket_Index = { 0 };
	//_int m_iRightShoulderSocket_Index = { 0 };
	//_int m_iEffectMouseSocket_Index = { 0 };
	//_int m_iRightHandWeaponSocket_Index = { 0 };
public:
	static CBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END