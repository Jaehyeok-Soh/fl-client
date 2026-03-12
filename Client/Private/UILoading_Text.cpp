#include "pch.h"
#include "UILoading_Text.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUILoading_Text::CUILoading_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUILoading_Text::CUILoading_Text(const CUILoading_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUILoading_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Initialize(void* pArg)
{
	LOADING_TEXT_DESC* pDesc = static_cast<LOADING_TEXT_DESC*>(pArg);
	m_pLoadingRatio = pDesc->pLoadingRatio;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	Set_Visible();

	return S_OK;
}

void CUILoading_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUILoading_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	// Ratio 값 갱신 
	if (m_eTextSubClassType == DTO::EUITextSubClassType::LOADING_TEXT_PERCENT && nullptr != m_pLoadingRatio)
	{
		_float fNewTarget = (*m_pLoadingRatio) * 100.f;

		if (fNewTarget < 0.f)	fNewTarget = 0.f;
		if (fNewTarget > 100.f) fNewTarget = 100.f;

		const _float fEps = 0.0001f;
		if (fabsf(fNewTarget - m_fTargetPercent) > fEps)
		{
			m_fStartPercent = m_fLerpPercent;
			m_fTargetPercent = fNewTarget;
			m_fPercentTimeAcc = 0.f;
		}
	}
}

void CUILoading_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	// Percent 계산
	if (m_eTextSubClassType == DTO::EUITextSubClassType::LOADING_TEXT_PERCENT)
	{
		Lerp_Percent(fTimeDelta);
	}
}

void CUILoading_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	if (MOUSE_RBUTTON_DOWN)
	{
		m_iCurTextCursor++;
		if (m_iCurTextCursor >= m_vecText.size())
			m_iCurTextCursor = 0;
	}

	Convert_Value_To_Text();
}

HRESULT CUILoading_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Ready_Components(LOADING_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Attach_Personal_Info()
{
	m_iCurTextCursor = 0;
	m_vecText.reserve(10);

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::LOADING_TEXT_TITLE:
		m_vecText.push_back(L"아스크의 징벌");
		m_vecText.push_back(L"악마의 쐐기");
		m_vecText.push_back(L"카론");
		m_vecText.push_back(L"필토이드");
		m_vecText.push_back(L"아이스레이크 시티");
		m_vecText.push_back(L"주조 마스터 대장장이");
		break;
	case DTO::EUITextSubClassType::LOADING_TEXT_CONTENTS:
		m_vecText.push_back(L"강적의 전투 태세가 완전히 무너지면 아스크의 징벌로 대량의 대미지를 입힐 수 있습니다.\n아스크는 카론의 신앙 속 아버지 신의 이름입니다.");
		m_vecText.push_back(L"악마의 쐐기를 캐릭터와 무기에 장착하면 캐릭터와 무기의 능력을 크게 향상시킬 수 있습니다. 일부 진귀한 악마의 쐐기는 캐릭터의 스킬 효과에 변화를 줄 수도 있습니다.");
		m_vecText.push_back(L"'마법'을 가진 이들은 카론이라 불리는데, 아트라시아 대륙의 각 나라마다 차이가 있다. 휘페르보레아 제국에서는 카론을 '악마의 아이'라고 부른다.");
		m_vecText.push_back(L"도시 바깥에 넓게 분포하는 몬스터로, 보통 무리 지어 출몰한다. 일반인에게 있어 결코 무시할 수 없을 정도로 위험하다.");
		m_vecText.push_back(L"휘페르보레아 제국 북쪽 지대에 있는 도시로 제국의 기원과 밀접한 관계를 가졌으며, 과거에는 월석의 주요 산지이기도 했다. 기나긴 혹한과 눈보라가 이곳을 지켜준다.");
		m_vecText.push_back(L"대장장이 루나에게서 무기와 아이템을 주조할 수 있습니다.");
		break;
	}
	return S_OK;
}

void CUILoading_Text::Lerp_Percent(const _float fTimeDelta)
{
	if (m_fDuration <= 0.f)
	{
		m_fPercentTimeAcc = 0.f;
		m_fLerpPercent = m_fTargetPercent;
		m_fStartPercent = m_fTargetPercent;
		m_wstrText = Float_To_Wstring(m_fLerpPercent, 0);
		return;
	}

	m_fPercentTimeAcc += fTimeDelta;

	_float t = m_fPercentTimeAcc / m_fDuration;
	if (t >= 1.f)
	{
		t = 1.f;
		m_fPercentTimeAcc = 0.f;
		m_fLerpPercent = m_fTargetPercent;
		m_fStartPercent = m_fTargetPercent;
		m_wstrText = Float_To_Wstring(m_fLerpPercent, 0);
		return;
	}

	m_fLerpPercent = m_fStartPercent + (m_fTargetPercent - m_fStartPercent) * t;
	m_wstrText = Float_To_Wstring(m_fLerpPercent, 0);
}

HRESULT CUILoading_Text::Convert_Value_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::LOADING_TEXT_TITLE:
	case DTO::EUITextSubClassType::LOADING_TEXT_CONTENTS:
		if (m_vecText.empty())
			break;
	m_wstrText = m_vecText[m_iCurTextCursor];
	break;
	}

	return S_OK;
}

void CUILoading_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	Ready_Fade_Text(1.f, 0.f, 1.f, m_fDelay);
}

void CUILoading_Text::Initialize_InVisible_Event()
{
}

_bool CUILoading_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade_Text(fTimeDelta);

	if (isFade)
	{
		m_isActive = true;
		m_isFin_Event = true;
		return true;
	}
	return false;
}

_bool CUILoading_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUILoading_Text* CUILoading_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUILoading_Text* pInstance = new CUILoading_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUILoading_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUILoading_Text::Clone(void* pArg)
{
	CUILoading_Text* pInstance = new CUILoading_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUILoading_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUILoading_Text::Free()
{
	Super::Free();
}
