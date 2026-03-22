#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_SoundEvent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDataDocument_SoundEvent final : public CDataDocumentBase
{
    using Super = CDataDocumentBase;

public:
    CDataDocument_SoundEvent();
    virtual ~CDataDocument_SoundEvent() = default;

    HRESULT Initialize();

public:
    virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::SOUNDEVENT; }

    virtual json ToJson() const override;
    virtual HRESULT FromJson(const json& j) override;

    HRESULT Upsert(const DTO::SOUND_EVENT_INFO_DESC& data);
    const DTO::SOUND_EVENT_INFO_DESC* Find_Data(const string& strOwnerTag) const;
private:
    virtual IObjectDataBase* Create_ObjectData(DTO::ECategory eType);
public:
    static CDataDocument_SoundEvent* Create();
    virtual void Free() override;
};

NS_END