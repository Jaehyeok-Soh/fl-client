#pragma once
#include "TextureBase.h"

#pragma region Texture Splating Define
#define CHANNEL_R 0
#define CHANNEL_G 1
#define CHANNEL_B 2
#define CHANNEL_A 3
#define MAX_RGBA  4
#define MAX_RGBA_TEXTURE_COUNT 2 
#pragma endregion

namespace Engine
{
#pragma region Texture Splating Struct

	struct ENGINE_DLL MIX_RGBA_DATA
	{
		// Mix할떄 UV좌표에 곱해주어 정밀한 표현을 담당해준다
		float	fRGBA_Mix_Forces[MAX_RGBA]{ 1.f, 1.f, 1.f, 1.f };
		// Mix될 RGBA 맵에서 각 R , G , B , A 가 연결된 Splating Texture들의 Index
		int		iRGBA_Connected_Tile_Index[MAX_RGBA]{ 0 , 0 , 0 , 0 };
		// Mix될 RGBA 맵에서 각 R , G , B , A 가 Splating을 사용할건지 안할건지에 대한 Flag값 false => BaseTexture가 그대로 들어감 true => Splating
		int		iUseFlags[MAX_RGBA]{ true , true , true , true };

	public:
		void	Save_Json(json& SaveJson);
		void	Load_Json(const json& LoadJson);
	};

#pragma region CB에 넘길떄 사용될 CB Data
	/* Shader에 넘길때 사용할 constant Buffer 데이터들 */
	struct CB_MIX_RGBA_INFO
	{
		MIX_RGBA_DATA g_MIX_RGBA_DATA[MAX_RGBA_TEXTURE_COUNT];

		int    g_iUse_Mix_RGBA_Count = { 0 };
		int    g_Use_Mix_RGBA_Map_Count_Dummy[3];
	};
#pragma endregion

	/* Tool에서 사용할 데이터들 */
	struct ENGINE_DLL MIX_RGBA_INFO
	{
		std::vector<class CTextureBase*>							vecMixRGBATexture{}; //RGB
		vector<MIX_RGBA_DATA>							vecMix_RGBA_Data{};
		_int											iUse_Mix_RGBA_Count{};
	public:
		MIX_RGBA_INFO()
			: iUse_Mix_RGBA_Count{ 0 }, vecMix_RGBA_Data{}
		{
			vecMixRGBATexture.reserve(MAX_RGBA_TEXTURE_COUNT);
		}
		~MIX_RGBA_INFO()
		{
			Free();
		}
		MIX_RGBA_INFO(const MIX_RGBA_INFO& Copy)
			: vecMix_RGBA_Data{ Copy.vecMix_RGBA_Data }, iUse_Mix_RGBA_Count{ Copy.iUse_Mix_RGBA_Count }, vecMixRGBATexture{ Copy.vecMixRGBATexture }
		{
			for (auto& TextureBase : vecMixRGBATexture)
				Safe_AddRef(TextureBase);
		}
		MIX_RGBA_INFO& operator=(const MIX_RGBA_INFO& Copy)
		{
			if (this == &Copy)
				return *this;

			for (auto& Tex : this->vecMixRGBATexture)
				Safe_Release(Tex);

			this->vecMix_RGBA_Data = Copy.vecMix_RGBA_Data;
			this->iUse_Mix_RGBA_Count = Copy.iUse_Mix_RGBA_Count;
			this->vecMixRGBATexture = Copy.vecMixRGBATexture;
			this->iUse_Mix_RGBA_Count = Copy.iUse_Mix_RGBA_Count;


			for (auto& TextureBase : vecMixRGBATexture)
				Safe_AddRef(TextureBase);

			return *this;
		}
	public:
		void Add_Texture_And_Data()
		{
			if (iUse_Mix_RGBA_Count >= MAX_RGBA_TEXTURE_COUNT)
				return;

			iUse_Mix_RGBA_Count++;
			vecMixRGBATexture.push_back(nullptr);
			vecMix_RGBA_Data.push_back(MIX_RGBA_DATA());

		}
		void Delete_Texture_And_Data(_int iIndex)
		{
			if (iIndex >= iUse_Mix_RGBA_Count)return;
			if (iIndex < 0)					  return;

			iUse_Mix_RGBA_Count--;

			/* vector 메모리 정리 */
			Safe_Release(vecMixRGBATexture[iIndex]);
			vecMixRGBATexture.erase(vecMixRGBATexture.begin() + iIndex);
			vecMix_RGBA_Data.erase(vecMix_RGBA_Data.begin() + iIndex);
		}
		void Free()
		{
			for (auto& TexBase : vecMixRGBATexture)
				Safe_Release(TexBase);
			vecMixRGBATexture.clear();
			vecMix_RGBA_Data.clear();

			this->iUse_Mix_RGBA_Count = 0;
		}

		void	Save_Json(json& SaveJson);
		void	Load_Json(const json& LoadJson);
	};

	struct ENGINE_DLL TEXTURE_SPLATTING_INFO
	{
		/* Land Scape 에서 쓰는 것 뿐만아니라 지형의 전체적인 Color를 담당해준다 추후 바위 Normal위에 들어갈 색깔을 담당하기도한다 */
		class CTextureBase* pBase_Texture{ nullptr };

		/* RGBA Texture와 함꼐 Mix될 Tile Texture / Texture 2D Array로 해서 들어가게 된다 한장씩만 Binding */
		class CTextureBase* pMix_DH_Tile_Texture{ nullptr };
		class CTextureBase* pMix_NBR_Tile_Texture{ nullptr };

		/* Mix_DH_Tile_Texture 가 Texture 2D Array로 들어가 있기 때문에 이를 따로따로 모아두고 ImGui에서 Rendering 해주기위함 SRV 모아놓기 */
		std::vector<ID3D11ShaderResourceView*>								vecDHTextureArraySlices{};
		std::vector<ID3D11ShaderResourceView*>								vecNBRTextureArraySlices{};

		/* 현재 RGBA Texture 및 info 를 사용하는 개수  */
		MIX_RGBA_INFO													tMix_RGBA_Info{};

	public:

		void Free()
		{
			/* vec DH Slice */
			for (auto& SRV : vecDHTextureArraySlices)
				Safe_Release(SRV);
			vecDHTextureArraySlices.clear();

			/* vec NBR Slice */
			for (auto& SRV : vecNBRTextureArraySlices)
				Safe_Release(SRV);
			vecNBRTextureArraySlices.clear();

			/* Base Texture */
			Safe_Release(pBase_Texture);
			pBase_Texture = nullptr;

			/* DH Texture */
			Safe_Release(pMix_DH_Tile_Texture);
			pMix_DH_Tile_Texture = nullptr;

			/* NBR Texture */
			Safe_Release(pMix_NBR_Tile_Texture);
			pMix_NBR_Tile_Texture = nullptr;

			/* RGBA Info Free */
			tMix_RGBA_Info.Free();
		}
		TEXTURE_SPLATTING_INFO()
			: pBase_Texture{ nullptr }, pMix_DH_Tile_Texture{ nullptr }, pMix_NBR_Tile_Texture{ nullptr }, vecDHTextureArraySlices{}, tMix_RGBA_Info{}, vecNBRTextureArraySlices{}
		{

		}
		~TEXTURE_SPLATTING_INFO()
		{
			Free();
		}
		TEXTURE_SPLATTING_INFO(const TEXTURE_SPLATTING_INFO& Copy)
			: tMix_RGBA_Info(Copy.tMix_RGBA_Info), pBase_Texture(Copy.pBase_Texture), pMix_DH_Tile_Texture(Copy.pMix_DH_Tile_Texture), pMix_NBR_Tile_Texture(Copy.pMix_NBR_Tile_Texture)
			, vecDHTextureArraySlices(Copy.vecDHTextureArraySlices), vecNBRTextureArraySlices(Copy.vecNBRTextureArraySlices)
		{
			Safe_AddRef(this->pBase_Texture);
			Safe_AddRef(this->pMix_DH_Tile_Texture);
			Safe_AddRef(this->pMix_NBR_Tile_Texture);

			for (auto& SRV : vecDHTextureArraySlices)
				Safe_AddRef(SRV);
			for (auto& SRV : vecNBRTextureArraySlices)
				Safe_AddRef(SRV);
		}

		TEXTURE_SPLATTING_INFO& operator=(const TEXTURE_SPLATTING_INFO& Copy)
		{

			this->pBase_Texture = Copy.pBase_Texture;
			this->pMix_DH_Tile_Texture = Copy.pMix_DH_Tile_Texture;
			this->pMix_NBR_Tile_Texture = Copy.pMix_NBR_Tile_Texture;
			Safe_AddRef(this->pBase_Texture);
			Safe_AddRef(this->pMix_DH_Tile_Texture);
			Safe_AddRef(this->pMix_NBR_Tile_Texture);

			this->vecDHTextureArraySlices = Copy.vecDHTextureArraySlices;
			for (auto& SRV : this->vecDHTextureArraySlices)
				Safe_AddRef(SRV);

			this->vecNBRTextureArraySlices = Copy.vecNBRTextureArraySlices;
			for (auto& SRV : this->vecNBRTextureArraySlices)
				Safe_AddRef(SRV);

			this->tMix_RGBA_Info = Copy.tMix_RGBA_Info;

			return *this;
		}
		void	Save_Json(json& SaveJson);
		void	Load_Json(const json& LoadJson);
	};



#pragma endregion

}
