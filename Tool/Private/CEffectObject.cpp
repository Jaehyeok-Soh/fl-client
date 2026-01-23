#include "CEffectObject.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "mesh.h"
#include "VIBuffer_Particle_Point.h"

CEffectObject::CEffectObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :Tool_PartObject(eType, pDevice, pDeviceContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
    :Tool_PartObject(rhs)
{
}

HRESULT CEffectObject::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::Initialize(pArg)");
        return E_FAIL;
    }

    if (FAILED(Component_Setting()))
        return E_FAIL;

    if (FAILED(EffectDesc_Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::EffectDesc_Initialize(pArg)");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CEffectObject::Component_Setting()
{
    if (FAILED(Add_Component<CVIBuffer_Particle_Point>(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Point", nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectObject::EffectDesc_Initialize(void* pArg)
{
    Effect_Desc* pEffectDesc = static_cast<Effect_Desc*>(pArg);
    
    if (pEffectDesc == nullptr)
        return E_FAIL;

    m_tEffectDesc = *pEffectDesc;
    return S_OK;
}

HRESULT CEffectObject::Bind_ShaderResource()
{
    if (m_pShaderCom)
    {

    }

    return S_OK;
}

HRESULT CEffectObject::Awake(const _uint iCurrentLevelID)
{


    return S_OK;
}

void CEffectObject::Update_Priority(const _float fDT)
{
    Super::Update_Priority(fDT);
}

void CEffectObject::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    switch (m_tEffectDesc._Effect_ShapeType)
    {
    case E_SHAPETYPE::NONE:
        break;

    case E_SHAPETYPE::SPHERE:
    case E_SHAPETYPE::HEMISPHERE:
    case E_SHAPETYPE::CONE:
    case E_SHAPETYPE::CIRCLE:
        Get_Component<CVIBuffer_Particle_Point>()->Update_Simulation(fTimeDelta, E_PARTICLE_MOVESTATE::SPREAD);
        break;

    }
}

void CEffectObject::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CEffectObject::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

    switch (m_tEffectDesc.eEffectType)
    {
    case E_EFFECTTYPE::Particle:
        // 파티클 타입에 따라 어떤 형식으로 분사하고.
        // 어떤 종류를 파티클로 지정할 것인지. (Mesh, Particle, Texture) 3가지 타입.
        // Texture일 때, 빌보드를 먹일 것인가?
        Bind_ShaderResource_Particles();
        break;

    case E_EFFECTTYPE::Mesh:
        // 매쉬를 그리고 스크롤 때리는 것. 로직 작성
        Bind_ShaderResource_Meshes();
        break;

    case E_EFFECTTYPE::Trail:
        // 무조건 SIMULATIONSPACE_ WORLD 타입으로 
        Bind_ShaderResource_Trails();
        break;
    }

    // ===========  셰이더에 값 바인딩  ===========
    if (FAILED(Bind_ShaderResource()))
        return;
}

HRESULT CEffectObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;


    return S_OK;
}

_bool CEffectObject::Picking(OUT _float4& vOut)
{
    return _bool();
}

HRESULT CEffectObject::Export_Data(OUT MAPOBJECT_SAVEDATA& data)
{
    return S_OK;
}

void CEffectObject::Draw_ImGui()
{
}

void CEffectObject::Set_Dead(const wstring& wstrLayerTag)
{
}

void CEffectObject::Bind_ShaderResource_Particles()
{
    switch (m_tEffectDesc.eEffectParticleType)
    {
        case E_PARTICLETYPE::NONE:
            break;

        case E_PARTICLETYPE::PARTICLE:
           /* m_pShaderCom->Set_Pass(2);*/
            break;

        case E_PARTICLETYPE::TEXTURE:
            m_pShaderCom->Set_Pass(1);
            break;

        case E_PARTICLETYPE::MESH:
            m_pShaderCom->Set_Pass(0);
            break;
    }
    // 어떤 종류를 파티클로 지정할 것인지. (Mesh, Particle, Texture) 3가지 타입.
 
    // Texture일 때, 빌보드를 먹일 것인가?
}

void CEffectObject::Bind_ShaderResource_Meshes()
{

}

void CEffectObject::Bind_ShaderResource_Trails()
{

}

CEffectObject* CEffectObject::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CEffectObject* pInstance = new CEffectObject(eType, pDevice, pDeviceContext);

    if (pInstance == nullptr)
    {
        MSG_BOX("Create to Fail : EffectObject");
        Safe_Release(pInstance);
        return nullptr;
    }

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Initialize to Fail : EffectObject");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CEffectObject::Clone(void* pArg)
{
    CEffectObject* pClone = new CEffectObject(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CEffectObject::Clone, Failed");
        Safe_Release(pClone);
    }
    return pClone;
}

void CEffectObject::Free()
{
    Super::Free();
}
