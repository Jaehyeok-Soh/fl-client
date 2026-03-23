#pragma once
#include "CameraMan.h"
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;

class CCinematicCamera;
struct CinematicCameraSequence;

template<typename T>
class CConstant_Buffer;


class CCamera_Manager final : public CBase
{
private:
	CCamera_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCamera_Manager() = default;

	HRESULT Initiailize();
public:
	CCameraMan* Get_MainCamera() { return m_pMainCamera; }

	/* Cinematic 카메라 등록함수 , Static Level에 등록 추천 아닐시 매 Level 등록 해줘야함 */
	HRESULT	Register_CinematicCamera(_uint iPrototypeLevelIndex, const wstring& wstrFindPrototypeTag, _uint iCloneLevelIndex, const wstring& wstrAddLagerTag, void* pCinematicCameraDesc);

	void	Change_MainCamera(CameraType eType, const wstring& wstrTag);
	void	Add_Camera(CameraType eType, const wstring& wstrTag, CCameraMan* pGo);
	void	Remove_Camera(CameraType eType, const wstring& wstrTag);

	void	Add_Actor_Object(CGameObject* pGo, _bool bImmediatelyChange = false);
	void	Remove_Actor_Object(CGameObject* pGo);

	void	Change_Target(CGameObject* pGo);
	HRESULT Change_Target_Next();

	HRESULT Play_MainCameraShake(const CAMERA_SHAKE_DESC& desc);
	HRESULT Play_MainCameraFOV(const CAMERA_FOV_DESC& desc);
	HRESULT Play_MainCameraPositionOffset(const CAMERA_POSITION_OFFSET_DESC& desc);
	HRESULT Play_MainCameraRotationOffset(const CAMERA_ROTATION_OFFSET_DESC& desc);
	HRESULT Play_CameraCinematic(CinematicCameraSequence* pCameraCinematicSequence );
	HRESULT	End_CameraCinematic();

	const Matrix& Get_ViewMatrix() const { return m_matView; }
	void Set_ViewMatrix(const Matrix &matView) { m_matView = matView; }
	const Matrix& Get_ProjMatrix() const { return m_matProjection; }
	void Set_ProjMatrix(const Matrix &matProj) { m_matProjection = matProj; }

	const Matrix& Get_UI_ViewMatrix() const { return m_matView_UI; }
	const Matrix& Get_UI_ProjMatrix() const { return m_matProjection_UI; }

	void Update_AccTime_ForShader(const _float fTimeDelta);
	void Update_ViewMatrix();

	void Setup_ViewProj_ToCBuffer();
	void Setup_UIViewProj_ToCBuffer();
	void Setup_Inv_ToCBuffer();

	ID3D11Buffer* Get_Inv_ConstantBuffer();
	ID3D11Buffer* Get_Global_ConstantBuffer();

	void Clear();
private:
	void Create_ConstantBuffer();
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CCameraMan* m_pMainCamera = { nullptr };
	Matrix m_matView;
	Matrix m_matProjection;
	Matrix m_matView_UI;
	Matrix m_matProjection_UI;
	unordered_set<CGameObject*> m_Actors;
	map<wstring, CCameraMan*> m_Cameras[ENUM_TO_UINT(CameraType::END)];

	SHADER_GLOBALDESC m_tGlobalDesc = {};
	SHADER_INVDESC m_tInvDesc = {};
	CConstant_Buffer<SHADER_GLOBALDESC>* m_pGlobal_CBuffer = { nullptr };
	CConstant_Buffer<SHADER_INVDESC>* m_pInv_CBuffer = { nullptr };


	/* 카메라 교체 시, 교체되기 '이전' 카메라의 정보 저장 */
	CameraType	m_ePrevCameraType{};
	wstring		m_wstrPrevCameraName{L""};
	Matrix		m_matPrevCameraWorld{ Matrix::Identity };
private:
	CGameInstance* m_pGameInstnace{ nullptr };
public:
	static CCamera_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END