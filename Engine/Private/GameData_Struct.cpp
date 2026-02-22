#include "Engine_pch.h"
#include "GameData_Struct.h"
#include "Engine_Utils.h"
#include "GameInstance.h"
#include "json.hpp"

namespace Engine
{

#pragma region Texture Splating Save Load Json

	void MIX_RGBA_INFO::Save_Json(json& SaveJson)
	{
		/* 배열로 저장해야함 */
		SaveJson = json::array();
		for (_int i = 0; i < this->iUse_Mix_RGBA_Count; ++i)
		{
			json Element{};

			Element["RGBA Texture"] = this->vecMixRGBATexture[i] == nullptr ? "None" : "Texture_" + Engine_Utils::ToString(this->vecMixRGBATexture[i]->Get_Name());
			this->vecMix_RGBA_Data[i].Save_Json(Element);
			SaveJson.push_back(Element);
		}
	}
	void MIX_RGBA_INFO::Load_Json(const json& LoadJson)
	{
		if (!LoadJson.is_array()) return;
		if (LoadJson.is_null()) return;

		this->vecMixRGBATexture.clear();
		this->vecMix_RGBA_Data.clear();

		_uint i = 0;
		for (const auto& Element : LoadJson)
		{
			string strRGBATextureName = Element.value("RGBA Texture", "None");
			this->vecMixRGBATexture.push_back(strRGBATextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(strRGBATextureName), nullptr));


			MIX_RGBA_DATA tData{};
			tData.Load_Json(Element);
			this->vecMix_RGBA_Data.push_back(tData);
			i++;
		}
		/* 개수 대입 */
		this->iUse_Mix_RGBA_Count = i;
	}

	void TEXTURE_SPLATTING_INFO::Save_Json(json& SaveJson)
	{
		SaveJson["Base Texture"] = this->pBase_Texture == nullptr ? "None" : "Texture_" +  Engine_Utils::ToString(pBase_Texture->Get_Name());
		SaveJson["Mix DH Tile Texture"] = this->pMix_DH_Tile_Texture == nullptr ? "None" : "Texture_" +  Engine_Utils::ToString(this->pMix_DH_Tile_Texture->Get_Name());
		SaveJson["Mix NBR Tile Texture"] = this->pMix_NBR_Tile_Texture == nullptr ? "None" : "Texture_" + Engine_Utils::ToString(this->pMix_NBR_Tile_Texture->Get_Name());

		json& Save_MixRGBAInfo_Json = SaveJson["Mix RGBA Info"];
		this->tMix_RGBA_Info.Save_Json(Save_MixRGBAInfo_Json);
	}
	void TEXTURE_SPLATTING_INFO::Load_Json(const json& LoadJson)
	{
		string TextureName = LoadJson.value("Base Texture", "None");
		this->pBase_Texture = TextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(TextureName), nullptr);

		TextureName = LoadJson.value("Mix DH Tile Texture", "None");
		this->pMix_DH_Tile_Texture = TextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(TextureName), nullptr);

		TextureName = LoadJson.value("Mix NBR Tile Texture", "None");
		this->pMix_NBR_Tile_Texture = TextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(TextureName), nullptr);


		if (LoadJson.contains("Mix RGBA Info"))
			this->tMix_RGBA_Info.Load_Json(LoadJson["Mix RGBA Info"]);

		return;
	}

	void MIX_RGBA_DATA::Save_Json(json& SaveJson)
	{

		SaveJson["R"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_R];
		SaveJson["R"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_R];
		SaveJson["R"]["Use Flags"] = this->iUseFlags[CHANNEL_R];

		SaveJson["G"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_G];
		SaveJson["G"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_G];
		SaveJson["G"]["Use Flags"] = this->iUseFlags[CHANNEL_G];

		SaveJson["B"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_B];
		SaveJson["B"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_B];
		SaveJson["B"]["Use Flags"] = this->iUseFlags[CHANNEL_B];

		SaveJson["A"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_A];
		SaveJson["A"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_A];
		SaveJson["A"]["Use Flags"] = this->iUseFlags[CHANNEL_A];

	}
	void MIX_RGBA_DATA::Load_Json(const json& LoadJson)
	{
		if (LoadJson.contains("R"))
		{
			auto& LoadJson_R = LoadJson["R"];

			this->fRGBA_Mix_Forces[CHANNEL_R] = LoadJson_R.value("RGBA Mix Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_R] = LoadJson_R.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_R] = LoadJson_R.value("Use Flags", 1);

		}


		if (LoadJson.contains("G"))
		{
			auto& LoadJson_G = LoadJson["G"];

			this->fRGBA_Mix_Forces[CHANNEL_G] = LoadJson_G.value("RGBA Mix Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_G] = LoadJson_G.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_G] = LoadJson_G.value("Use Flags", 1);
		}


		if (LoadJson.contains("B"))
		{
			auto& LoadJson_B = LoadJson["B"];

			this->fRGBA_Mix_Forces[CHANNEL_B] = LoadJson_B.value("RGBA Mix Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_B] = LoadJson_B.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_B] = LoadJson_B.value("Use Flags", 1);

		}

		if (LoadJson.contains("A"))
		{
			auto& LoadJson_A = LoadJson["A"];

			this->fRGBA_Mix_Forces[CHANNEL_A] = LoadJson_A.value("RGBA Mix Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_A] = LoadJson_A.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_A] = LoadJson_A.value("Use Flags", 1);

		}
	}

#pragma endregion

}