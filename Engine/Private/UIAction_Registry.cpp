#include "Engine_pch.h"
#include "UIAction_Registry.h"
#include "IUIActionForMe.h"

CUIAction_Registry::CUIAction_Registry()
{
}

void CUIAction_Registry::Initialize_CommonAction()
{
	// SetVisible : Params = { "Visible": bool }
	Register_Factory("SetVisible",
		[](const json& params) -> ActionFunc
		{
			const bool isVisible = params.value("Visible", true);

			return [isVisible](IUIActionForMe* me)
				{
					if (nullptr == me)
						return;
					me->Set_Visible(isVisible);
				};
		});

	// SetTextureIndex : Params = { "Index": uint }
	Register_Factory("SetTextureIndex",
		[](const json& params) -> ActionFunc
		{
			const uint32_t idx = params.value("Index", 0u);

			return [idx](IUIActionForMe* me)
				{
					if (nullptr == me)
						return;
					me->Set_TextureIndex(idx);
				};
		});
}

void CUIAction_Registry::Register_Factory(const _string& strActionName, FactoryFunc factory)
{
	if (strActionName.empty() || nullptr == factory)
		return;

	auto iter = m_Factories.find(strActionName);
	if (iter != m_Factories.end())
	{
		MSG_BOX("CUIAction_Registry::Register_Factory, Action Name Already Registerd");
		return;
	}
	m_Factories[strActionName] = std::move(factory);
}

CUIAction_Registry::ActionFunc CUIAction_Registry::Build_Action(const _string& strActionName, const json& params) const
{
	auto iter = m_Factories.find(strActionName);
	if (iter == m_Factories.end())
	{
		MSG_BOX("CUIAction_Registry::Build_Action, No Action ");
		return ActionFunc{};
	}

	return iter->second(params);
}

CUIAction_Registry* CUIAction_Registry::Create()
{
	CUIAction_Registry* pInstance = new CUIAction_Registry();
	pInstance->Initialize_CommonAction();
	return pInstance;
}

void CUIAction_Registry::Clear()
{
	m_Factories.clear();
}

void CUIAction_Registry::Free()
{
	Clear();
	Super::Free();
}
