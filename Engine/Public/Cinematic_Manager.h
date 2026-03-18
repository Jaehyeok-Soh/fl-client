#pragma once
#include "Base.h"

NS_BEGIN(Engine)


class CGameInstance;
class CCamera_Cinematic_Sequence;

class CCinematic_Manager :  public CBase
{
	using Super = CBase;
private:
	//L"../../Resources/Data/CameraCinematicData/CameraCinematicData.json";

	/* 저장할 JsonPath 값을 넣어준다 */
	CCinematic_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,const _tchar* wszCameraCinematicDataJsonPath);
	virtual ~CCinematic_Manager() = default;

	HRESULT Initialize();
public:
	HRESULT							Play_CameraCinematic(const wstring& wstrFindKey);
public:
	HRESULT							Load_CameraCinematicSequence();  	/* Load */
	HRESULT							Save_CameraCinematicSequence();		/* 현재 저장된 데이터 전부 저장함수 */

	/* 이미 전부 Load된 Data에서 받아가는 함수 */
	HRESULT							Load_CameraCinematicSequence(const wstring& wstrFindKey, OUT CCamera_Cinematic_Sequence* pOutCamCinematicSequence);
	HRESULT							Save_CameraCinematicSequence(const wstring& wstrFindKey, const	CCamera_Cinematic_Sequence* pSaveCamCinematicSequence);


/* Tool 작업을 위한함수 */
	vector<string>					Get_CameraCinematicSequenceNames() const;
public:
	HRESULT							Register_GlobalEventsBroadCast(_uint iTypeIndex, std::function<void()> funcGlobalEvent);
	HRESULT							BroadCaset_RegisterGlobalEvent(_uint iTypeIndex);
private:
	map<wstring, CCamera_Cinematic_Sequence*>				m_mapCameraCinematicSequence;
	const _tchar*											m_wszCameraCinematicDataPath;
private:
	/* Cinematic */
	ID3D11Device*											m_pDevice;
	ID3D11DeviceContext*									m_pDeviceContext;
	CGameInstance*											m_pGameInstance;
private:
	/* String값 매핑 */
public:
	static CCinematic_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};


NS_END