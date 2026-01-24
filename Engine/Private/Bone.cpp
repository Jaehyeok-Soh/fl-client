#include "Engine_pch.h"
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
    m_matTransform = pDesc->matTransform;
    m_matCombinedTransform = Matrix::Identity;
    return S_OK;
}

void CBone::Update_CombinedTransformMatrix(const vector<CBone*>& Bones, const Matrix& PreTransformMatrix)
{

    // 최상위 부모의 경우 PreTrnasformMatrix 연산
    // 이후에 자식들이 연산하면서 다 먹어 들어갈것
    if (m_iParentIndex == -1)
        m_matCombinedTransform = m_matTransform * PreTransformMatrix;
    else
        // 저장해두었던 ParentIndex로 CombinedTransformMatrix를 가져와 연산
        m_matCombinedTransform = m_matTransform * Bones[m_iParentIndex]->Get_CombinedTransformMatrix();
}

void CBone::Setup_BindPoseTransformMatrix(const vector<CBone*>& Bones, const Matrix& PreTransformMatrix)
{
    if (m_iParentIndex == -1)
        m_matBindPoseTransform = m_matTransform * PreTransformMatrix;
    else
        m_matBindPoseTransform = m_matTransform * Bones[m_iParentIndex]->Get_BindPoseTransformMatrix();
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
