#include "Tool_ContainerObject.h"
#include "Tool_PartObject.h"
#include "GameInstance.h"

Tool_ContainerObject::Tool_ContainerObject(EToolObjectType EType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CToolObject(EType, pDevice, pDeviceContext)
{

}

Tool_ContainerObject::Tool_ContainerObject(const Tool_ContainerObject& rhs)
	:CToolObject(rhs)
{

}

HRESULT Tool_ContainerObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool_ContainerObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool_ContainerObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Awake(iCurrentLevelID);
	}

}

void Tool_ContainerObject::Update_Priority(const _float fDT)
{
	Super::Update_Priority(fDT);

	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Update_Priority(fDT);
	}
}

void Tool_ContainerObject::Update(const _float fDT)
{
	Super::Update(fDT);
	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Update(fDT);
	}
}

void Tool_ContainerObject::Update_Late(const _float fDT)
{
	Super::Update_Late(fDT);
	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Update_Late(fDT);
	}
}

void Tool_ContainerObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Ready_Before_Render(fTimeDelta);
	}

}

HRESULT Tool_ContainerObject::Render()
{
	return S_OK;
}

_bool Tool_ContainerObject::Picking(OUT _float4& vOut)
{
	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			return pPart->Picking(vOut);
	}

}

HRESULT Tool_ContainerObject::Export_Data(OUT MAPOBJECT_SAVEDATA& data)
{
	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			return pPart->Export_Data(data);
	}
}

void Tool_ContainerObject::Draw_ImGui()
{
	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Draw_ImGui();
	}
	
}
void Tool_ContainerObject::Set_Dead(const wstring& wstrLayerTag)
{
	Super::Set_Dead(wstrLayerTag);

	for (Tool_PartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Set_Dead(wstrLayerTag);
	}
}


void Tool_ContainerObject::Remove_Part(_uint iPartID)
{
	if (m_vecPartObjects[iPartID])
		Safe_Release(m_vecPartObjects[iPartID]);

	m_vecPartObjects[iPartID] = nullptr;
}

HRESULT Tool_ContainerObject::Add_Part(Tool_PartObject* pPart, _uint iPartID)
{
	if (m_vecPartObjects[iPartID])
		return E_FAIL;

	pPart->Set_Parent(this);
	m_vecPartObjects[iPartID] = pPart;
	return S_OK;
}

HRESULT Tool_ContainerObject::Add_Part(_uint iPartID, _uint iPrototypeLevelIndex, const  wstring& wstrPrototypeTag, void* pArg)
{
	if (m_vecPartObjects[iPartID])
		return E_FAIL;

	if (CBase* pClone = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg))
	{
		Tool_PartObject* pPartObject = static_cast<Tool_PartObject*>(pClone);
		pPartObject->Set_Parent(this);
		m_vecPartObjects[iPartID] = pPartObject;
		return S_OK;
	}

	return E_FAIL;
}

HRESULT Tool_ContainerObject::Change_Part(Tool_PartObject* pPart, _uint iPartID)
{
	if (!pPart)
		return E_FAIL;

	if (m_vecPartObjects[iPartID])
		Safe_Release(m_vecPartObjects[iPartID]);

	m_vecPartObjects[iPartID] = pPart;
	return S_OK;
}

HRESULT Tool_ContainerObject::Change_Part(_uint iPartID, _uint iPrototypeLevelIndex, const  wstring& wstrPrototypeTag, void* pArg)
{
	if (m_vecPartObjects[iPartID])
		Safe_Release(m_vecPartObjects[iPartID]);

	if (CBase* pClone = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg))
	{
		Tool_PartObject* pPartObject = static_cast<Tool_PartObject*>(pClone);
		m_vecPartObjects[iPartID] = pPartObject;
		return S_OK;
	}
	return E_FAIL;
}


void Tool_ContainerObject::Free()
{
	Super::Free();
}