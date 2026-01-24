#pragma once
#include "ToolObject.h"

NS_BEGIN(Tool)

class Tool_PartObject;

class Tool_ContainerObject :
    public CToolObject
{
	using Super = CToolObject;

protected:
	Tool_ContainerObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Tool_ContainerObject(const Tool_ContainerObject& rhs);
	virtual ~Tool_ContainerObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _bool Picking(OUT Vec3& vOut) override;
	virtual HRESULT Export_Data(OUT MAPOBJECT_SAVEDATA& data) override;
	virtual void Draw_ImGui();
	virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
	template<typename T>
	T* Get_Part(_uint iPartID);

	void Remove_Part(_uint iPartID);
	HRESULT Add_Part(class Tool_PartObject* pPart, _uint iPartID);
	HRESULT Add_Part(_uint iPartID, _uint iPrototypeLevelIndex, const  wstring& wstrPrototypeTag, void* pArg);
	HRESULT Change_Part(class Tool_PartObject* pPart, _uint iPartID);
	HRESULT Change_Part(_uint iPartID, _uint iPrototypeLevelIndex, const  wstring& wstrPrototypeTag, void* pArg);
protected:
	vector<class Tool_PartObject*> m_vecPartObjects;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

template<typename T>
T* Tool_ContainerObject::Get_Part(_uint iPartID)
{
	if (!m_vecPartObjects[iPartID])
		return nullptr;

	return static_cast<T*>(m_vecPartObjects[iPartID]);
}


NS_END