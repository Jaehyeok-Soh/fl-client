#pragma once
#include "Tool_PartObject.h"


NS_BEGIN(Engine)
class CComputeShader;
class CModel;
NS_END

NS_BEGIN(Tool)

class CBonePart : public Tool_PartObject
{
	using Super = Tool_PartObject;

public:
	enum class BonePartFlag : Flags
	{
		None = 0,
		VSShakeOn = 0x0001,

	};

	typedef struct tagBonePartDesc : public Tool_PartObject::PARTOBJ_DESC
	{
		Vec4				tTintColor{1.f,1.f,1.f,1.f};
		EAnimShaderPass		m_eAnimShaderPass{EAnimShaderPass::Default};

		Matrix				ModelPreMatrix{Matrix::Identity};
		EModelType			eAddModelType{EModelType::NONANIM };
		_int				iAddModelRootBoneIndex{ -1 };
		ELevelType			iAddModelPrototypeLevel{ELevelType::STATIC};
		CComputeShader*		pParentBoneCombineCS = { nullptr };
		CModel*				pParentModel = { nullptr };
		wstring				wstrModelPrototypeName = { L"" };
		Flags				FFlags = ENUM_TO_UINT(BonePartFlag::None); // BonePartFlag ÀÌ¿כ
	}BONEPART_DESC;

private:
	CBonePart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBonePart(const CBonePart& rhs);
	virtual ~CBonePart() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void			Set_TintColor(const Vec4& vColor) { m_tMIDesc.vTintColor; }
	HRESULT			Change_Model(const wstring& wstrModelFolderName,_uint iAddModelPrototypeLevelType ,_uint iAddModelType , Matrix PreMatrix, _int iRootBoneIndex = -1);
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
	virtual HRESULT Render_Shadow() override;
public:
	void			Update_PartsModel();
private:
	CComputeShader* m_pBoneMeshCS = { nullptr };
	CComputeShader* m_pPartBoneCombineCS = { nullptr };
	CComputeShader* m_pParentBoneCombineCS = { nullptr };

	Flags			m_FFlags = {};


	SHADER_MI_DESC	m_tMIDesc{};
private:
	HRESULT Ready_Components(BONEPART_DESC* pDesc);
	HRESULT Ready_ComputeShaders(BONEPART_DESC* pDesc);

public:
	static	CBonePart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};

NS_END

