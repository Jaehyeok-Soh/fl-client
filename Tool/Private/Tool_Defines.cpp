#include "pch.h"
#include "Tool_Defines.h"
#include "Tool_Structs.h"
#include <fstream>

unsigned int Tool::g_iWinSizeX = 1850;
unsigned int Tool::g_iWinSizeY = 950;
float Tool::g_fAspectio = 1.f;


HRESULT Tool::Load_CCS_EventManifest(OUT vector<struct CCS_EVENT_MANIFEST>* pOutData)
{
	if (pOutData == nullptr) return E_FAIL;
	std::ifstream ifs{ g_wszCameraCinematicSequnceEventManifest_JsonPath };
	if (!ifs.peek()) //엿보기 했는데 데이터가없다면 그냥 반환
		return S_OK;

	json LoadJson{};
	ifs >> LoadJson; //읽기
	*pOutData = LoadJson.get<std::vector<CCS_EVENT_MANIFEST>>();


	return S_OK;
}

HRESULT Tool::Save_CCS_EventManifest(const vector<struct CCS_EVENT_MANIFEST>* pSaveData)
{
	if (pSaveData == nullptr) return E_FAIL;
	std::ofstream ofs{ g_wszCameraCinematicSequnceEventManifest_JsonPath };

	json SaveJson{};
	SaveJson = *pSaveData;

	ofs << SaveJson.dump(4);
	ofs.close();

	return S_OK;
}
