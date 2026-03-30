#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUIPrefab;



class CUIQTE_Manager final : public CBase
{
	DECLARE_SINGLETON(CUIQTE_Manager)
	using Super = CBase;

	enum class EQTETiming
	{
		FAST, PERFECT, SLOW, FAIL, END
	};

private:
	CUIQTE_Manager();
	virtual ~CUIQTE_Manager() = default;

public:
	void Start_QTE(_uint iNodeCount);
	void Tick_QTE(const _float fTimeDelta);

	void Success_QTE();
	void Fail_QTE();

	_uint Get_CurrentNodeIndex() const { return m_iCurrentNodeIndex; }

	void Set_Current_Node_Progress(EQTETiming eType);

private:
	_uint Get_Random_Key();
	void Tick_Show_NextNode(const _float fTimeDelta);

	void Init_Vaules();

private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<UI_QTE_PREFAB_DATA> m_vecQTEData;
	_bool m_isStartQTE = { false };

	_float m_fTimeAcc = {};

	_uint m_iCurrentNodeIndex = {};
	_uint m_iMaxNodeCount = {};

	_float m_fShowTimeAcc = {};

	_uint m_iNextShowedNodeIndex = {};

	_bool m_isLastNodeShowed = { false };

	EQTETiming m_eCurrentQTETIming = { EQTETiming::END };

public:
	virtual void Free()override;
};

NS_END