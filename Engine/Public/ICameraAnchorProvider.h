#pragma once
#include <string>
#include "Engine_Define.h"

//////////////////////////////////
// Body 계열에서 구현할 인터페이스 //
// bone을 실제로 해석 하는 쪽!    //
//////////////////////////////////

NS_BEGIN(Engine)

class ICameraAnchorProvider
{
public:
	virtual ~ICameraAnchorProvider() = default;

	virtual _bool Resolve_CameraAnchor(
		ECameraAnchorResolve eResolve,
		const std::string& strAnchorTag,
		const Matrix& matOwnerWorld,
		OUT CAMERA_ANCHOR_RESULT& outResult) PURE;
};

NS_END