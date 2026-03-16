#pragma once
#include "PartObject.h"


//뼈가 있는 파트 오브젝트 이지만
//이 객체가 뼈를 업데이트를 하는게 아닌, 부모 뼈를 받아 스키닝 하는 오브젝트

NS_BEGIN(Engine)
class CComputeShader;
class CModel;
NS_END

NS_BEGIN(Client)
class CBonePart : public CPartObject
{
	using Super = CPartObject;

public:
	enum class BonePartFlag : Flags
	{
		None = 0,
		VSShakeOn = 0x0001,

	};

	typedef struct tagBonePartDesc : public CPartObject::PARTOBJ_DESC
	{
		CComputeShader*		pParentBoneCombineCS = { nullptr };
		CModel*				pParentModel = { nullptr };
		wstring				wstrModelPrototypeName = { L"" };

		Flags				FFlags = ENUM_TO_UINT(BonePartFlag::None); // BonePartFlag 이용
	}BONEPART_DESC;

private:
	CBonePart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBonePart(const CBonePart& rhs);
	virtual ~CBonePart() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual HRESULT Render() override;

private:
	CComputeShader* m_pBoneMeshCS = { nullptr };
	CComputeShader* m_pPartBoneCombineCS = { nullptr };
	CComputeShader* m_pParentBoneCombineCS = { nullptr };

	Flags				m_FFlags = {};

private:
	HRESULT Ready_Components(BONEPART_DESC* pDesc);
	HRESULT Ready_ComputeShaders(BONEPART_DESC* pDesc);

public:
	static	CBonePart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};

NS_END