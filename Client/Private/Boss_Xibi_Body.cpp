#include "pch.h"
#include "Boss_Xibi_Body.h"
#include "ComputeShader.h"
#include "Model.h"
#include "GameInstance.h"

CBoss_Xibi_Body::CBoss_Xibi_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CBoss_Xibi_Body::CBoss_Xibi_Body(const CBoss_Xibi_Body& rhs)
	: Super(rhs)
{
}

HRESULT CBoss_Xibi_Body::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi_Body::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Boss_Xibi_Body");
	return S_OK;
}

HRESULT CBoss_Xibi_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	// Mixing Face
	{
		_uint iFaceAnimIdx = Get_Component<CModel>()->Get_AnimationIndex(L"Animation_Xibi_FaceNone");
		vector<CModel::DATA_ANIMIX> vecMix = { { 139, false, 1.f } };
		CComputeShader* pAnimMixCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimMix")));
		Get_Component<CModel>()->Set_MixAnim(true);
		Get_Component<CModel>()->Set_MixAnim_ResetSize(1);
		Get_Component<CModel>()->Make_MixRatio(iFaceAnimIdx, vecMix, pAnimMixCS);
		Get_Component<CModel>()->Set_MixAnim_AnimIndex(0, iFaceAnimIdx);
	}

	return S_OK;
}

void CBoss_Xibi_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBoss_Xibi_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CBoss_Xibi_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CBoss_Xibi_Body::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CBoss_Xibi_Body::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CBoss_Xibi_Body::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CBoss_Xibi_Body::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CBoss_Xibi_Body::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CBoss_Xibi_Body::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

_bool CBoss_Xibi_Body::On_Hit(_uint iCollideMyLayer, ATTACK_DESC* pDesc, CGameObject* pOther)
{
	return true;
}

HRESULT CBoss_Xibi_Body::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

CBoss_Xibi_Body* CBoss_Xibi_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBoss_Xibi_Body* pInsatnce = new CBoss_Xibi_Body(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CBoss_Xibi_Body::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CBoss_Xibi_Body::Clone(void* pArg)
{
	CBoss_Xibi_Body* pInstance = new CBoss_Xibi_Body(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBoss_Xibi_Body::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Xibi_Body::Free()
{
	Super::Free();
}