#pragma once
#include "ToolObject.h"

NS_BEGIN(Tool)

class Tool_PartObject :
    public CToolObject
{
	using Super = CToolObject;
public:
	typedef struct tagPartObjectDesc : public Super::TOOLOBJECT_DESC
	{
		const Matrix* pMatParent = { nullptr };
	}PARTOBJ_DESC;

protected:
	Tool_PartObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Tool_PartObject(const Tool_PartObject& rhs);
	virtual ~Tool_PartObject() = default;

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
	virtual void Draw_ImGui();
	virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
	void Set_Parent(CGameObject* pGo);
	CGameObject* Get_Parent() { return m_pParentObject; }

protected:
	CGameObject*	 m_pParentObject = { nullptr };
	Matrix        m_CombineWorldMatrix = {};
	const Matrix* m_pMatParent = { nullptr };

protected:
	void Update_CombinedWorldMatrix(const Matrix* pMatParent);
	void Update_CombinedWorldMatrix(Matrix matParent);
	void Update_CombinedWorldMatrix_Bilboad(Matrix matParent);
	void Update_CombinedWorldMatrix_Bilboad(Matrix matParent, Vec2 vUIScale);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END