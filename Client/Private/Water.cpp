#include "pch.h"
#include "Water.h"
#include "Shader.h"
#include "Model.h"
#include "InstanceMesh.h"
#include "Player.h"
#include "Transform.h"

#include "GameInstance.h"



CWater::CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(pDevice ,pDeviceContext)
	, m_tCBWaterData{}
	, m_pEffectCBBuffer{nullptr}
	, m_pEffectSRV{nullptr}
	, m_arrayWaterSRVs{}
	, m_vMI_TintColor{}
	, m_fDT{0.f}
	, m_pPlayer{nullptr}
	, m_iRippleIndex{0}
	, m_isRipple{false}
{ 
	m_arrayWaterSRVs.fill(nullptr);

}	

CWater::CWater(const CWater& rhs)
	: CMapObject(rhs)
	, m_tCBWaterData{ rhs .m_tCBWaterData }
	, m_pEffectCBBuffer{ rhs.m_pEffectCBBuffer }
	, m_pEffectSRV{ rhs.m_pEffectSRV }
	, m_arrayWaterSRVs{rhs.m_arrayWaterSRVs }
	, m_vMI_TintColor{rhs.m_vMI_TintColor }
	, m_iRippleIndex{rhs.m_iRippleIndex }
	, m_isRipple{rhs.m_isRipple }
{
}


HRESULT CWater::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CWater::WATER_DESC* pDesc = static_cast<CWater::WATER_DESC*>(pArg);
	m_tCBWaterData = pDesc->tCBWaterData;
	m_vMI_TintColor = pDesc->vMI_TintColor;
	
	/* 레퍼런스 카운터 증가 */
	m_arrayWaterSRVs = pDesc->arrayWaterSRVs;
	for (auto& SRV : m_arrayWaterSRVs)
		Safe_AddRef(SRV);


	if(FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	/* 고정 */
	m_tCBWaterData.tWaterRippleEffect.fRingThickness = 0.75f;
	m_tCBWaterData.tWaterRippleEffect.fMaxRadius = 0.5f;
	m_tCBWaterData.tWaterRippleEffect.fRippleFreq = 20.0f;
	m_tCBWaterData.tWaterRippleEffect.fRippleSpeed = 0.5f;
	m_tCBWaterData.tWaterRippleEffect.fRippleAmp = 3.0f;

	m_isRipple = m_tCBWaterData.tWaterRippleEffect.isUseRipple[0] = pDesc->isUseRipple;

	return S_OK;
}

HRESULT CWater::Ready_Component(WATER_DESC* pDesc)
{
	/* 생성된 Shader받아오기 */
	CShader* pShader = Get_Component<CShader>();
	if (!pShader) return E_FAIL;

	m_pEffectCBBuffer	=  pShader->Get_ConstantBuffer("CB_WaterData");		if (m_pEffectCBBuffer->IsValid() == false) return E_FAIL;
	m_pEffectSRV		=  pShader->Get_SRV("g_WaterTexture");				if (m_pEffectCBBuffer->IsValid() == false) return E_FAIL;


	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr) return E_FAIL;
	for (_uint i = 0; i < pModel->Get_MaterialCount(); ++i)
	{
		if (FAILED(pModel->Change_MI(i, EMaterialInstanceType::Free)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWater::Bind_Water(CShader* pShader)
{
	if (FAILED(m_pEffectSRV->SetResourceArray(m_arrayWaterSRVs.data(), 0, ENUM_TO_UINT(EWaterTextureType::END))))
		return E_FAIL;

	if (FAILED(m_pEffectCBBuffer->SetRawValue(&m_tCBWaterData, 0, sizeof(CB_WaterData))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;


	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer));
	if (m_pPlayer == nullptr) return E_FAIL;

	return S_OK;
}

void CWater::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CWater::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	m_tCBWaterData.g_fWaterDT += fTimeDelta;
	if (m_tCBWaterData.g_fWaterDT >= 1000.f)
		m_tCBWaterData.g_fWaterDT = 0.f;


	m_fDT += fTimeDelta;


	if (m_isRipple)
	{

		int iActiveCount = m_tCBWaterData.tWaterRippleEffect.iActiveRippleCount;
		for (int i = 0; i < iActiveCount; ++i)
		{
			m_tCBWaterData.tWaterRippleEffect.arrayRipplesPos[i].w += fTimeDelta;
		}
		if (m_fDT >= 0.25f)
		{
			if (m_pPlayer->Get_PlayerInfo()->fCurSpeed >= 0.3f)
			{
				m_fDT = 0.f;

				Vec3 vPlayerPos = m_pPlayer->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

				m_tCBWaterData.tWaterRippleEffect.arrayRipplesPos[m_iRippleIndex] = Vec4(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z, 0.f);

				// ★ [핵심] 16이 아니라 20을 기준으로 빙글빙글 돕니다! (0 ~ 19)
				m_iRippleIndex = (m_iRippleIndex + 1) % (_int)m_tCBWaterData.tWaterRippleEffect.arrayRipplesPos.size();

				// 활성화 개수 최대치도 20으로!
				if (m_tCBWaterData.tWaterRippleEffect.iActiveRippleCount < (_int)m_tCBWaterData.tWaterRippleEffect.arrayRipplesPos.size())
				{
					m_tCBWaterData.tWaterRippleEffect.iActiveRippleCount++;
				}
			}
		}
	}
}

void CWater::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CWater::Ready_Before_Render(const _float fTimeDelta)
{
	/* Water는 특별하게 해준다 */
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::COMPUTELIGHT_BLEND, this);
}

HRESULT CWater::Render()
{
	SHADER_PLAYER_INFO tInfo{};
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer));
	if (pPlayer == nullptr)
		return E_FAIL;



	if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
	{
		CShader* pShader = Get_Component<CShader>();				if (pShader == nullptr)         return E_FAIL;
		CModel* pModel = Get_Component<CModel>();					if (pModel == nullptr)          return E_FAIL;
		CTransform* pTransform = Get_Component<CTransform>();		if (pTransform == nullptr)      return E_FAIL;


		pPlayer->Bind_PlayerInfo(pShader);


		if (m_pEffectCBBuffer == nullptr)	return E_FAIL;
		if (m_pEffectSRV == nullptr)		return E_FAIL;

		/* WorldMatrix 바인딩 */
		pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Water));
		pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
		/* Object Info Update */
		if (FAILED(pShader->Bind_ObjectInfoData(m_tObjectInfoDesc)))	return E_FAIL;

		/* Transform Update */
		if (FAILED(pShader->Bind_TransformData(pTransform->Get_WorldMatrix())))
			return E_FAIL;

		if (FAILED(Bind_Water(pShader)))
			return E_FAIL;

		_uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

		for (_uint i = 0; i < iMeshCount; ++i)
		{
			pModel->Set_MI_TintColor(i ,m_vMI_TintColor);
			pModel->Bind_Material(pShader, i);
			pModel->Bind_MaterialInstance(pShader, i);
			pShader->Apply();
			pModel->Render(i);
		}

		return S_OK;

	}
	else if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
	{
		if (FAILED(CMapObject::Render_Instance(ENUM_TO_UINT(EMapObjectShaderPass::Water))))
			return E_FAIL;


		CShader* pShader = Get_Component<CShader>();						if (pShader == nullptr)         return E_FAIL;
		CModel* pModel = Get_Component<CModel>();							if (pModel == nullptr)          return E_FAIL;
		CTransform* pTransform = Get_Component<CTransform>();				if (pTransform == nullptr)      return E_FAIL;
		CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();		if (pInstanceMesh == nullptr)      return E_FAIL;

		if (m_pEffectCBBuffer == nullptr)	return E_FAIL;
		if (m_pEffectSRV == nullptr)		return E_FAIL;

		/* WorldMatrix 바인딩 */
		pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Water));
		pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
		/* Object Info Update */
		if (FAILED(pShader->Bind_ObjectInfoData(m_tObjectInfoDesc)))	return E_FAIL;

		/* Transform Update */
		if (FAILED(pShader->Bind_TransformData(pTransform->Get_WorldMatrix())))
			return E_FAIL;

		if (FAILED(Bind_Water(pShader)))
			return E_FAIL;

		_uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
		_uint iInstanceCount = pInstanceMesh->Get_InstanceCount();

		pInstanceMesh->Bind_Instance(1);
		for (_uint i = 0; i < iMeshCount; ++i)
		{
			pModel->Set_MI_TintColor(i, m_vMI_TintColor);
			pModel->Bind_Material(pShader, i);
			pModel->Bind_MaterialInstance(pShader, i);
			pShader->Apply();
			pModel->Render_Instance(i,iInstanceCount);
		}
		pInstanceMesh->Unbind_Resource(1);


		return S_OK;
	}
	else
		return E_FAIL;

	return S_OK;
}

CWater* CWater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWater* pWater = new CWater(pDevice, pDeviceContext);

	if (FAILED(pWater->Initialize_Prototype()))
	{
		Safe_Release(pWater);
		MSG_BOX("Water is failed to Create");
		return nullptr;
	}
	return pWater;
}

CGameObject* CWater::Clone(void* pArg)
{
	CWater* pWater = new CWater(*this);

	if (FAILED(pWater->Initialize(pArg)))
	{
		Safe_Release(pWater);
		MSG_BOX(" Water is failed to Clone ");
		return nullptr;
	}

	return pWater;
}

void CWater::Free()
{
	Super::Free();

	for (auto& SRV : m_arrayWaterSRVs)
		Safe_Release(SRV);
}
