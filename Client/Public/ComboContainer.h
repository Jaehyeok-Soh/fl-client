#pragma once
#include "ComboState.h"

NS_BEGIN(Client)

class CComboContainer final : public CStateBase
{
	using Super = CStateBase;
	enum class ComboKeyState : unsigned short
	{
		MOUSE = 0,
		KEY,
		NONE
	};
protected:
	CComboContainer(CActionState* pOwnerComponent, const string& strName);
	virtual ~CComboContainer() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
	HRESULT Add_Comobo(CComboState* pComobo);
	virtual _uint Get_Capabilities() const override
	{
		return m_vecComboes[m_iCurrentComboIndex]->Get_Capabilities();
	}
private:
	void Setup_Combo(CComboState* pCombo);
	_bool Goto_NextCombo(void* pArg);
	_bool Get_Input();
protected:
	ComboKeyState m_eKeyState = { ComboKeyState::NONE };
	_int m_iCurrentRootState = { -1 };
	_int m_iCurrentComboIndex = { -1 };
	_bool m_bCurrentLeftMouseInput = { false };
	_int m_iCurrentDIKInput = { -1 };
	vector<CComboState*> m_vecComboes;
public:
	static CComboContainer* Create(CActionState* pOwnerComponent, const string &strName);
	virtual void Free() override;
};

NS_END