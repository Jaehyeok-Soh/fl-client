#include "pch.h"
#include "Tool_PartObject.h"
#include "CameraMan.h"
#include "GameInstance.h"

Tool_PartObject::Tool_PartObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CToolObject(eType, pDevice, pDeviceContext)
{
}

Tool_PartObject::Tool_PartObject(const Tool_PartObject& rhs)
	:CToolObject(rhs)
{
}

HRESULT Tool_PartObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool_PartObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);
	m_pMatParent = pDesc->pMatParent;
	m_iToolObjectID = pDesc->iPartsID;

	return S_OK;
}

HRESULT Tool_PartObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void Tool_PartObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void Tool_PartObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void Tool_PartObject::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void Tool_PartObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT Tool_PartObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

_bool Tool_PartObject::Picking(OUT Vec3& vOut)
{
	return true;
}

void Tool_PartObject::Draw_ImGui()
{
	Super::Draw_ImGui();
}


void Tool_PartObject::Set_Parent(CGameObject* pGo)
{
	if (!pGo)
		return;

	m_pParentObject = pGo;
}

void Tool_PartObject::Update_CombinedWorldMatrix(const Matrix* pMatParent)
{
	m_CombineWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix() * (*pMatParent);
}

void Tool_PartObject::Update_CombinedWorldMatrix(Matrix matParent)
{
	m_CombineWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix() * matParent;
}

void Tool_PartObject::Update_CombinedWorldMatrix_Bilboad(Matrix matParent)
{
	m_CombineWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix() * matParent;
	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	_float fScaleX = m_CombineWorldMatrix.Right().Length();
	_float fScaleY = m_CombineWorldMatrix.Up().Length();
	_float fScaleZ = m_CombineWorldMatrix.Backward().Length();
	Vec3 vPosition = m_CombineWorldMatrix.Translation() + Vec3{ 0.f, 0.3f, 0.f };
	Vec3 vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vLook = vPosition - vCamPosition;
	vLook.Normalize();
	Vec3 vRight = Vec3::Up.Cross(vLook);
	vRight.Normalize();
	Vec3 vUp = vLook.Cross(vRight);
	vUp.Normalize();

	m_CombineWorldMatrix.Right(vRight * fScaleX);
	m_CombineWorldMatrix.Up(vUp * fScaleY);
	m_CombineWorldMatrix.Backward(vLook * fScaleZ);
	m_CombineWorldMatrix.Translation(vPosition);
}

void Tool_PartObject::Update_CombinedWorldMatrix_Bilboad(Matrix matParent, Vec2 vUIScale)
{
	m_CombineWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix() * matParent;
	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	Vec3 vPosition = m_CombineWorldMatrix.Translation() + Vec3{ 0.f, 0.3f, 0.f };
	Vec3 vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vLook = vPosition - vCamPosition;
	vLook.Normalize();
	Vec3 vRight = Vec3::Up.Cross(vLook);
	vRight.Normalize();
	Vec3 vUp = vLook.Cross(vRight);
	vUp.Normalize();

	m_CombineWorldMatrix.Right(vRight * vUIScale.x);
	m_CombineWorldMatrix.Up(vUp * vUIScale.y);
	m_CombineWorldMatrix.Backward(vLook);
	m_CombineWorldMatrix.Translation(vPosition);
}

void Tool_PartObject::Free()
{
	__super::Free();
}
