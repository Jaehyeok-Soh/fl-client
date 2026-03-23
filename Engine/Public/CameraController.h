#pragma once
#include "Component.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

class ICameraModifier;

class ENGINE_DLL CCameraController final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::CAMERACONTROLLER;
private:
    CCameraController();
    CCameraController(const CCameraController& rhs);
    virtual ~CCameraController() = default;
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
public:
    void Update_Controller(_float fTimeDelta);
    void Play_Shake(const CAMERA_SHAKE_DESC& desc);
    void Play_FOV(const CAMERA_FOV_DESC& desc);
    void Play_PositionOffset(const CAMERA_POSITION_OFFSET_DESC& desc);
    void Play_RotationOffset(const CAMERA_ROTATION_OFFSET_DESC& desc);
    void Stop_All();
    void Stop_ByType(ECameraModifierType eType);
    void Build_FinalPose(const CAMERA_POSE& tBasePose, CAMERA_POSE& tOutPose) const;
private: 
    void Add_Modifier(ICameraModifier* pModifier);
    void Remove_FinishedModifiers();
    void Remove_ModifiersByType(ECameraModifierType eType);
    ICameraModifier* Create_ShakeModifier(const CAMERA_SHAKE_DESC& desc);
    ICameraModifier* Create_FOVModifier(const CAMERA_FOV_DESC& desc);
    ICameraModifier* Create_PositionOffsetModifier(const CAMERA_POSITION_OFFSET_DESC& desc);
    ICameraModifier* Create_RotationOffsetModifier(const CAMERA_ROTATION_OFFSET_DESC& desc);
    void Accumulate_Modifiers(const CAMERA_POSE& tBasePose, CAMERA_MODIFIER_RESULT& tOutResult) const;
private:
    CAMERA_CONTROLLER_DESC m_tConfig = {};
    vector<ICameraModifier*> m_vecModifiers;
public:
    static CCameraController* Create();
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END