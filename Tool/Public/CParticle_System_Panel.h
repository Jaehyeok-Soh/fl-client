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
    enum class E_EFFECT_RESOURCETYPE {TEXTURE, MESH, SHADER, FOLDER};
protected:
    explicit CParticle_System_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CParticle_System_Panel() = default;

protected:
    HRESULT EffectPanel_Initialize();
    HRESULT EffectFileResource_Setting();
    HRESULT ResourceFolderSearch(E_EFFECT_RESOURCETYPE eType, const string& Path);
    HRESULT ResourceFileSearch(E_EFFECT_RESOURCETYPE eType, const string& Path);


protected:
    //  =======  시간 계산  =========
    void Time_Calculator(const float fDT);

    void Binding_EffectDesc(CToolObject* pGo);

    //  =======  그리기 Window 창  =======
    void Draw_Timer(CToolObject* pGo);
    void Draw_ParticleSystem(CToolObject* pGo);

protected:
    void Draw_EffectColor(CToolObject* pGo);
    void Draw_Parts(CToolObject* pGo);
    void Draw_Drop_Texture(CToolObject* pGo);
    void Draw_Rotation_Texture(CToolObject* pGo);
    void Draw_Sprite_Texture(CToolObject* pGo);

    // Draw_Preview Texture Setting 함수
    void Draw_Preview_Texture(CToolObject* pGo);
    HRESULT Create_CanvasEffect();
    HRESULT Create_Preview_Resources();

    // Texture Window 함수들
    void UpdateRotationFlags();

protected:
    // ========= Renderer Button 창 ===========
    void Make_MeshSelectButton();

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
    std::map<string, std::vector<std::pair<string/*path*/, string/*Name*/>>> m_TextureMap;
    std::vector<string> m_TextureFolderNames;

    std::vector<std::pair<string/*Path*/, string/*Name*/>> m_TextureFileNames;
    std::vector<std::pair<string/*Path*/, string/*Name*/>> m_ShaderFileNames;

    string              m_sTextureFolderPath = "../../Resources/Textures/Effect";
    string              m_sShaderFolderPath = "../../Shaders";

private:
    // ======== ImGui 값이 변동 됐다는걸 알리는 변수 ========
    _bool               m_bModified = false;
    int                 m_iSelectPartsIndex = 0;
    string              m_strSelectedFolder = "";

    // ======== 이미지의 회전정보를 담고 있는 배열 ==========
    bool                m_bShowRotationModal = false;
    int                 m_iRotIndices[6] = { 0, 0, 0, 0, 0, 0 };

    // ======== 도화지로 쓸 이펙트 객체 ============ (Preview Texture를 만들기 위함)
    class CEffectObject*        m_pCanvasEffectObject = { nullptr };
    ID3D11RenderTargetView*     m_pPreviewRTV = {nullptr};
    ID3D11ShaderResourceView*   m_pPreviewSRV = { nullptr };
};

NS_END