#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CNavigation;
class CStateBase;
NS_END

NS_BEGIN(Client)
class CStatComponent;
class CCameraMan_Targeter;
class CBody;

class CPlayer abstract : public CContainerObject
{
	using Super = CContainerObject;
public:
	typedef struct tagPlayerDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrBodyModelTag = { L"" };
		wstring wstrNavigationPrototypeTag = { L"" };
		_int iNavigationCellIndex = { -1 };
		_float3 vSpawnPosition = {};
	}PLAYER_DESC;
	enum Part : _uint
	{
		BODY = 0,
		WEAPON,
		RIGHTHAND,
		LEFTHAND,
		RIGHTFOOT,
		LEFTFOOT,
		END
	};
	enum class State : _uint
	{
		IDLE,
		RUNSTART,
		RUN,
		RUNEND,
		LEFTMELEE,
		END
	};
protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _int Get_AnimationIndex(const wstring& wstrName) override;
private:
	_vector Get_Dir(EDir eDir);
	HRESULT Ready_BaseStates();
	HRESULT Ready_PartObjects(PLAYER_DESC* pDesc);
	HRESULT Ready_Components(PLAYER_DESC* pDesc);
protected:
	CStatComponent* m_pStatComp = { nullptr };
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END
