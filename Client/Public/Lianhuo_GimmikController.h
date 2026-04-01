#pragma once
#include "GimmikController.h"


NS_BEGIN(Engine)
class CSkillObjectSpawner_RandomXZ;
class CSingleSkillSpawner;
NS_END

NS_BEGIN(Client)

class CLianhuo_GimmikController final : public CGimmikController
{
	using Super = CGimmikController;
private:
	enum class EGimmikType
	{
		END
	};

	enum class EEffectType { XSpace, StunChanin, ChainThron };
private:
	CLianhuo_GimmikController();
	CLianhuo_GimmikController(const CLianhuo_GimmikController& rhs);
	virtual ~CLianhuo_GimmikController() = default;

	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
public:
	HRESULT Awake(const _uint iCurLevelIndex);
	void Update(const _float fTimeDelta);
	void Trigger_XSpace(const Vec3& vPosition);
	void Trigger_StunChain(const Vec3& vPosition);
	const Vec3& Get_BattleFieldCenter() const { return m_vSpawnPosition; }
	_float Get_BattleFiledMaxRange() const { return m_fFieldMaxRange; }
private:
	virtual HRESULT Bind_Events() override;
private:
	HRESULT Ready_Spawner();
	void Spawn_RandomSkill(const _float fTimeDelta);
	void Trigger_ChainThron();
private:
	void Play_EffectSound(EEffectType eType);

protected:
	///////////////
	//// Event ////
	///////////////
	virtual void On_ModelAnimNotify(const AnimNotifyKey& key) override;
private:
	// TODO - BattleField·Î »©±â
	Vec3 m_vSpawnPosition{ Vec3::Zero };
	_float m_fFieldMaxRange{ 15.f };

	TIME_LINE m_tChainThronTimer{};

	CSkillObjectSpawner_RandomXZ* m_pRandomChainThron = { nullptr };
	CSingleSkillSpawner* m_pXSpaceSpawner = { nullptr };
	CSingleSkillSpawner* m_pStunChainSpawner = { nullptr };
private:
	_uint m_iSoundHash_X0		= {};
	_uint m_iSoundHash_X1		= {};
	_uint m_iSoundHash_X2		= {};

	_uint m_iSoundHash_Chain0	= {};
	_uint m_iSoundHash_Chain1	= {};

public:
	static CLianhuo_GimmikController* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END