#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(BONE_DESC* pDesc)
{
    ::strcpy_s(m_szName, pDesc->strName.c_str());
    m_iIndex = pDesc->iIndex;
    m_iParentIndex = pDesc->iParentIndex;

    // Converter에서 Export할때 이미 전치행렬 연산해서 넘겨줄거임
    ::memcpy(&m_matTransform, &pDesc->matTransform, sizeof(_float4x4));
    ::XMStoreFloat4x4(&m_matCombinedTransform, ::XMMatrixIdentity());
    return S_OK;
}

void CBone::Update_CombinedTransformMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    
    // 최상위 부모의 경우 PreTrnasformMatrix 연산
    // 이후에 자식들이 연산하면서 다 먹어 들어갈것
    if (m_iParentIndex == -1)
    {
        ::XMStoreFloat4x4(&m_matCombinedTransform,
            ::XMLoadFloat4x4(&m_matTransform) * PreTransformMatrix);
    }
    else
    {
        // 저장해두었던 ParentIndex로 CombinedTransformMatrix를 가져와 연산
        ::XMStoreFloat4x4(&m_matCombinedTransform,
            ::XMLoadFloat4x4(&m_matTransform) * Bones[m_iParentIndex]->Get_CombinedTransformMatrix());
    }
}

void CBone::Setup_BindPoseTransformMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    if (m_iParentIndex == -1)
    {
        ::XMStoreFloat4x4(&m_matBindPoseTransform,
            ::XMLoadFloat4x4(&m_matTransform) * PreTransformMatrix);
    }
    else
    {
        ::XMStoreFloat4x4(&m_matBindPoseTransform,
            ::XMLoadFloat4x4(&m_matTransform) * Bones[m_iParentIndex]->Get_BindPoseTransformMatrix());
    }
}

CBone* CBone::Create(BONE_DESC* pDesc)
{
    CBone* pInstance = new CBone();
    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("CBone::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CBone* CBone::Clone()
{
    return new CBone(*this);
}

void CBone::Free()
{
    Super::Free();
}
