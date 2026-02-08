#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"
#include "FileUtils.h"

NS_BEGIN(Tool)

class CDummy_Collider;

class CToolObject abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagToolObjectDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrLayerTag = L"";
	}TOOLOBJECT_DESC;
protected:
	CToolObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToolObject(const CToolObject& rhs);
	virtual ~CToolObject() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _bool Picking(OUT Vec3& vOut) PURE;
	virtual void Draw_ImGui();
	virtual void Set_Dead(const wstring& wstrLayerTag) override;

	virtual bool	Get_SRT( OUT  Vec3& vOutScale , OUT Quat& vQuat , OUT Vec3& vPosition  );
	
	virtual void	Set_WorldMatrix(const Matrix& WorldMatrix);
	virtual void	Set_WorldMatrix(const Vec3& vScale, const Quat& vQuat ,const Vec3& vPosition);
	virtual Matrix  Get_WorldMatrix();
public:
	virtual void Set_Visible();
	virtual void Set_Invisible();
	_bool Is_Visible() const { return m_bVisible; }
	_bool Is_Type(EToolObjectType eType) { return m_eType == eType; }
	const string& Get_Name() const { return m_strName; }
	const string& Get_TypeName() const { return m_strObjectType; }
	EToolObjectType Get_Type() const { return m_eType; }
	const string& Get_ModelFileName() const { return m_strModelFileName; }
	const wstring& Get_LayerTag() const { return m_wstrLayerTag; }
	void Update_CombinedWorldMatrix(const Matrix &matParent);
private:
	HRESULT Set_TypeString();
protected:
	string m_strModelFileName = "";
	string m_strObjectType = "";
	wstring m_wstrLayerTag = L"";
private:
	EToolObjectType m_eType = { EToolObjectType::END };
	_bool m_bVisible = { false };
public:
	virtual void Free() override;
};

NS_END