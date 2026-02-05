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

			return [isVisible](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;
					me->Set_Visible(isVisible);
				};
		});

	Register_Factory((DTO::EUIAction::SET_TEXTURE_INDEX),
		[](const json& params) -> ActionFunc
		{
			const uint32_t idx = params.value("uIndex", 0u);

			return [idx](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;
					me->Set_TextureIndex(idx);
				};
		});

	Register_Factory((DTO::EUIAction::START_LERP_MOVEMENT),
		[](const json& params) -> ActionFunc
		{
			Vec3 vTargetPos = Vec3{ 0.f, 0.f, 0.f };
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

			return [vTargetPos, fTargetAlpha, fDuration, isPin](IUIActionForMe* me, IUIActionForTarget*)
				{
					if (nullptr == me)
						return;

					me->Start_Lerp_Movement(vTargetPos, fTargetAlpha, fDuration, isPin);
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
			return [iLevelIndex,strCanvasTag](IUIActionForMe*, IUIActionForTarget* target)
				{
					if (nullptr == target)
						return;

					target->Trigger_All_Canvas(iLevelIndex, strCanvasTag);
				};
		});

	Register_Factory((DTO::EUIAction::TRIGGER_ALL_LAYER),
		[](const json& params) -> ActionFunc
		{
			const uint32_t iLevelIndex = params.value("iLevelIndex", 0u);
			const _string strLayerTag = params.value("strLayerTag", "");
			return [iLevelIndex,strLayerTag](IUIActionForMe*, IUIActionForTarget* target)
				{
					if (nullptr == target)
						return;

					target->Trigger_All_Layer(iLevelIndex,strLayerTag);
				};
		});

	Register_Factory((DTO::EUIAction::TRIGGER_TARGET_UI),
		[](const json& params) -> ActionFunc
		{
			const uint32_t iLevelIndex = params.value("iLevelIndex", 0u);
			const _string strUITag = params.value("strUITag", "");
			return [iLevelIndex,strUITag](IUIActionForMe*, IUIActionForTarget* target)
				{
					if (nullptr == target)
						return;

					target->Trigger_TargetUI(iLevelIndex,strUITag);
				};
		});
}

void CUIAction_Registry::Register_Factory(DTO::EUIAction FuncType, FactoryFunc factory)
{
	size_t index = ENUM_TO_SZET(FuncType);
	if (index >= m_Factories.size() || !factory)
		return;

	m_Factories[index] = std::move(factory);
}

CUIAction_Registry::ActionFunc CUIAction_Registry::Build_Action(DTO::EUIAction FuncType, const json& params) const
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