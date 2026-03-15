#pragma once

NS_BEGIN(Client)

class IInteractable abstract
{
public:
	virtual ~IInteractable() = default;

public:
	void Set_Interact_Enable() { m_bIsInteractEnabled = true; }
	void Set_Interact_Disable() { m_bIsInteractEnabled = false; }

	_bool Is_Interact_Enabled() { return m_bIsInteractEnabled; }

	virtual void Interact() PURE;

	//virtual void InteractEnter() PURE;
	//virtual void InteractExit() PURE;

protected:
	_bool m_bIsInteractEnabled = { true };

	_int m_iDefaultDialogueId = { -1 };
	//_int m_iEnterDialogueId = { -1 };
	//_int m_iExitDialogueId = { -1 };
};

NS_END