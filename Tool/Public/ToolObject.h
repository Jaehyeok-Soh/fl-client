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
	virtual _bool Picking(OUT _float4& vOut) PURE;
	virtual HRESULT Export_Data(OUT MAPOBJECT_SAVEDATA &data) PURE;
	virtual void Draw_ImGui();
	virtual void Set_Dead(const wstring& wstrLayerTag) override;
public:
	virtual void Set_Visible();
	virtual void Set_Invisible();
	_bool Is_Visible() const { return m_bVisible; }
	_bool Is_Type(EToolObjectType eType) { return m_eType == eType; }
	const string& Get_Name() const { return m_strName; }
	const string& Get_TypeName() const { return m_strObjectType; }
	EToolObjectType Get_Type() const { return m_eType; }
	const wstring& Get_LayerTag() const { return m_wstrLayerTag; }
public:
	void Set_Name(const string& strName);
	void Set_Name(const wstring& wstrName);
protected:
	void Set_Name(const string& strName, _uint iValue);
	void Set_Name(const wstring& wstrName, _uint iValue);
	void Update_CombinedWorldMatrix(const _float4x4* pMatParent);
	void Update_CombinedWorldMatrix(_fmatrix matParent);
	void Export_TransformData(OUT TRANSFORM_SAVEDATA& data);
	void Export_ModelData(OUT MODEL_SAVEDATA& data);
private:
	HRESULT Set_TypeString();
protected:
	string m_strName = "";
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