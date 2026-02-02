#pragma once
#include "MonoBehaviour.h"
#include "DataStruct_UI.h"

NS_BEGIN(Tool)
class CToolUI;
class CImage final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagImageDesc : public CMonoBehaviour::MONO_DESC
	{
		CToolUI* pOwner = { nullptr };
	}IMAGE_DESC;

private:
	CImage();
	CImage(const CImage& rhs);
	virtual ~CImage() = default;

private:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(const IMAGE_DESC& Desc);
	HRESULT Initialize(void* pArg) override;

public:
	void Update(const _float fTimeDelta) override;
	void Sync_Data();

public:
	void Set_TextureIndex(uint32_t index) { m_iTextureIndex = index; }
	uint32_t Get_TextureIndex() const { return m_iTextureIndex; };

private:
	CToolUI* m_pOwner = { nullptr };
	uint32_t m_iTextureIndex = {};
	DTO::TUI_ImageComponentData m_tData = {};

public:
	static CImage* Create(const IMAGE_DESC& Desc);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;

};

NS_END