#pragma once
#include "ICameraModifier.h"

NS_BEGIN(Engine)

class CCameraModifierBase abstract : public ICameraModifier
{
    using Super = ICameraModifier;
protected:
    CCameraModifierBase() = default;
    CCameraModifierBase(const CCameraModifierBase&) = default;
    virtual ~CCameraModifierBase() = default;
public:
    virtual void Start() override;
    virtual void Update(_float fTimeDelta) override;
    virtual _bool IsFinished() const override { return m_bFinished; }
protected:
    _float Get_TotalDuration() const { return m_fBlendInTime + m_fHoldTime + m_fBlendOutTime; }
    // 0~1 가중치를 공통 계산
    _float Get_BlendWeight() const;
    _float Rand_Signed() const;
protected:
    _bool  m_bStarted = false;
    _bool  m_bFinished = false;
    _float m_fBlendInTime = 0.f;
    _float m_fHoldTime = 0.f;
    _float m_fBlendOutTime = 0.f;
    _float m_fElapsed = 0.f;
};

NS_END