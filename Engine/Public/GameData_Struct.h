#pragma once
#include "TextureBase.h"
#include "GameObject.h"

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
		// Mix 할때 높이값에 대해서 부드럽게 섞여울 Mix값 곱해지는 값
		float	fRGBA_Mix_Height_Forces[MAX_RGBA]{ 1.f, 1.f, 1.f, 1.f };
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




#pragma region Camera Cinematic Struct

#pragma region CinematicTarget Enum

	enum class ECinematicTarget
	{
		NONE,
		PLAYER,
		BOSS,
		END
	};

	static const char* g_szCinematicTarget[(int)Engine::ECinematicTarget::END] = {
		"NONE",
		"PLAYER",
		"BOSS"
	};

	inline string CinematicTarget_ToString(ECinematicTarget eType)
	{
		// 인덱스 초과 방지 안전장치
		if (eType >= Engine::ECinematicTarget::NONE && eType < Engine::ECinematicTarget::END)
			return g_szCinematicTarget[(int)eType];

		return "Unknown";
	}
	inline ECinematicTarget	CinematicTarget_ToEnum(const string& strType)
	{
		for (int i = 0; i < (int)Engine::ECinematicTarget::END; ++i)
		{
			if (strType == g_szCinematicTarget[i])
				return (Engine::ECinematicTarget)i;
		}
		return Engine::ECinematicTarget::NONE;
	}

	enum class ELerpType
	{
		NONE,           /* 보간 안 함 (Cut 연출. 즉시 텔레포트) */
		Linear,         /* 등속 이동 (기계처럼 처음부터 끝까지 똑같은 속도) */
		SlowStart,      /* 서서히 출발 (점점 빨라짐. 상용 엔진의 EaseIn) */
		SlowEnd,        /* 서서히 도착 (목적지에서 스르륵 멈춤. 상용 엔진의 EaseOut) */
		SmoothStep,     /* 서서히 출발 + 서서히 도착 (컷신의 꽃. 상용 엔진의 EaseInOut) */
		Curve,			/* 국선 주행 */
		END
	};

	// 헤더 파일의 Enum 선언 바로 밑이나, cpp 파일 상단에 선언해 둡니다.
	static const char* g_szLerpTypes[(int)Engine::ELerpType::END] = {
		"NONE",
		"Linear",
		"SlowStart",
		"SlowEnd",
		"SmoothStep",
		"Curve"
	};

	inline string LerpType_ToString(ELerpType eType)
	{
		// 인덱스 초과 방지 안전장치
		if (eType >= Engine::ELerpType::NONE && eType < Engine::ELerpType::END)
			return g_szLerpTypes[(int)eType];

		return "Unknown";
	}

	inline ELerpType LerpType_ToEnum(const string& strType)
	{
		for (int i = 0; i < (int)Engine::ELerpType::END; ++i)
		{
			if (strType == g_szLerpTypes[i])
				return (Engine::ELerpType)i;
		}
		return Engine::ELerpType::NONE;
	}


#pragma endregion

	/* 각 움직임별 카메라 움직임 정보 */
	struct ENGINE_DLL Camera_Keyframe_Data
	{
	public:
		/* Json 저장용 데이터 */

		/* 카메라가 움직일 기준이될 Target정보  */
		ECinematicTarget	eMoveBaseTarget{ ECinematicTarget::NONE };	/* 타겟 정보를 들고온다 */
		_int				iMoveBaseTargetBoneIndex{ NONE_BONE_INDEX };/* BoneIndex 정보 없으면 -1 있다면 0 이상 */
		Vec3				vPosition{ Vec3::Zero };					/* 포지션 */

		ELerpType			eMoveLerpType{ELerpType::Linear};			/* 이번 포지션값   이동에 Lerp를 쓸건지 말건지 */
		ELerpType			eLookAtLerpType{ELerpType::Linear };		/* 이번 LookAt    이동에  Lerp를 쓸지 말지 */
		ELerpType			eFovLerpType{ELerpType::NONE};				/* 이번 Fov값에   이동에  Lerp를 쓸지 말지 */

		/* Position , Look At , fov 가 전부 동일하게 사용하게된다 3개로 쪼갤려니 크게 어려워질거같아서 못쪼갬 ㅇㅇ */
		_float				fDuration{ 1.f };									/* 걸리는 시간 (초) */
		_float				fHoldTime{ 0.f };									/* 도착하고 다음 위치 이동까지 대기하는 시간 (초) */
		
		_float				fFov{ 60.f };										/* 카메라 줌인용 Fov 값 */

		/* 카메라가 바라볼 대상 */
		ECinematicTarget    eLookAtTarget{ECinematicTarget::NONE};				/* 바라볼 대상 */
		Vec3				vPitchYawRoll{0.f,0.f,0.f };						/* 바라볼 대상이 없다면 사용할 데이터 */
		/* 바라볼 대상이 있을때 사용할 데이터들 */
		_int                iLookAtBoneIndex{ NONE_BONE_INDEX };				/* 바라볼 대상의 특정 뼈 */
		Vec3                vLookAtOffset{ Vec3::Zero };						/* 타겟 위치에서 약간 위/아래를 볼 때 쓰는 오프셋 */


		vector<_uint>		vecOnReachEventIndex{};							/* 카메라가 이 위치에 도달했을 때 사용해줄 Global Event Index */
		vector<_uint>		vecHoldTimeEndEventIndex{};							/* 카메라가 이 위치에 도달하고 대기시간이 모두 끝났을 때 발송 해줄 Event Index */

	public:
		/* Cashing 용 데이터 */
		/* MoveBase */
		class CGameObject*	pCinematicMoveBaseTarget{nullptr};
		class CModel*		pCinematicMoveBaseModel{nullptr};

		/* LookAt Base */
		class CGameObject*	pCinematicLookAtTarget{ nullptr };
		class CModel*		pCinematicLookAtModel{ nullptr };

	private:
		_bool				isOnReachEventWork{false};
		_bool				isHoldTimeEndEventWork{false};
	public:
		Camera_Keyframe_Data();
		Camera_Keyframe_Data(class CCameraMan* pCameraman);
		Camera_Keyframe_Data(const Camera_Keyframe_Data& rhs);
		Camera_Keyframe_Data& operator=(const Camera_Keyframe_Data& rhs);
		~Camera_Keyframe_Data();
	public:
		Matrix				Get_WorldMatrix() const;

		/* BroadCast */
		void				BroadCast_OnReachEvent();
		void				BroadCast_HoldTimeEndEvent();
		void				Reset_GlobalEventWork();
	public:
		void				Reset();
		void				Copy_Camera(class CCameraMan* pCameraman);
	public:
		void				Save_Json(json& SaveJson);
		void				Load_Json(const json& LoadJson);
	private:
		void				UnBind_CashingData();
	};

	struct		ENGINE_DLL				Camera_Cinematic_Sequence
	{
	public:
		vector<_uint>									vecStartCinematic_GlobalEventIndex{0};
		vector<_uint>									vecEndCinematic_GlobalEventIndex{0};
		string											strDataName;			/* Data Name */
		vector<Camera_Keyframe_Data>					vecCamKeyFrameData{};	/* 카메라 움직임 정보 */
		_bool											isDebugRender{ true };
	private:

		ID3D11Device*									pDevice{nullptr};
		ID3D11DeviceContext*							pContext{nullptr};

		/* Debug Line을 위한 용도 */			
		PrimitiveBatch<DirectX::VertexPositionColor>*	pBatch{ nullptr };
		BasicEffect*									pEffect{ nullptr };
		ID3D11InputLayout*								pInputLayout{ nullptr };

	public:
		Camera_Cinematic_Sequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		Camera_Cinematic_Sequence(const Camera_Cinematic_Sequence& rhs);
		Camera_Cinematic_Sequence& operator=(const Camera_Cinematic_Sequence & rhs);
		virtual ~Camera_Cinematic_Sequence();
	public:
		/* 카메라 위치 Render용 Model , Shader */
		HRESULT	Render_Debug(_uint iPassIndex , class CModel* pCameraModel, class CShader* pShader);
	public:
		void	Delete(_int iDeleteIndex);
		void	Reset(_int iResetIndex = -1);
		void	Copy_Camera(class CCameraMan* pCamera , _int iCopyIndex = -1);
		void	Add_KeyFrameData(_int iCopyBeforeDataIndex = false);
		void	Insert_KeyFrameData( _uint iCurIndex , CCameraMan* pCamera = nullptr);
	public:
		void	BroadCast_OnReachEvent(_uint iindex);
		void	BroadCast_HoldTimeEndEvent(_uint iindex);
	public:
		void	Save_Json(json& SaveJson);
		void	Load_Json(const json& LoadJson);
	public:
		void	BroadCast(_bool isStart);
	};

#pragma endregion
}
