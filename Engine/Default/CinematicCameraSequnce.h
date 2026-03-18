#pragma once
#include "Base.h"


NS_BEGIN(Engine)

class ENGINE_DLL CCinematicCameraSequnce final: public CBase
{
	using Super = CBase;
private:
	CCinematicCameraSequnce(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicCameraSequnce(const CCinematicCameraSequnce& rhs);
	virtual ~CCinematicCameraSequnce() {}
private:
	HRESULT					Initialize();
	HRESULT					Ready_Batch();

private:
	/* Data */


private:
	/* Debug Line을 위한 용도 */
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch;
	BasicEffect*									m_pEffect;
	ID3D11InputLayout*								m_pInputLayout;
private:
	ID3D11Device*									m_pDevice;
	ID3D11DeviceContext*							m_pContext;
public:
	void	Save_Json(json& SaveJson);
	void	Load_Json(const json& LoadJson);
public:
	static  CCinematicCameraSequnce* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static  CCinematicCameraSequnce* Clone(const CCinematicCameraSequnce& rhs);
	virtual void Free()override; 
};

NS_END

