#include "Engine_pch.h"
#include "UIAction_Registry.h"
#include "IUIActionForMe.h"

CUIAction_Registry::CUIAction_Registry()
{
}

void CUIAction_Registry::Initialize_CommonAction()
{
	m_Factories.resize(ENUM_TO_UINT(DTO::EUIFunc::END));

	Register_Factory((DTO::EUIFunc::SET_VISIBLE),
		[](const json& params) -> ActionFunc
		{
			const bool isVisible = params.value("isVisible", true);

			return [isVisible](IUIActionForMe* me)
				{
					if (nullptr == me)
						return;
					me->Set_Visible(isVisible);
				};
		});

	Register_Factory((DTO::EUIFunc::SET_TEXTURE_INDEX),
		[](const json& params) -> ActionFunc
		{
			const uint32_t idx = params.value("index", 0u);

			return [idx](IUIActionForMe* me)
				{
					if (nullptr == me)
						return;
					me->Set_TextureIndex(idx);
				};
		});
}

void CUIAction_Registry::Register_Factory(DTO::EUIFunc FuncType, FactoryFunc factory)
{
	size_t index = ENUM_TO_SZET(FuncType);
	if (index >= m_Factories.size() || !factory)
		return;

	m_Factories[index] = std::move(factory);
}

CUIAction_Registry::ActionFunc CUIAction_Registry::Build_Action(DTO::EUIFunc FuncType, const json& params) const
{
	size_t index = ENUM_TO_SZET(FuncType);
	if (index >= m_Factories.size())
		return ActionFunc{};

	const auto& Factory = m_Factories[index];
	if (!Factory)
		return ActionFunc{};

	return Factory(params);
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
