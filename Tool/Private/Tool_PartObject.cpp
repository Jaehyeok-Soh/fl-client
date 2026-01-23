#include "Tool_PartObject.h"
#include "GameInstance.h"
#include "CameraMan.h"

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

_bool Tool_PartObject::Picking(OUT _float4& vOut)
{
	return true;
}

HRESULT Tool_PartObject::Export_Data(OUT MAPOBJECT_SAVEDATA& data)
{
	return S_OK;
}

void Tool_PartObject::Set_Dead(const wstring& wstrLayerTag)
{
	Super::Set_Dead(wstrLayerTag);
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

void Tool_PartObject::Update_CombinedWorldMatrix(const _float4x4* pMatParent)
{
	::XMStoreFloat4x4(&m_CombineWorldMatrix, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * ::XMLoadFloat4x4(pMatParent));
}

void Tool_PartObject::Update_CombinedWorldMatrix(_fmatrix matParent)
{
	::XMStoreFloat4x4(&m_CombineWorldMatrix, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * matParent);
}


void Tool_PartObject::Update_CombinedWorldMatrix_Bilboad(_fmatrix matParent)
{
	::XMStoreFloat4x4(&m_CombineWorldMatrix, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * matParent);

	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	_matrix matWorld = ::XMLoadFloat4x4(&m_CombineWorldMatrix);
	_float fScaleX = ::XMVectorGetX(::XMVector3Length(matWorld.r[0]));
	_float fScaleY = ::XMVectorGetX(::XMVector3Length(matWorld.r[1]));
	_float fScaleZ = ::XMVectorGetX(::XMVector3Length(matWorld.r[2]));
	_vector vPosition = matWorld.r[3] + ::XMVectorSet(0.f, 0.3f, 0.f, 0.f);
	_vector vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vLook = ::XMVector3Normalize(vPosition - vCamPosition);
	_vector vRight = ::XMVector3Normalize(::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = ::XMVector3Normalize(::XMVector3Cross(vLook, vRight));
	matWorld.r[0] = ::XMVectorSetW(vRight * fScaleX, 0.f);
	matWorld.r[1] = ::XMVectorSetW(vUp * fScaleY, 0.f);
	matWorld.r[2] = ::XMVectorSetW(vLook * fScaleZ, 0.f);
	matWorld.r[3] = ::XMVectorSetW(vPosition, 1.f);

	::XMStoreFloat4x4(&m_CombineWorldMatrix, matWorld);
}

void Tool_PartObject::Update_CombinedWorldMatrix_Bilboad(_fmatrix matParent, _float2 vUIScale)
{
	::XMStoreFloat4x4(&m_CombineWorldMatrix, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * matParent);

	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	_matrix matWorld = ::XMLoadFloat4x4(&m_CombineWorldMatrix);
	_vector vPosition = matWorld.r[3] + ::XMVectorSet(0.f, 0.3f, 0.f, 0.f);
	_vector vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vLook = ::XMVector3Normalize(vPosition - vCamPosition);
	_vector vRight = ::XMVector3Normalize(::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = ::XMVector3Normalize(::XMVector3Cross(vLook, vRight));
	matWorld.r[0] = ::XMVectorSetW(vRight * vUIScale.x, 0.f);
	matWorld.r[1] = ::XMVectorSetW(vUp * vUIScale.y, 0.f);
	matWorld.r[2] = ::XMVectorSetW(vLook, 0.f);
	matWorld.r[3] = ::XMVectorSetW(vPosition, 1.f);

	::XMStoreFloat4x4(&m_CombineWorldMatrix, matWorld);
}

void Tool_PartObject::Free()
{
	__super::Free();
}
