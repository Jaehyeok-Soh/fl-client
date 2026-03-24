#pragma once
#include <string>
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ICameraAnchorProvider
{
public:
	virtual ~ICameraAnchorProvider() = default;

	virtual _bool Resolve_CameraAnchor(
		ECameraAnchorResolve eResolve,
		const std::wstring& wstrAnchorTag,
		const Matrix& matOwnerWorld,
		/*OUT*/ CAMERA_ANCHOR_RESULT& outResult) const PURE;
};

NS_END