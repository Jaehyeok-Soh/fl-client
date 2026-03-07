#include "pch.h"
#include "Builder_Effect.h"
#include "DataDocument_Effect.h"
#include "ContainerObject.h"
#include "Effect.h"
#include "EffectObject.h"
#include "GameInstance.h"

CBuilder_Effect::CBuilder_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	: Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_Effect::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_Effect::Build(const CDataDocumentBase& document)
{
	if (document.Get_Category() != DTO::ECategory::EFFECT)
		return E_FAIL;

	const auto& doc = static_cast<const CDataDocument_Effect&>(document);
	// For. Effect_Container
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EEffectType::EFFECT_CONTAINER));
		for (const auto& pObjectData : vecList)
		{
			const auto* pEffectData = static_cast<const Engine::CEFFECT_CONTAINER*>(pObjectData);
 			if (FAILED(Create_Effect(pEffectData->Get_Data())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBuilder_Effect::Create_Effect(const DTO::TEFFECT_ContainerData& data)
{
	if (data.eType != DTO::EEffectType::EFFECT_CONTAINER)
		return E_FAIL;

	DTO::TEFFECT_ContainerData pData = data;

	// 부모 객체 생성 과정
		// worldmatrix 분해해서 transform에 굳이굳이 넣기.
	Vec3 vScale, vPos, vRot;
	Quat vQuat;
	pData.vWorldMatrix.Decompose(vScale, vQuat, vPos);

	CTransform::TRANSFORM_DESC pTransDesc = {};
	pTransDesc.fMovePerSec = 1.f;
	pTransDesc.fRotatePerSec = 1.f;
	pTransDesc.ScaleMatrix = Matrix::CreateScale(Vec3(vScale));
	pTransDesc.RotationMatrix = Matrix::CreateFromQuaternion(vQuat);
	pTransDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(vPos));

	Effect::EFFECT_CONTAINERDESC pDesc = {};
	pDesc._Effect_SimulationType = (DTO::E_SIMULATION_SPACE)pData._Effect_SimulationType;
	pDesc.iLevelIndex = m_iLevelID;
	pDesc.pTransform_Desc = &pTransDesc;
	pDesc._childData = pData._ChildData;

	Regist_pool(&pDesc, pData.EffectContainerName);

	return S_OK;
}

const wstring CBuilder_Effect::Create_PrototypeTag(const string Tag)
{
	wstring PoolTag = {};
	wstring LayTag = {};
	wstring PrototypeTag = {};

	if (Tag.find("Warning") != std::string::npos)
		PrototypeTag = L"Prototype_GameObject_Effect_WarningCircle";

	else
		PrototypeTag = L"Prototype_GameObject_Effect";

	return PrototypeTag;
}

void CBuilder_Effect::Regist_pool(void* pArg, string& PrefabEffectTag)
{
	wstring PoolTag = L"POOL_" + Engine_Utils::ToWString(PrefabEffectTag);
	wstring LayTag = L"Effect_Layer";
	wstring PrototypeTag = Create_PrototypeTag(PrefabEffectTag);

	m_pGameInstance->Push_EffectData(Engine_Utils::ToHash(PrefabEffectTag.c_str()), pArg);

	if (PrefabEffectTag == "Boss_Xibi_Lightning_Oneshot")
		m_pGameInstance->Regist_Pool(m_iLevelID, PoolTag, LayTag, 0, PrototypeTag, pArg, 100);

	else
		m_pGameInstance->Regist_Pool(m_iLevelID, PoolTag, LayTag, 0, PrototypeTag, pArg, 30);

}

CBuilder_Effect* CBuilder_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_Effect* pInstance = new CBuilder_Effect(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_Effect::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_Effect::Free()
{
	Super::Free();
}
