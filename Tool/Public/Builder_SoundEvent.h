#pragma once
#include "BuilderBase.h"
#include "Anim_Event_Sound.h"
#include "ToolAnimSoundPlayer.h"

NS_BEGIN(Tool)

class CBuilder_SoundEvent final : public CBuilderBase
{
    using Super = CBuilderBase;

private:
    CBuilder_SoundEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
    virtual ~CBuilder_SoundEvent() = default;

private:
    HRESULT Initialize();

public:
    virtual HRESULT Build(const CDataDocumentBase& document) override;

private:
    HRESULT Create_SoundHandler(const DTO::SOUND_EVENT_INFO_DESC& data);

public:
    static CBuilder_SoundEvent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
    virtual void Free() override;
};

NS_END