#pragma once
#include "ImGui_Panel.h"
#include "CEffectObject.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)



class CParticle_System_Panel :
    public CImGui_Panel
{
    using Super = CImGui_Panel;
public:
    enum class E_EFFECT_RESOURCETYPE {TEXTURE, MESH};
protected:
    explicit CParticle_System_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CParticle_System_Panel() = default;

protected:
    HRESULT EffectPanel_Initialize();
    HRESULT EffectFileResource_Setting();
    HRESULT ResourceFileSearch(E_EFFECT_RESOURCETYPE eType, const string& Path);


protected:
    //  =======  시간 계산  =========
    void Time_Calculator(const float fDT);


    //  =======  그리기 Window 창  =======
    void Draw_Timer();
    void Draw_ParticleSystem();
    void Draw_TextureFileList();
    void Draw_MeshFileList();

public:
    void Update(const _float fDT) override;
    virtual HRESULT Render(CToolObject* pGo) override;

public:
    virtual void Free() override;
    static CParticle_System_Panel* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

private:
    CGameInstance* m_pGameInstance = { nullptr };
    _bool          m_bParticleFlag = { false };

    CEffectObject::Effect_Desc  m_tCurrentDesc;

private:
    //  =======  Particle Window 전용 변수  ==========
    _float        m_fTimeAccumulation = 0.f;
    _float        m_fPlayBackSpeed = 1.f; // 시간 배속
    _bool         m_bTimeSetting = false;

private:
    //  =======  폴더명 & 파일명을 대신할 Mesh 전용 vector 컨테이너
    std::vector<string> m_MeshFileNames;
    std::vector<string> m_TextureFileNames;

    string              m_sMeshFolderPath = "../../Resources/Models";
    string              m_sTextureFolderPath = "../../Resources/Textures";
};

NS_END