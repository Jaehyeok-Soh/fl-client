#include "Tool_Defines.h"
#include "ImGui_Layout.h"

CImGui_Layout::CImGui_Layout(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pLabel, pDevice, pDeviceContext)
{
}

void CImGui_Layout::Free()
{
	Super::Free();
}