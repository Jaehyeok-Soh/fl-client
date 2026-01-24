#pragma once
#include "Tool_ContainerObject.h"

NS_BEGIN(Tool)

class Effect :
    public Tool_ContainerObject
{
	using Super = Tool_ContainerObject;

protected:
	Effect(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Effect(const Tool_ContainerObject& rhs);
	virtual ~Effect() = default;

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
	virtual void Draw_ImGui() override;
	virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
	static Effect* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END