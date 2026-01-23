#include "StaticModel.h"

CStaticModel::CStaticModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(eType, pDevice,pDeviceContext)
{
}

CStaticModel::CStaticModel(const CStaticModel& rhs)
	: CMapObject(rhs)
{
}

HRESULT CStaticModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CStaticModel::Awake(const _uint iCurrentLevelID)
{
	return S_OK;
}

void CStaticModel::Update_Priority(const _float fTimeDelta)
{
}

void CStaticModel::Update(const _float fTimeDelta)
{
}

void CStaticModel::Update_Late(const _float fTimeDelta)
{
}

void CStaticModel::Ready_Before_Render(const _float fTimeDelta)
{
}

HRESULT CStaticModel::Render()
{
	return E_NOTIMPL;
}

void CStaticModel::Draw_ImGui()
{
}

void CStaticModel::Set_Dead(const wstring& wstrLayerTag)
{
}

_bool CStaticModel::Picking(OUT _float4& vOut)
{
	return _bool();
}

HRESULT CStaticModel::Export_Data(OUT MAPOBJECT_SAVEDATA& data)
{
	return E_NOTIMPL;
}
