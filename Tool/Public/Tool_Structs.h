#pragma once
#include "json_forward.h"

namespace Tool
{
#pragma region Map Data

    typedef struct tagSRTData
    {
        Vec3 vScale{1.f,1.f ,1.f};
        Quat vQuat{0.f,0.f,0.f,1.f};
        Vec3 vPosition{0.f,0.f,0.f};
        Vec3 vScale_Isolated{}; //TEST: 소재혁 임시 추가
    public:
        Matrix Get_World() const
        {
            return  Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(vPosition);
        }
        void Update_SRT(const Vec3& vScale , const Quat&  vQuat , const Vec3& vPosition)
        {
            this->vScale = vScale;     this->vQuat = vQuat; this->vPosition = vPosition;
        }
        void Update_SRT(const Matrix& WorldMatrix)
        {
            Matrix Mat = WorldMatrix;
            Mat.Decompose(this->vScale, this->vQuat, this->vPosition);
        }
        void Update_Scale(const Vec3& vScale)
        {
            this->vScale = vScale;
        }
        void Update_Position(const Vec3& vPosition)
        {
            this->vPosition = vPosition;
        }
        void Update_Quat(const Quat& vQuat)
        {
            this->vQuat = vQuat;
        }
    }SRT_DATA;

    typedef struct tagOverrideMaterials
    {
        bool   isNull{ false };
        /* 참조하고 있는 Origin Mrt Material Json 파일 Path 값 */
        wstring wstrMtl_JsonFile_Name{};
        wstring wstrMtl_JsonFile_Path{};
        /* 그 안에서 뜯어낸 Texutre 바인딩 이름 : Texutre 경로 [ 메테리얼 Json 경로에 꽃아줄 이름 ] */
        vector < std::pair<wstring, wstring>> vecUsingTextureInfo{};
    }OVERRIDE_MATERIALS;

    typedef struct tagUsingModelInfo
    {
        wstring wstrName{};
        wstring wstrPath{};

        /* 모델이 생성되고 난 이후에 저장되는 메테리얼 경로 */
        wstring wstrMtl_JsonFile_Path{};

        vector<OVERRIDE_MATERIALS> vecOverrideMaterial{};
    public:

    }USING_MODEL_INFO;

    typedef struct tagMapData_Base
    {
        EMapObject_Type eMapObjectType{ EMapObject_Type::END };
    public:
        virtual ~tagMapData_Base() {};
    }MAPDATA_BASE;

    /* Static Model Data */
    /* Loaded 된 애들이면 tSRT가 처음 Orgin Data로써 Reset 버튼 누를 시 작동한다 */
    typedef struct tagStaticModel_Data : public MAPDATA_BASE
    {
        USING_MODEL_INFO tUsingModelInfo{};
        /* Loaded Data */
        SRT_DATA       tOriginSRT{};
    public:
        virtual ~tagStaticModel_Data() {}
    }STATICMODEL_DATA;

    /* Instance StaticModel Data */
    /* Loaded 된 애들이면 tSRT가 처음 Orgin Data로써 Reset 버튼 누를 시 작동한다 */
    typedef struct tagInstanceModel_Data : public MAPDATA_BASE
    {
        USING_MODEL_INFO tUsingModelInfo{};

        vector<SRT_DATA> vecSRT{};
    public:
        virtual ~tagInstanceModel_Data() {}

    }INSTANCEMODEL_DATA;


    struct CB_EnvData
    {
        Vec4        vEnvColor{1.f,1.f,1.f,1.f};              // 16Byte 환경 Color
        /*  16Byte  */
        

        Vec3        vWindDirection{ 1.f,0.f, 1.f };             // 12Byte 바람이 부는 방향
        _float      fWindPower{ 1.f };                          // 4Byte 바람이 부는 새기      
        /*  16Byte */

        /*  16Byte */
        Vec4        vSkyColor{0.3f,0.7f,0.8f,1.f};
        Vec4        vCloudBaseColor{ 0.8f,0.8f,0.8f,1.f };
        Vec4        vCloudHighlight{ 1.f,1.f,1.f,1.f };
        Vec4        vCloudShadowColor{ 1.f, 1.f, 1.f, 1.f };   //16

        float       fCloudHighlightPower = 1.f;
        float       fCloudShadowPower = 1.f;
        Vec2        EnvDataDummy2;

        _int        isChannelPacking{ false };                  // 4Byte 채널 패킹을 사용하는지 않나는지
        _int        iSkyBoxTextureType{0};                      // 4Byte 텍스처가 원형용텍스처인지 , 사각형용 텍스처인지
        _float      fPolarRadiusScale{1.f};                     // 4Byte 여백 조절용 , 기본값 1.0
        _float      EnvDataDummy1{ 0.f };

        /*  16Byte */

        Vec2        vSkyBoxTextureUVSpeed{ 1.f,1.f };        // 8Byte UV
        float       fEnvAccDT{ 0.f };                        // 4Byte 시간값
        float       EnvDataDummy3{1.f};                      // 4Byte 더미

        /* 16 Byte */
    };


    struct CB_PlantData
    {
        _float      g_fDiffuseColorPower{ 1.f }; //바람이 부는 새기
        Vec3        g_vDummy{1.f,1.f,1.f};
    };

    struct CB_GrassData
    {
        _float g_fGrassDT{0.f};
        _float g_fGrassMaxHeight{1.f}; //이 모델의 잔디 MinMax중 Max의  Y값
        _float g_fGrassSwaySpeed{1.f}; //이 잔디가 Sway = 흔들리는 Speed
        _float g_fGrassWaveSize{1.f}; //이 잔디가 Power = 흔들리는 힘
    };


    struct CB_WaterData
    {
        _uint  g_WaterTexBindingFlags           {0};                            // Texture가 바인딩되었는지 안되어있는지 Flag값
        float  g_fWaterDT                       {0.f};                          // 움직이는 UV좌표를 위한 DT값
        Vec2   g_vWaterSpeed1                   {1.f,1.f};                      // 물 일렁임관련? Speed 값
        Vec2   g_vWaterSpeed2                   {1.f,1.f};                      // 물 일렁임관련? Speed 값
        Vec2   g_vWaterDistortionSpeed         {1.f, 1.f};                      // 

        Vec2    g_vWaterUVPower                 { 1.f, 1.f };
        Vec2    g_vWaterDistortionUVPower       {1.f, 1.f};                     // Noise Texture UV Tiling Power
        float   g_fDistortionPower              {1.f};                          // Noise가 섞이는 비율? 세기

        float   g_fSparklePower;                                                // 4 Byte (윤슬 눈뽕 강도!)
        Vec2    g_vSparkleUVPower;                                              // 8 Byte (윤슬 자글자글함 크기 조절!)
    };


    struct CB_FogData
    {
        _uint  g_FogTexBindingFlags{ 0 };                           // Texture가 바인딩되었는지 안되어있는지 Flag값
        float  g_fFogDT{ 0.f };                                     // 움직이는 UV좌표를 위한 DT값
        float  g_fDistortionPower{1.f};
        float  g_fDummy{1};
        Vec4   g_vUV[2];                                            //
    };

    struct ActionInfo
    {
        std::string strNames{};
        std::string strExplain{};
    };


    struct CCS_EVENT_MANIFEST
    {
        string              strSubscriberName;   // 예: "UI_Tutorial"
        vector<ActionInfo>  vecActionNames;      // 예: [ "Show_Popup", "Hide_Popup" ]
    public:
    };



    void to_json(json& SaveJson, const    SRT_DATA& tData);
    void to_json(json& SaveJson, const    OVERRIDE_MATERIALS& tData);
    void to_json(json& SaveJson, const    USING_MODEL_INFO& tData);
    void to_json(json& SaveJson, const    STATICMODEL_DATA& tData);


    void to_json(json& SaveJson, const    STATICMODEL_DATA& tData);
    void to_json(json& SaveJson, const    INSTANCEMODEL_DATA& tData);

    void from_json(const json& LoadJson, SRT_DATA& tData);
    void from_json(const json& LoadJson, OVERRIDE_MATERIALS& tData);
    void from_json(const json& LoadJson, USING_MODEL_INFO& tData);

    void from_json(const json& LoadJson, STATICMODEL_DATA& tData);
    void from_json(const json& LoadJson, INSTANCEMODEL_DATA& tData);


    void to_json(json& SaveJson, const    CCS_EVENT_MANIFEST& tData);
    void from_json(const json& LoadJson, CCS_EVENT_MANIFEST& tData);

    void to_json(json& SaveJson, const    ActionInfo& tData);
    void from_json(const json& LoadJson, ActionInfo& tData);

#pragma endregion 


}


