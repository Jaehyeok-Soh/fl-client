#pragma once
#include "Client_Defines.h"
#include "EffectBase.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CGameInstance;
NS_END

NS_BEGIN(Client)

class Effect : public CEffectBase
{
	using Super = CEffectBase;

protected:
	Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Effect(const Effect& rhs);
	virtual ~Effect() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
protected:
	virtual HRESULT Ready_PartsData(void* pArg);

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;
	virtual HRESULT Despawn_FromPool() override;

public:

	virtual HRESULT Enable_VFX(void* pArg) override;
	virtual HRESULT Disable_VFX() override;

protected:
	virtual void Update_Bone_Attached_Matrix();
	virtual void Update_CombinedWorldMatrix();
	virtual void Spawn_PositionCalculate(void* pArg);
	virtual void Update_FinishState();
public:
	static Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

protected:
	EFFECT_CONTAINERDESC			m_eDesc = {};
	Matrix							m_pOffsetMartix = {};
	const Matrix*					m_pBoneMatrix = { nullptr };
	const Matrix*					m_pBoneOwnerMatrix = { nullptr };
	_uint							m_iBoneFlag = {};
	_bool							m_bUseChildBone = { false };
	Matrix							m_matCombinedWorld = {};
};

NS_END