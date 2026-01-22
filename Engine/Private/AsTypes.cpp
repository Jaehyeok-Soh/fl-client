#include "AsTypes.h"


NS_BEGIN(Engine)

void to_json(json& _j, const AS_BONE& _tData)
{
	_j = json
	{
		{"NAME", _tData.strName},
		{"INDEX", _tData.iIndex},
		{"PARENT", _tData.iParent},
		{"Transform_Right", {_tData.matTransform._11, _tData.matTransform._12, _tData.matTransform._13, _tData.matTransform._14}},
		{"Transform_Up", {_tData.matTransform._21, _tData.matTransform._22, _tData.matTransform._23, _tData.matTransform._24}},
		{"Transform_Look", {_tData.matTransform._31, _tData.matTransform._32, _tData.matTransform._33, _tData.matTransform._34}},
		{"Transform_Pos", {_tData.matTransform._41, _tData.matTransform._42, _tData.matTransform._43, _tData.matTransform._44}}
	};
}

void from_json(const json& _j, AS_BONE& _tData)
{
	using vVector4 = std::array<_float, 4>;

	_j.at("NAME").get_to(_tData.strName);
	_j.at("INDEX").get_to(_tData.iIndex);
	_j.at("PARENT").get_to(_tData.iParent);

	vVector4 right = _j.at("Transform_Right").get<vVector4>();
	vVector4 up = _j.at("Transform_Up").get<vVector4>();
	vVector4 look = _j.at("Transform_Look").get<vVector4>();
	vVector4 pos = _j.at("Transform_Pos").get<vVector4>();

	_tData.matTransform._11 = right[0];
	_tData.matTransform._12 = right[1];
	_tData.matTransform._13 = right[2];
	_tData.matTransform._14 = right[3];

	_tData.matTransform._21 = up[0];
	_tData.matTransform._22 = up[1];
	_tData.matTransform._23 = up[2];
	_tData.matTransform._24 = up[3];

	_tData.matTransform._31 = look[0];
	_tData.matTransform._32 = look[1];
	_tData.matTransform._33 = look[2];
	_tData.matTransform._34 = look[3];

	_tData.matTransform._41 = pos[0];
	_tData.matTransform._42 = pos[1];
	_tData.matTransform._43 = pos[2];
	_tData.matTransform._44 = pos[3];
}

void to_json(json& _j, const AS_MESH& _tData)
{
}

void from_json(const json& _j, AS_MESH& _tData)
{
}

void to_json(json& _j, const AS_MATERIAL& _tData)
{
	_j = json
	{
		{"NAME", _tData.strName},
		{"DIFFUSE_FILE", _tData.strDiffuseFile},
		{"NORMAL_FILE", _tData.strNormalFile},
		{"SPECULAR_FILE", _tData.strSpecularFile},
		{"AMBIENT", {_tData.vAmbient.x, _tData.vAmbient.y, _tData.vAmbient.z, _tData.vAmbient.w}},
		{"DIFFUSE", {_tData.vDiffuse.x, _tData.vDiffuse.y, _tData.vDiffuse.z, _tData.vDiffuse.w}},
		{"SPECULAR", {_tData.vSpecular.x, _tData.vSpecular.y, _tData.vSpecular.z, _tData.vSpecular.w}},
		{"EMISSIVE", {_tData.vEmissive.x, _tData.vEmissive.y, _tData.vEmissive.z, _tData.vEmissive.w}},
	};
}

void from_json(const json& _j, AS_MATERIAL& _tData)
{
	using vVector4 = std::array<_float, 4>;

	_j.at("NAME").get_to(_tData.strName);
	_j.at("DIFFUSE_FILE").get_to(_tData.strDiffuseFile);
	_j.at("NORMAL_FILE").get_to(_tData.strNormalFile);
	_j.at("SPECULAR_FILE").get_to(_tData.strSpecularFile);

	vVector4 Ambient = _j.at("AMBIENT").get<vVector4>();
	vVector4 Diffuse = _j.at("DIFFUSE").get<vVector4>();
	vVector4 Specular = _j.at("SPECULAR").get<vVector4>();
	vVector4 Emissive = _j.at("EMISSIVE").get<vVector4>();

	::memcpy(&_tData.vAmbient, Ambient.data(), sizeof(_float) * 4);
	::memcpy(&_tData.vDiffuse, Diffuse.data(), sizeof(_float) * 4);
	::memcpy(&_tData.vSpecular, Specular.data(), sizeof(_float) * 4);
	::memcpy(&_tData.vEmissive, Emissive.data(), sizeof(_float) * 4);
}

void to_json(json& _j, const VTXANIMMESH& _tData)
{
	_j = json
	{
		{"POSITION", {_tData.vPosition.x, _tData.vPosition.y, _tData.vPosition.z}},
		{"UV", {_tData.vUV.x, _tData.vUV.y}},
		{"NORMAL", {_tData.vNormal.x, _tData.vNormal.y, _tData.vNormal.z}},
		{"TANGENT", {_tData.vTangent.x, _tData.vTangent.y, _tData.vTangent.z}},
		{"BLEND_INDICIES", {_tData.vBlendIndices.x, _tData.vBlendIndices.y, _tData.vBlendIndices.z, _tData.vBlendIndices.w}},
		{"BLEND_WEIGHTS", {_tData.vBlendWeights.x, _tData.vBlendWeights.y, _tData.vBlendWeights.z, _tData.vBlendWeights.w}}
	};
}

void from_json(const json& _j, VTXANIMMESH& _tData)
{
	using vVector2 = std::array<_float, 2>;
	using vVector3 = std::array<_float, 3>;
	using vVector4 = std::array<_float, 4>;

	vVector3 vPosition = _j.at("POSITION").get<vVector3>();
	vVector2 vUV = _j.at("UV").get<vVector2>();
	vVector3 vNormal = _j.at("NORMAL").get<vVector3>();
	vVector3 vTangent = _j.at("TANGENT").get<vVector3>();
	vVector4 vBlendIndicies = _j.at("BLEND_INDICIES").get<vVector4>();
	vVector4 vBlendWeights = _j.at("BLEND_WEIGHTS").get<vVector4>();

	::memcpy(&_tData.vPosition, vPosition.data(), sizeof(_float) * 3);
	::memcpy(&_tData.vUV, vUV.data(), sizeof(_float) * 2);
	::memcpy(&_tData.vNormal, vNormal.data(), sizeof(_float) * 3);
	::memcpy(&_tData.vTangent, vTangent.data(), sizeof(_float) * 3);
	::memcpy(&_tData.vBlendIndices, vBlendIndicies.data(), sizeof(_float) * 4);
	::memcpy(&_tData.vBlendWeights, vBlendWeights.data(), sizeof(_float) * 4);
}
NS_END