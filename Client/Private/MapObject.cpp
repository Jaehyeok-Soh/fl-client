#include "pch.h"
#include "Model.h"
#include "Shader.h"
#include "MapObject.h"
#include "GameInstance.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext), m_eMapObjectType{EMapObject_Type::END}
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs), m_eMapObjectType(rhs.m_eMapObjectType)
{
}


HRESULT	CMapObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT	CMapObject::Initialize(void* pArg)
{
	if(FAILED(Super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

HRESULT	CMapObject::Ready_Component()
{
	return S_OK;
}

HRESULT	CMapObject::Add_MapToolComponent(CMapObject::COMPONENT eType)
{
	return S_OK;
}

HRESULT	CMapObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void	CMapObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}
void	CMapObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}
void	CMapObject::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);



}
void	CMapObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}



HRESULT CMapObject::Ready_OverrideMtl(const DTO::USING_MODEL_INFO& tUsingModelInfo)
{

    if (tUsingModelInfo.vecOverrideMaterial.empty())
        m_iUseOverrideMaterials = false;
    else
    {
        for (auto& OverrideMtl : tUsingModelInfo.vecOverrideMaterial)
        {
            if (!OverrideMtl.isNull)
                m_iUseOverrideMaterials = true;
        }
    }

    if (!m_iUseOverrideMaterials) return S_OK;

    size_t iSizeMtl = Get_Component<CModel>()->Get_MaterialCount();
    size_t iSizeeOverrideMtl = tUsingModelInfo.vecOverrideMaterial.size();

    m_vecOverrideMaterials.resize(max(iSizeMtl, iSizeeOverrideMtl));


    CTextureBase::RESOURCE_BASE_DESC tResourceTextureOriginDecs{};

    CMaterial::MATERIAL_DESC tDesc{};

    /* 여기다가 경로를 집어넣어서 진행해주면된다 */
    vector<std::string> vecMateiralTexturePath{};
    vecMateiralTexturePath.resize(ENUM_TO_UINT(EMaterialTextureType::MAX_COUNT));

    _uint iIndex{};
    for (auto& OverrideMtl : tUsingModelInfo.vecOverrideMaterial)
    {
        if (OverrideMtl.isNull)
        {
            iIndex++;
            continue;
        }

        for (auto& pairTexturePath : OverrideMtl.vecUsingTextureInfo)
        {
            tResourceTextureOriginDecs.wstrPath = pairTexturePath.second;
            tResourceTextureOriginDecs.wstrName = path(pairTexturePath.second).filename().stem().wstring();
            CTextureBase* pBase = m_pGameInstance->GetOrAddTexture(tResourceTextureOriginDecs.wstrName, &tResourceTextureOriginDecs);
            Safe_Release(pBase);

            vecMateiralTexturePath[Get_IndexByMaterialSlotName(pairTexturePath.first)]
                = Engine_Utils::ToString(tResourceTextureOriginDecs.wstrName);
        }

        CMaterial* pMtl = m_pGameInstance->Get_Resource<CMaterial>(OverrideMtl.wstrMtl_JsonFile_Name);
        if (pMtl == nullptr)
        {
            tDesc.wstrName = OverrideMtl.wstrMtl_JsonFile_Name;
            tDesc.wstrPath = OverrideMtl.wstrMtl_JsonFile_Path;
            tDesc.spanTags = vecMateiralTexturePath;
            m_pGameInstance->Add_Resource<CMaterial>(tDesc.wstrName, CMaterial::Create(m_pDevice, m_pDeviceContext, &tDesc));
            pMtl = m_pGameInstance->Get_Resource<CMaterial>(OverrideMtl.wstrMtl_JsonFile_Name);
        }

        if (pMtl == nullptr)  return E_FAIL;
        /* Override Mtl 값 가져오기 */
        m_vecOverrideMaterials[iIndex++] = pMtl;

        /* 비우기 */
        std::fill(vecMateiralTexturePath.begin(), vecMateiralTexturePath.end(), "");
    }

    return S_OK;

}





HRESULT	CMapObject::Render()
{
	if (Super::Render())
		return E_FAIL;

	return S_OK;
}


void CMapObject::Free()
{

	for (auto& Com : m_arrayMapToolComponent)
	{
		Safe_Release(Com);
	}

    for (auto& OverrideMtl : m_vecOverrideMaterials)
    {
        Safe_Release(OverrideMtl);
    }


	Super::Free();

}
