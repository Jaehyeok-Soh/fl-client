#include "pch.h"
#include "Builder_AttackOverlap.h"
#include "GameInstance.h"
#include "DataDocument_AttackOverlap.h"
#include "PhysicsAttackOverlap.h"

CBuilder_AttackOverlap::CBuilder_AttackOverlap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	:Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_AttackOverlap::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_AttackOverlap::Build(const CDataDocumentBase& document)
{
	if (document.Get_Category() != DTO::ECategory::OVERLAP_SCRIPT)
		return E_FAIL;

	const auto& doc = static_cast<const CDataDocument_AttackOverlap&>(document);

	// For. DataOverlap
	{
		/* 문서에 저장된 IObjectDataBase -> 데이터를 가진 클래스의 부모 */
		const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(DTO::ATTACKOVERLAP_TYPE::NORMAL);
		for (const auto& pDtoBase : vecDtoList)
		{
			/* 데이터를 보유한 클래스 다운캐스팅 */
			const auto* pDto = static_cast<const Engine::CDataStruct_AttackOverlap*>(pDtoBase);

			/* 프로토타입 태그는 캐릭터(플레이어, 몬스터)와 1:1 연결될 수 있게 */
			if (FAILED(Create_AttackOverlap_Prototype(pDto->Get_Data())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBuilder_AttackOverlap::Create_AttackOverlap_Prototype(const DTO::ATTACKOVERLAP_DESC& data)
{
	DTO::ATTACKOVERLAP_DESC desc{};
	desc.iNumPool = data.iNumPool;
	
	for (auto& event : data.attackEvents)
	{
		//DTO::HITBOX_DESC hbDesc{};
		//hbDesc.strName = event.tHitboxDesc.strName;
		//hbDesc.eType = (OverlapType::Enum)event.tHitboxDesc.eType;
		//hbDesc.vExtents = event.tHitboxDesc.vExtents;
		//hbDesc.fRadius = event.tHitboxDesc.fRadius;
		//hbDesc.vOffset = event.tHitboxDesc.vOffset;
		//hbDesc.fDuration = event.tHitboxDesc.fDuration;
		//hbDesc.fTickTime = event.tHitboxDesc.fTickTime;
		//hbDesc.fDamage = event.tHitboxDesc.fDamage;
		//hbDesc.iMaxHit = event.tHitboxDesc.iMaxHit;
		//hbDesc.eFilterLayer = event.tHitboxDesc.eFilterLayer;
		//hbDesc.iFilterMask = event.tHitboxDesc.iFilterMask;

		//DTO::ATTACKEVENT ae{};
		//ae.eEventType = AnimEvent::Enum::OVERLAP;
		//ae.strDescription = event.strDescription;
		//ae.strAnimTag = event.strAnimTag;
		//ae.iAnimIndex = event.iAnimIndex;

		desc.attackEvents.emplace_back(event);
	}

	// For. Prototype_Component_AttackOverlap_{ CharacterName }
	wstring prototypeTag(L"Prototype_Component_AttackOverlap_" + Engine_Utils::ToWString(data.strTag));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_AttackOverlap_PlayerMoon", CPhysicsAttackOverlap::Create(&desc));

	return S_OK;
}

CBuilder_AttackOverlap* CBuilder_AttackOverlap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_AttackOverlap* pInstance = new CBuilder_AttackOverlap(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_AttackOverlap::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_AttackOverlap::Free()
{
	Super::Free();
}
