#include "pch.h"
#include "Gravity_Force.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

CGravity_Force::CGravity_Force(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :Tool_PartObject(eType, pDevice, pDeviceContext)
{
}

CGravity_Force::CGravity_Force(const CGravity_Force& rhs)
    :Tool_PartObject(rhs)
{
}

HRESULT CGravity_Force::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CGravity_Force::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CGravity_Force -__super::Initialize(pArg)");
        return E_FAIL;
    }

    wstring objectName = L"Cube";
    CGameObject::Add_Component<CModel>(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_Model_" + objectName, nullptr);

    if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGravity_Force::Awake(const _uint iCurrentLevelID)
{
    // 절대로 절대로 Loader에서 불리면 안된다.
    return S_OK;
}

void CGravity_Force::Update_Priority(const _float fDT)
{
    // 임시 방편 Speed
    Super::Update_Priority(fDT);
}

void CGravity_Force::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

void CGravity_Force::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CGravity_Force::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
    m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONELIGHT, this);
#endif
    Super::Update_CombinedWorldMatrix(m_pMatParent);
}

HRESULT CGravity_Force::Render()
{
#ifdef _DEBUG
    if (FAILED(Super::Render()))
        return E_FAIL;


    CModel* pModel = CGameObject::Get_Component<CModel>();
    CShader* pShader = CGameObject::Get_Component<CShader>();

    if (!pModel || !pShader) return S_OK;

    pShader->Bind_TransformData(m_CombineWorldMatrix);

    UINT32 iMeshCount = pModel->Get_MeshCount();


    for (UINT32 i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader, i);
        pModel->Bind_MaterialInstance(pShader, i);
        pShader->Apply();
        pModel->Render(i);
    }
#endif
    return S_OK;
}

_bool CGravity_Force::Picking(OUT Vec3& vOut)
{
    return _bool();
}

_bool CGravity_Force::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
    return false;
}

void CGravity_Force::Draw_ImGui()
{
}

CGravity_Force* CGravity_Force::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CGravity_Force* pInstance = new CGravity_Force(eType, pDevice, pDeviceContext);

    if (pInstance == nullptr)
    {
        MSG_BOX("Create to Fail : CGravity_Force");
        Safe_Release(pInstance);
        return nullptr;
    }

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Initialize to Fail : CGravity_Force");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CGravity_Force::Clone(void* pArg)
{
    CGravity_Force* pClone = new CGravity_Force(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CGravity_Force::Clone, Failed");
        Safe_Release(pClone);
    }
    return pClone;
}

void CGravity_Force::Free()
{
    Super::Free();
}
