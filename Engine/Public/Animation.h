#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

class CTexture;

typedef struct tagAnimationKeyframe
{
    _float2 vOffset = { 0.0f, 0.0f };
    _float2 vSize = { 0.0f, 0.0f };
    _float fTime = { 0.0f };
}ANIM_KEYFRAME;

class ENGINE_DLL CAnimation final : public CResourceBase
{
    using Super = CResourceBase;
public:
    typedef struct tagAnimationDesc : public Super::RESOURCE_BASE_DESC
    {
        _bool bLoop = { false };
        wstring wstrTextureTag = L"";
        vector<ANIM_KEYFRAME> vecKeyFrames;
    }ANIM_DESC;
private:
    CAnimation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CAnimation() = default;

    virtual HRESULT Initialize(void* pArg) override;
public:

    void Set_Loop(_bool bLoop) { m_bLoop = bLoop; }
    _bool IsLoop() const { return m_bLoop; }
    void Set_Texture(CTexture* pTexture) { m_pTexture = pTexture; }
    CTexture* Get_Texture() { return m_pTexture; }
    inline _float2 Get_TextureSize();
    const ANIM_KEYFRAME& Get_KeyFrame(_int iIndex) const
    {
        int a = 0;
        return m_vecKeyFrames[iIndex];
    }
    _int Get_KeyFrameCount() const { return (_int)m_vecKeyFrames.size(); }
private:
    HRESULT Create_ShaderResourceView(const wstring& wstrTag);
private:
    _bool m_bLoop = false;
    CTexture* m_pTexture = { nullptr };
    vector<ANIM_KEYFRAME> m_vecKeyFrames;
public:
    static CAnimation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
    virtual void Free() override;
};

NS_END