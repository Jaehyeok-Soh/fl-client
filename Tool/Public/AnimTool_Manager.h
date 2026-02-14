#pragma once
#include "Base.h"
#include "Animation_Defines.h"

NS_BEGIN(Engine)
class  CGameInstance;
NS_END

NS_BEGIN(Tool)

class CAnimTool_Manager final : public CBase
{
public:
	using Super = CBase;
	DECLARE_SINGLETON(CAnimTool_Manager)

private:
	CAnimTool_Manager();
	virtual ~CAnimTool_Manager() = default;

public:
	HRESULT Initialize_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	
	void Update(const _float fTimeDelta);

/// <summary>
/// 외부에 애니메이션 정보 건내주기
/// </summary>
public:
	ANIMCTRLINFO& Get_AnimControllInfo() { return m_tAnimControllInfo; }
	ANIM_EVENT_INFO& Get_AnimEventInfo() { return m_tEventInfo; }
	vector<ANIM_EVENT_BASE>& Get_AnimEvents(AnimEvent::Enum eType) { return m_tEventInfo.vecAnimEvents[eType]; }

/// <summary>
/// 애니메이션 모델과 애니메이션
/// </summary>
public:
	// 오브젝트와 정보 다시 가져오기
	void SetAnimationObject(CAnimObj* pObject);
	void SetAnimControllInfo();
	void SetAnimationInfo();
	void SetBoneInfo();

	// 오브젝트가 nullptr이 아닌지 검사
	_bool ValidCheck();

	// 애니메이션 트랙포지션 업데이트
	void Update_Animation(const _float fTimeDelta);

	// 애니메이션 바꾸기
	void ChangeAnimation(_uint iIndex);
private:
	// 매 프레임 가져오는 애니메이션 정보
	void UpdateAnimationInfo();

private:
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pDeviceContext{};

	CGameInstance* m_pGameInstance{ nullptr };

private:
	ANIMCTRLINFO m_tAnimControllInfo{};

	ANIM_EVENT_INFO m_tEventInfo{};

public:
	virtual void Free() override;
};

NS_END