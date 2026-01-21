#include "Base.h"

CBase::CBase()
{
}

unsigned int CBase::AddRef()
{
    return ++m_iReferenceCount;
}

unsigned int CBase::Release()
{
    if (m_iReferenceCount == 0)
    {
        Free();
        delete this;
        return 0;
    }
    else
    {
        return m_iReferenceCount--;
    }
}

void CBase::Free()
{
}
