#pragma once
#include "PartObject.h"

/*
 
weapon은 상속 받아서 만들어야 하므로
m_pMatParent 말고 소켓 매트릭스로 더 달 오브젝트들

final 아니라서 필요하다면 부모객체로 만들기

*/

NS_BEGIN(Engine)
class CComputeShader;
NS_END

NS_BEGIN(Client)

class CSocketObject : public CPartObject
{
	using Super = CPartObject;

public:
	enum class SOCEKT_FLAGS
	{
		None = 0,

		Has_SocketMatrix		= 0x000001,

		Model_Static			= 0x000002,
		Model_Anim				= 0x000004,

		RootMotion_RemoveAll	= 0x000008,
	};

	typedef struct tagSocketDesc : public CPartObject::PARTOBJ_DESC
	{
		const Matrix*		pMatSocket				= { nullptr };
		wstring				wstrModelPrototypeName	= { L"" };
		Flags				FSocketFlgas			= {ENUM_TO_UINT(SOCEKT_FLAGS::None)};

		// animation model일때 필요한 정보들

		_int iStartAnimIdx;

	}SOCEKT_DESC;

private:
	CSocketObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSocketObject(const CSocketObject& rhs);
	virtual ~CSocketObject() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual void	Ready_Before_Render(_float fTimeDelta) override;
	virtual void	OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void	OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void	OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual HRESULT Render() override;

private:
	Flags			m_FSocketFlags = {};
	const Matrix*	m_pMatSocket = { nullptr };
	_bool			m_bAnimModel = { true };

private:
	CComputeShader* m_pBoneMeshCS			= { nullptr };
	CComputeShader* m_pBoneCombineCS		= { nullptr };
	CComputeShader* m_pBoneAnimEvaluateCS	= { nullptr };
	CComputeShader* m_pBoneAnimBlendCS		= { nullptr };

private:
	HRESULT Ready_Components(SOCEKT_DESC* pDesc);
	HRESULT Ready_AnimModel(SOCEKT_DESC* pDesc);

private:
	void	Update_Anim(const _float fTimeDelta);

	HRESULT Render_Static();
	HRESULT Render_Anim();

public:
	static	CSocketObject*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};
NS_END