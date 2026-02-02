#pragma once
#include "Base.h"
#include "GameInstance.h"

NS_BEGIN(Engine)

class CBuilderBase;
class CBuilderSystem;

class ENGINE_DLL CLevel abstract : public CBase
{
	using Super = CBase;
protected:
	explicit CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel() = default;

	virtual HRESULT Initialize();
public:
	// 객체 생성이 완료되고 Change_Level이 완료되었을 때 호출
	virtual HRESULT Awake(const _uint iCurrentLevelID);
	virtual void Update(_float fTimeDelta);
	virtual void Update_Picking();
	virtual HRESULT Render();
	const Vec4& Get_ClearColor() const { return m_vClearColor; }
public:
	HRESULT Ready_Builder(DTO::ECategory eCategory, CBuilderBase* pBuilder);
	HRESULT Build_File(_uint iLevelID, DTO::ECategory eCategory, string strFileKey);
protected:
	Vec4 m_vClearColor = { 0.f, 0.f, 1.f, 1.f };
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
private:
	CBuilderSystem* m_pBuilderSystem = { nullptr };
public:
	virtual void Free() override;
};

NS_END

