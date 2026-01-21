#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Tool)

class CGodInput final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagGodInputDesc : public CMonoBehaviour::MONO_DESC
	{

	}GODINPUT_DESC;
private:
	CGodInput();
	virtual ~CGodInput() = default;

	virtual HRESULT Initialize_Prototype() override;
public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float fTimeDelta) override;
private:

public:
	static CGodInput* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END