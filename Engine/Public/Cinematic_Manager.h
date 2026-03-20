#pragma once
#include "Base.h"
#include "CinematicCameraSequence.h"

NS_BEGIN(Engine)

class CGameInstance;

class CCinematic_Manager :  public CBase
{
	using Super = CBase;
private:
	//L"../../Resources/Data/CameraCinematicData/CameraCinematicData.json";

	/* 저장할 JsonPath 값을 넣어준다 */
	CCinematic_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCinematic_Manager() = default;

	HRESULT Initialize();
public:
	HRESULT							Play_CameraCinematic(const wstring& wstrFindKey);
public:
	/* Save Load */

	HRESULT							Load_CameraCinematicSequence(const _tchar* wszCameraCinematicDataJsonPath);
	HRESULT							Save_CameraCinematicSequence(const _tchar* wszCameraCinematicDataJsonPath);

	/* Key값을 통해 그 부분만 저장 */
	HRESULT							Load_CameraCinematicSequence(const wstring& wstrFindKey, OUT CinematicCameraSequence* pOutCamCinematicSequence);
	HRESULT							Save_CameraCinematicSequence(const wstring& wstrFindKey, const	CinematicCameraSequence* pSaveCamCinematicSequence);

public:
	/* Get */
	vector<string>					Get_CameraCinematicSequenceNames() const;
	const _int&						Get_CurrentKeyFrameIndex()	const { return m_iCurrentKeyFrameIndex;}
	const _int&						Get_PreviousFrameIndex()	const { return m_iPreviousKeyFrameIndex;}
public:
	/* Set */

private:
	map<wstring, CinematicCameraSequence>					m_mapCinematicCameraSequence;
	_int													m_iCurrentKeyFrameIndex;	/* 처음 시작 Current Index = 0 */
	_int													m_iPreviousKeyFrameIndex;	/* 이전 = -1 */
private:
	/* Cinematic */
	ID3D11Device*											m_pDevice;
	ID3D11DeviceContext*									m_pDeviceContext;
	CGameInstance*											m_pGameInstance;
public:
	static CCinematic_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
public:

};


NS_END