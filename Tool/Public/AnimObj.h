#pragma once
#include "Tool_ContainerObject.h"

/* 애니메이션 툴에서 사용 할 애니메이션 모델*/

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Tool)
class CAnimObj final : public Tool_ContainerObject
{
	using Super = Tool_ContainerObject;

public:
	typedef struct tagAnimObjDesc : public Tool_ContainerObject::TOOLOBJECT_DESC
	{
		_wstring wstrModelProtoTag = TEXT("");
	}ANIMOBJ_DESC;

private:
	CAnimObj(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAnimObj(const CAnimObj& rhs);
	virtual ~CAnimObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void	Update_Priority(const _float fTimeDelta) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual void	Update_Late(const _float fTimeDelta) override;
	virtual void	Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _bool	Picking(OUT Vec3& vOut) override;
	virtual void	Draw_ImGui()override;
	virtual void	Set_Dead(const wstring& wstrLayerTag) override;

public:
	const CModel*	Get_ModelComPtr(); // 모델 컴포넌트를 가져와야 하므로 계속 접근하게 한다.
	HRESULT			Change_ModelCom(_wstring wstrModelTag);

private:
	HRESULT			Ready_Components(ANIMOBJ_DESC* pDesc);
	HRESULT			Ready_ComputeShaders();

public:
	static  CAnimObj* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};
NS_END