#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Tool)

class CImage final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagImageDesc : public CMonoBehaviour::MONO_DESC
	{

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
public:
	static CImage* Create(const IMAGE_DESC& Desc);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;

};

NS_END