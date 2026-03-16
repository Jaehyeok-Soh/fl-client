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

	void Set_Interact_DefaultEnable() { m_bIsInteractDefaultEnabled = true; }
	void Set_Interact_DefaultDisable() { m_bIsInteractDefaultEnabled = false; }

	_bool Is_Interact_DefaultEnabled() { return m_bIsInteractDefaultEnabled; }

	virtual void Interact() PURE;

	void Interact_SetDefaultDialogue(_int iID) { m_iDefaultDialogueId = iID; }

	//virtual void InteractEnter() PURE;
	//virtual void InteractExit() PURE;

protected:
	_bool m_bIsInteractEnabled = { true };
	_bool m_bIsInteractDefaultEnabled = { true };

	_int m_iDefaultDialogueId = { -1 };
	//_int m_iEnterDialogueId = { -1 };
	//_int m_iExitDialogueId = { -1 };
};

NS_END