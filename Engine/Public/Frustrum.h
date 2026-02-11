#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CFrustrum : public CBase
{
	using Super = CBase;
private:
	struct FrustrumBounds
	{
		BoundingFrustum Origin;
		BoundingFrustum World;
	};
private:
	CFrustrum();
	virtual ~CFrustrum() = default;
public:
	HRESULT Initialize(const _float fMidStart = 300.f, const _float fFarStart = 700.f);
	EFrustrumTier Classify_BySplitFrustrum(const BoundingBox& AABB);
	EFrustrumTier Classify_BySplitFrustrum(const BoundingSphere& Sphere);
	void Update();
	void Update_WhenMainCameraProjUpdate(const Matrix &matProj);
	_float Get_MidStart() const { return m_fMidStart; }
	_float Get_FarStart() const { return m_fFarStart; }
	void Resize_SplitFrustrum(const _float fMidStart, const _float fFarStart);
	BoundingFrustum* Get_BoundingFrustrum_Local() { return &m_Bounds.Origin; }
	BoundingFrustum* Get_BoundingFrustrum_World() { return &m_Bounds.World; }
private:
	void Set_SplitFrustrum(const _float fMidStart, const _float fFarStart);
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	FrustrumBounds m_Bounds;
	FrustrumBounds m_nearBounding;
	FrustrumBounds m_midBounding;
	FrustrumBounds m_farBounding;
	_float m_fMidStart{ 0.f };
	_float m_fFarStart{ 0.f };
public:
	static CFrustrum* Create();
	virtual void Free() override;
};

NS_END