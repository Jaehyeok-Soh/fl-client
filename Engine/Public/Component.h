#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;
class CGameInstance;

class ENGINE_DLL CComponent abstract : public CBase
{
	using Super = CBase;
#pragma region DESC
public:
	typedef struct tagComponentDesc
	{
		void* pTransform_Desc = { nullptr };
		void* pCamera_Desc = { nullptr };
	}COMPONENT_DESC;
#pragma endregion
protected:
	CComponent();
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;
	
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) PURE;
public:
	virtual void Render() {};
	_bool IsClone() const { return m_bClone; }
	virtual void Clear_WhenChangeLevel() {};
public:
	HRESULT Set_Owner(CGameObject* pOwner);
	CGameObject* Get_Owner() { return m_pOwner; }
private:
	_bool m_bClone = { false };
protected:
	CGameObject* m_pOwner = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
public:
	virtual CComponent* Clone(void *pArg) PURE;
	virtual void Free() override;
};

NS_END