#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CGun;

class CUIAimDot_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIAimDotImageDesc : public DIMAGE_DESC
	{
	}AIMDOT_IMAGE_DESC;
private:
	CUIAimDot_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIAimDot_Image(const CUIAimDot_Image& rhs);
	virtual ~CUIAimDot_Image() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Components(AIMDOT_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
private:
	CStatCom_Player* m_pPlayerStatCom	= { nullptr };
	CGun* m_pGunParts					= { nullptr };

	_bool m_isHitScan					= { false };	// 몬스터가 조준되고 있는지
	_bool m_isPreRangeAtt				= { true };		// 이전 프레임에 원거리 공격이였는지
	_bool m_isPreMeeleAtt				= { false };	// 이전 프레임이 근거리 공격이였는지
	_float m_fAttSpeed					= {0.1f};		// 공격 딜레이 -> Lerp Movement Duration으로 들어감
	_bool m_isShootingTrigger			= {};			// 총을 쐈는지
	Vec2 m_vMaxOffset					= {};			// 크로스 헤어 얼마나 튈지
	_bool m_isSpreadStart				= {};			// 지금 크로스 헤어가 올라가고 있는지
	_bool m_isSpreadEnd					= {};			// 지금 크로스 헤어가 내려오고 있는지 
	_bool m_isAtt						= {};			// 몬스터 공격에 성공했는지

public:
	static CUIAimDot_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END