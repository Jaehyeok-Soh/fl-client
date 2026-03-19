#include "pch.h"
#include "Tool_Structs.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
#include <wincrypt.h>
using json = nlohmann::json;
#pragma pop_macro("new")


namespace Tool
{

#pragma region SRT Data
    void from_json(const json& LoadJson, SRT_DATA& tData)
    {
        if (LoadJson.contains("Scale"))
            Engine_Utils::read_vec3_xyz(LoadJson["Scale"], tData.vScale);

        if (LoadJson.contains("Quaternion"))
            Engine_Utils::read_vec4_Quat(LoadJson["Quaternion"], tData.vQuat);

        if (LoadJson.contains("Position"))
            Engine_Utils::read_vec3_xyz(LoadJson["Position"], tData.vPosition);

        if (LoadJson.contains("Scale_Isolate"))
            Engine_Utils::read_vec3_xyz(LoadJson["Scale_Isolate"], tData.vScale_Isolated); // TEST: 소재혁 임시 추가
    }
    void to_json(json& SaveJson, const SRT_DATA& tData)
    {
        Engine_Utils::write_vec3_xyz(SaveJson["Scale"], tData.vScale);
        Engine_Utils::write_vec4_Quat(SaveJson["Quaternion"], tData.vQuat);
        Engine_Utils::write_vec3_xyz(SaveJson["Position"], tData.vPosition);

        Engine_Utils::write_vec3_xyz(SaveJson["Scale_Isolate"], tData.vScale_Isolated); // TEST: 소재혁 임시 추가
    }
#pragma endregion
#pragma region Using Model 
    void from_json(const json& LoadJson, USING_MODEL_INFO& tData)
    {
        tData.wstrName = Engine_Utils::ToWString(LoadJson.value("Name", ""));
        tData.wstrPath = Engine_Utils::ToWString(LoadJson.value("Path", ""));
        tData.wstrMtl_JsonFile_Path = Engine_Utils::ToWString(LoadJson.value("Meterial Json File Path", ""));


        if (LoadJson.contains("Override Materials"))
        {
            auto& MtlJsons = LoadJson["Override Materials"];
            tData.vecOverrideMaterial.resize(MtlJsons.size());

            _uint iIndex{};
            for (auto& MtlJson : MtlJsons)
            {
                if (MtlJson.empty())
                    tData.vecOverrideMaterial[iIndex].isNull = true;
                else
                    tData.vecOverrideMaterial[iIndex] = MtlJson;
                iIndex++;
            }
        }
    }
    void to_json(json& SaveJson, const USING_MODEL_INFO& tData)
    {
        SaveJson["Name"] = Engine_Utils::ToString(tData.wstrName);
        SaveJson["Path"] = Engine_Utils::ToString(tData.wstrPath);
        SaveJson["Meterial Json File Path"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Path);

        if (!tData.vecOverrideMaterial.empty())
        {
            auto& Material_Json = SaveJson["Override Materials"];

            for (auto& Material_Info : tData.vecOverrideMaterial)
            {
                json NullJson{};
                if (!Material_Info.isNull)
                    NullJson = Material_Info;
                Material_Json.push_back(NullJson);
            }
        }
    }

#pragma endregion
#pragma region Using Material
    void from_json(const json& SaveJson, OVERRIDE_MATERIALS& tData)
    {
        if (tData.isNull == true) return;

        tData.wstrMtl_JsonFile_Name = Engine_Utils::ToWString(SaveJson.value("Name", ""));
        tData.wstrMtl_JsonFile_Path = Engine_Utils::ToWString(SaveJson.value("Path", ""));


        if (SaveJson.contains("Textures"))
        {
            auto& TexJsons = SaveJson["Textures"];
            for (auto& TexJson : TexJsons)
            {
                if (TexJson.empty()) continue;
                std::pair<string, string> strPair = TexJson;
                tData.vecUsingTextureInfo.push_back({ Engine_Utils::ToWString(strPair.first) , Engine_Utils::ToWString(strPair.second) });
            }
        }
    }

    void Tool::to_json(json& SaveJson, const OVERRIDE_MATERIALS& tData)
    {

        SaveJson["Name"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Name);
        SaveJson["Path"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Path);

        for (auto& pairTextureInfo : tData.vecUsingTextureInfo)
        {
            SaveJson["Textures"].push_back({ Engine_Utils::ToString(pairTextureInfo.first), Engine_Utils::ToString(pairTextureInfo.second) });
        }
    }
#pragma endregion

#pragma region Static Model
    void from_json(const json& LoadJson, STATICMODEL_DATA& tData)
    {
        if (LoadJson.contains("SRT"))
            tData.tOriginSRT = LoadJson["SRT"];
        if (LoadJson.contains("Using Model Info"))
            tData.tUsingModelInfo = LoadJson["Using Model Info"];
    }

    void Tool::to_json(json& SaveJson, const STATICMODEL_DATA& tData)
    {

        Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Scale"], tData.tOriginSRT.vScale);
        Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Position"], tData.tOriginSRT.vPosition);
        Engine_Utils::write_vec4_Quat(SaveJson["SRT"]["Rotation"], tData.tOriginSRT.vQuat);

        SaveJson["Using Model Info"] = tData.tUsingModelInfo;
    }

#pragma endregion

#pragma region InstanceModel 
    void from_json(const json& LoadJson, INSTANCEMODEL_DATA& tData)
    {
        if (LoadJson.contains("SRTs"))
            tData.vecSRT = LoadJson["SRTs"];
        if (LoadJson.contains("Using Model Info"))
            tData.tUsingModelInfo = LoadJson["Using Model Info"];
    }


    void to_json(json& SaveJson, const INSTANCEMODEL_DATA& tData)
    {
        SaveJson["Using Model Info"] = tData.tUsingModelInfo;
        if (!tData.vecSRT.empty())
            SaveJson["SRTs"] = tData.vecSRT;
    }
#pragma endregion


#pragma region CCS_EventManifast
    
    void to_json(json& SaveJson, const CCS_EVENT_MANIFEST& tData)
    {
        SaveJson = json
        {
            {"Subscriber Name",tData.strSubscriberName},
            {"Action Infos",tData.vecActionNames}
        };
    }
    void from_json(const json& LoadJson, CCS_EVENT_MANIFEST& tData)
    {
        if (LoadJson.contains("Subscriber Name"))
        {
            LoadJson.at("Subscriber Name").get_to(tData.strSubscriberName);
        }
        if (LoadJson.contains("Action Infos"))
        {
            LoadJson.at("Action Infos").get_to(tData.vecActionNames);
        }
    }

    void to_json(json& SaveJson, const ActionInfo& tData)
    {
        SaveJson = json{
        {"Name", tData.strNames},
        {"Explain", tData.strExplain}
        };
    }

    void from_json(const json& LoadJson, ActionInfo& tData)
    {
        if (LoadJson.contains("Name")) LoadJson.at("Name").get_to(tData.strNames);
        if (LoadJson.contains("Explain")) LoadJson.at("Explain").get_to(tData.strExplain);
    }

#pragma endregion


}