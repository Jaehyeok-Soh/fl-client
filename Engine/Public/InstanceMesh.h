#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CRay;
class CModel;

class ENGINE_DLL CInstanceMesh final : public CVIBuffer
{
	using Super = CVIBuffer;
public:
	typedef struct tagInstanceMesh_Desc
	{
		D3D11_USAGE		VB_Usage = { D3D11_USAGE::D3D11_USAGE_DEFAULT };
		D3D11_USAGE		IB_Usage = { D3D11_USAGE::D3D11_USAGE_DEFAULT };


		/* Option 느낌으로 Min Max가 필요하다면 Model의 MinMax받아오기 */
		const Vec3*		pModelMinMax{nullptr};

		vector<Matrix>* vecInstanceMatrixPointer{ nullptr };
	}INSTANCEMESH_DESC;
private:
	CInstanceMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInstanceMesh(const CInstanceMesh& rhs);
	virtual ~CInstanceMesh() = default;
private:
	HRESULT			Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT			Ready_Instance_WorldMinMax(const Vec3* pModelMinMax , const vector<Matrix>* vecInstanceMatrixPointer);
public:
	HRESULT			Bind_Instance(_uint iSlotNum);
	virtual HRESULT Bind_Resource() override;
	virtual void	Render()		override;
	void			Unbind_Resource(_uint iSlotNum);
public:
	const Vec3*		Get_InstanceWorldMinMax() const { return m_pInstanceWorldMinMax; }
	HRESULT			ReMake_InstanceBuffer(vector<Matrix>* vecInstanceMatrixPointer);
	void			Update_Matrix(const Matrix& WorldMatrix, _uint iIndex);
	void			Update_Matrix(const vector<Matrix>& vMatrix);
public:
	_uint			Get_InstanceCount() const { return m_iInstanceCount; }
public:
	Vec3*							m_pInstanceWorldMinMax{ nullptr };
private:
	D3D11_BUFFER_DESC				m_tInstanceVertexBufferDesc{};
	_uint							m_iInstanceCount{};
	_uint							m_iVisibleInstanceCount{};
public:
	static  CInstanceMesh*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END
