#pragma once
#include "ImGui_Inspector.h"

NS_BEGIN(Client)

class CImGui_TextureDebugInspector : public CImGui_Inspector
{
	using Super = CImGui_Inspector;
private:
	enum ELayoutType : unsigned int
	{
		ShaderDebug = 0,
		COUNT
	};
private:
	CImGui_TextureDebugInspector();
	virtual ~CImGui_TextureDebugInspector() = default;

	HRESULT Initialize();
public:
	virtual void Render(CGameObject* pGo) override;
private:
	
public:
	static CImGui_TextureDebugInspector* Create();
	virtual void Free() override;
};

NS_END