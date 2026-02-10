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
		vector<Matrix>* vecInstanceMatrixPointer{ nullptr };
	}INSTANCEMESH_DESC;
private:
	CInstanceMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInstanceMesh(const CInstanceMesh& rhs);
	virtual ~CInstanceMesh() = default;
private:
	HRESULT			Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT			Bind_Instance(_uint iSlotNum);
	virtual HRESULT Bind_Resource() override;
	virtual void	Render()		override;
	void			Unbind_Resource(_uint iSlotNum);
public:
	HRESULT			ReMake_InstanceBuffer(vector<Matrix>* vecInstanceMatrixPointer);
	void			Update_Matrix(const Matrix& WorldMatrix, _uint iIndex);
	void			Update_Matrix(const vector<Matrix>& vMatrix , _uint iIndex);
public:
	_uint			Get_InstanceCount() const { return m_iInstanceCount; }
public:
private:
	D3D11_BUFFER_DESC				m_tInstanceVertexBufferDesc{};
	_uint							m_iInstanceCount{};
public:
	static  CInstanceMesh*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END
