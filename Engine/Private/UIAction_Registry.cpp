#include "Engine_pch.h"
#include "UIAction_Registry.h"
#include "IUIActionForMe.h"
#include "IUIActionForTarget.h"

NS_BEGIN(Engine)

CUIAction_Registry::CUIAction_Registry()
{
}

void CUIAction_Registry::Initialize_CommonAction()
{
	m_Factories.resize(ENUM_TO_UINT(DTO::EUIAction::END));

	Register_Factory((DTO::EUIAction::SET_VISIBLE),
		[](const json& params) -> ActionFunc
		{
			const bool isVisible = params.value("isVisible", true);
			const _float fDelay = params.value("fDelay", 0.f);
			return [isVisible, fDelay](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;
					me->Set_Visible(isVisible, fDelay);
				};
		});

	Register_Factory((DTO::EUIAction::SET_TEXTURE_INDEX),
		[](const json& params) -> ActionFunc
		{
			const uint32_t idx = params.value("uIndex", 0u);

			const _float fDelay = params.value("fDelay", 0.f);
			return [idx, fDelay](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;
					me->Set_TextureIndex(idx, fDelay);
				};
		});

	Register_Factory((DTO::EUIAction::START_LERP_MOVEMENT),
		[](const json& params) -> ActionFunc
		{
			Vec3 vTargetPos = Vec3{ 0.f, 0.f, 0.f };
			const _float fDelay = params.value("fDelay", 0.f);
			if (params.contains("vTargetPos"))
			{
				const auto& jPos = params["vTargetPos"];
				vTargetPos.x = jPos.value("x", 0.f);
				vTargetPos.y = jPos.value("y", 0.f);
				vTargetPos.z = jPos.value("z", 0.f);
			}

			const _float fTargetAlpha = params.value("fTargetAlpha", 1.f);
			const _float fDuration = params.value("fDuration", 0.f);
			const _bool isPin = params.value("isPin", false);

			return [vTargetPos, fTargetAlpha, fDuration, isPin, fDelay](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;

					me->Start_Lerp_Movement(vTargetPos, fTargetAlpha, fDuration, isPin, fDelay);
				};
		});

	Register_Factory((DTO::EUIAction::START_RETURN_LERP_MOVEMENT),
		[](const json& params) -> ActionFunc
		{
			const _float fDelay = params.value("fDelay", 0.f);
			return [fDelay](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;
					me->Start_Return_Lerp_Movement(fDelay);
				};
		});

	Register_Factory((DTO::EUIAction::START_FADE),
		[](const json& params) -> ActionFunc
		{
			const _float fStartAlpha = params.value("fStartAlpha", 0.f);
			const _float fTargetAlpha = params.value("fTargetAlpha", 1.f);
			const _float fDuration = params.value("fDuration", 0.f);

			const _float fDelay = params.value("fDelay", 0.f);
			return [fStartAlpha, fTargetAlpha, fDuration, fDelay](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;

					me->Start_Fade(fStartAlpha, fTargetAlpha, fDuration, fDelay);
				};
		});
}

void CUIAction_Registry::Initialize_CommonTargetAction()
{
	Register_Factory((DTO::EUIAction::TRIGGER_ALL_CANVAS),
		[](const json& params) -> ActionFunc
		{
			const uint32_t iLevelIndex = params.value("iLevelIndex", 0u);
			const _string strCanvasTag = params.value("strCanvasTag", "");
			DTO::EUIAction eAction = params.value("eAction", DTO::EUIAction::END);
			const json jTargetActionParam = params.value("jTargetActionParam", json::object());
			return [iLevelIndex,strCanvasTag, eAction, jTargetActionParam](IUIActionForMe*, IUIActionForTarget* target)
				{
					if (nullptr == target)
						return;

					target->Trigger_All_Canvas(iLevelIndex, strCanvasTag, eAction, jTargetActionParam);
				};
		});

	Register_Factory((DTO::EUIAction::TRIGGER_TARGET_UI),
		[](const json& params) -> ActionFunc
		{
			const uint32_t iLevelIndex = params.value("iLevelIndex", 0u);
			const _string strUITag = params.value("strUITag", "");
			DTO::EUIAction eAction = params.value("eAction", DTO::EUIAction::END);
			const json jTargetActionParam = params.value("jTargetActionParam", json::object());
			return [iLevelIndex,strUITag, eAction, jTargetActionParam](IUIActionForMe*, IUIActionForTarget* target)
				{
					if (nullptr == target)
						return;

					target->Trigger_TargetUI(iLevelIndex,strUITag, eAction, jTargetActionParam);
				};
		});
}

void CUIAction_Registry::Register_Factory(DTO::EUIAction ActionType, FactoryFunc factory)
{
	size_t index = ENUM_TO_SZET(ActionType);
	if (index >= m_Factories.size() || !factory)
		return;

	m_Factories[index] = std::move(factory);
}

CUIAction_Registry::ActionFunc CUIAction_Registry::Build_Action(DTO::EUIAction ActionType, const json& params) const
{
	size_t index = ENUM_TO_SZET(ActionType);
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
	pInstance->Initialize_CommonTargetAction();
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

NS_END