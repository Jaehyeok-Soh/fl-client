#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Client)
class CUIQTE_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIQTEImageDesc : public DIMAGE_DESC
	{
	}QTE_IMAGE_DESC;
private:
	CUIQTE_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIQTE_Image(const CUIQTE_Image& rhs);
	virtual ~CUIQTE_Image() = default;
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
	HRESULT Ready_Components(QTE_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Bind_Events()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
	virtual HRESULT Spawn_FromPool(void* pArg)override;
	virtual HRESULT Despawn_FromPool()override;
private:
	void KeyType_To_Texturetag(_uint iKeyIndex);

private:
	_bool m_isSpawned = { false };
	_uint m_iNumbering = {};
	
	//QTE_LINE
	Vec2 m_vCurrentNodePosOffset = {};
	Vec2 m_vPrevNodePosOffset = {};

	// QTE_KEYICON
	_uint m_iKeyType = {};

	// QTE_TIMING_CIRCLE
	_float m_fTimingDuration = {};
	_bool m_isOneTimeVisible = { false };

	// Glow Effect Values
	_float m_fTargetGlowInstensity = {};
	_float	m_fGlowIntensity = {};

	_uint m_iTimingType = {};

public:
	static CUIQTE_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END