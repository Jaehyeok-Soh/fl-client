#pragma once
#include "MonoBehaviour.h"
#include "Animation_Defines.h"
#include "DataStruct_SoundEvent.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Tool)

class CToolAnimSoundPlayer : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
private:
	CToolAnimSoundPlayer();
	CToolAnimSoundPlayer(const CToolAnimSoundPlayer& rhs);
	virtual ~CToolAnimSoundPlayer() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Update(const _float fTimeDelta) override;
	HRESULT Rebuild(const vector<DTO::SOUNDEVENT>& events);

	void Set_SoundLevelID(_uint iLevelID) { m_iSoundLevelID = iLevelID; }
	_uint Get_SoundLevelID() const { return m_iSoundLevelID; }
private:
	HRESULT Cache_OwnerModel();

	void Clear_SoundNotifies();
	void Register_Notifies(const vector<DTO::SOUNDEVENT>& events);
	AnimNotifyKey Build_SoundNotifyKey(const DTO::SOUNDEVENT& event);

	void Bind_Event();
	void Release_Event();
	void CallbackEvent(const Engine::AnimNotifyKey& key);
private:
	Engine::CModel* m_pOwnerModel = { nullptr };
	DelegateHandle m_EventHandle{};
	_uint m_iSoundLevelID = ENUM_TO_UINT(ELevelType::ANIMATION);
public:
	static CToolAnimSoundPlayer* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END