#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CRay;
class CModel;
struct MESH_RAY_HITINFO;

class CInstanceStaticMesh : public CVIBuffer
{
	using Super = CComponent;
public:
	typedef struct tagInstanceMesh_Desc
	{
		/* 참조할 Model 정보 */
		CModel* pModel{nullptr};
	};
private:
	CInstanceStaticMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInstanceStaticMesh(const CInstanceStaticMesh& rhs);
	virtual ~CInstanceStaticMesh() = default;
private:
	HRESULT			Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			Make_InsatnceDataByPrototypeModel(CModel* pModel);
public:
	_bool			IntsersectWithPlane(OUT Vec3& vOut);
	_bool			IntsersectWithPlane(CRay* const pRay, Matrix matWorld, _float fMaxDistance, OUT MESH_RAY_HITINFO& outHit);
private:
	CModel*							 m_pReferModel{nullptr};

	/* Mesh Count 동일한 매쉬하나만 그릴거니까 */
	_uint							 m_iTotalMeshNum{0};
	vector<class ID3D11Buffer*>		 m_vecVertex;
	vector<class CMaterial*>		 m_vecMaterials;

	vector<CVIBuffer*>				 m_pBuffer{};
	CVIBuffer*						 m_pInstanceBuffer{nullptr};
public:
	static  CInstanceStaticMesh*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END
