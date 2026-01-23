#pragma once
#include "VertexData.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(Engine)

typedef struct tagAsBone
{
	string strName;

	_int iIndex = { -1 };
	_int iParent = { -1 };
	Matrix matTransform;

	tagAsBone* Clone()
	{
		return new tagAsBone(*this);
	}
}AS_BONE;

typedef struct tagAsMesh
{
	aiMesh* pMesh = { nullptr };
	string strName = { "" };
	_uint iMaterialIndex = { 0 };

	vector<VTXANIMMESH> vecVertices;
	vector<_uint> vecIndices;

	_uint iAffectBoneCount = { 0 };
	vector<_uint> vecAffectBoneIndices;

	vector<Matrix> vecOffsetMatrices;
}AS_MESH;

typedef struct tagAsMaterial
{
	string strName = { "" };
	string strDiffuseFile = { "" };
	string strNormalFile = { "" };
	string strSpecularFile = { "" };

	Vec4 vAmbient = { 0.f, 0.f, 0.f, 0.f };
	Vec4 vDiffuse = { 0.f, 0.f, 0.f, 0.f };
	Vec4 vSpecular = { 0.f, 0.f, 0.f, 0.f };
	Vec4 vEmissive = { 0.f, 0.f, 0.f, 0.f };
}AS_MATERIAL;

typedef struct tagAsBlendWeight
{
	XMUINT4 vIndices = { 0, 0, 0, 0 };
	Vec4 vWeights = { 0.f, 0.f, 0.f, 0.f };

	void Set(_uint iIndex, _uint iBoneIndex, _float fWeight)
	{
		_uint i = iBoneIndex;
		_float w = fWeight;

		switch (iIndex)
		{
			case 0: vIndices.x = i; vWeights.x = w; break;
			case 1: vIndices.y = i; vWeights.y = w; break;
			case 2: vIndices.z = i; vWeights.z = w; break;
			case 3: vIndices.w = i; vWeights.w = w; break;
		}
	}
}AS_BLENDWEIGHT;

typedef struct tagBoneWieght
{
	using Pair = std::pair<_int, _float>;
	vector<Pair> vecBoneWeights;
	vector<_int> vecBoneIndices;

	void Add_Weights(_uint iBoneIndex, _float fWeight)
	{
		if (fWeight <= 0.0f)
			return;

		auto itr = std::find_if(vecBoneWeights.begin(), vecBoneWeights.end(),
			[fWeight](const Pair& p)
			{
				return fWeight > p.second;
			});

		// (5, 0.5)
		// (1, 0.4)
		// (2, 0.2)
		// 가중치가 높은 애들을 앞에 배치
		vecBoneWeights.insert(itr, Pair(iBoneIndex, fWeight));
	}

	// (1, 0.3)
	// (2, 0.2)
	// 위 최종 가중치를 더하면 0.5
	// 이 가중치 합산을 1로 맞춰주기 위해 각각 2를 곱해주는 유틸함수
	void Normalize()
	{
		if (vecBoneWeights.size() >= 4)
			vecBoneWeights.resize(4);

		_float fTotalWeight = 0.f;
		for (const auto& pair : vecBoneWeights)
		{
			fTotalWeight += pair.second;
		}

		for (auto& pair : vecBoneWeights)
		{
			pair.second /= fTotalWeight;
		}
	}

	tagAsBlendWeight Get_BlendWeight()
	{
		tagAsBlendWeight tBlendWeights = {};

		for (size_t i = 0; i < vecBoneWeights.size(); ++i)
		{
			if (i >= 4)
				break;

			tBlendWeights.Set((_uint)i, vecBoneWeights[i].first, vecBoneWeights[i].second);
		}

		return tBlendWeights;
	}
}AS_BONEWEIGHTS;

// Keyframe 정보
typedef struct tagAsKeyFrame
{
	_float fTrackPosition = { 0.0f };
	Vec3 vScale = { 0.0f, 0.0f, 0.0f };
	Vec4 vQuaternion = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vec3 vTranslation = { 0.0f, 0.0f, 0.0f };
}AS_KEYFRAME;

// KeyFrame을 담고있는 구조체
typedef struct tagAsChannel
{
	string strBoneName = { "" };
	_int iBoneIndex = { 0 };
	_uint iKeyFrameCount = { 0 };
	vector<tagAsKeyFrame> vecKeyFrames;
}AS_CHANNEL;

typedef struct tagAsAnimation
{
	string strName = { "" };
	_float fTickPerSecond = { 0 };
	_float fDuration = { 0.0f };
	vector<tagAsChannel*> vecChannels;
}AS_ANIMATION;

void to_json(json& _j, const AS_BONE& _tData);
void from_json(const json& _j, AS_BONE& _tData);

void to_json(json& _j, const AS_MESH& _tData);
void from_json(const json& _j, AS_MESH& _tData);

void to_json(json& _j, const AS_MATERIAL& _tData);
void from_json(const json& _j, AS_MATERIAL& _tData);

void to_json(json& _j, const VTXANIMMESH& _tData);
void from_json(const json& _j, VTXANIMMESH& _tData);
NS_END