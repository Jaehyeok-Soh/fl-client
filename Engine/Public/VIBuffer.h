#pragma once
#include "Component.h"

//D3D11 Usage
//DEFAULT - GPU에서만 읽고 쓸수 있다.리소스를 GPU가 읽고 써야한다면 ?
//IMMUTABLE - GPU에서 읽기만 가능, 생성과 동시에 초기화가 이루어져야만 한다.왜냐하면 생성 이후에는 수정이 불가
//DYNAMIC - GPU에서는 읽기, CPU에서는 쓰기가 가능하다.동적인 리소스를 프레임마다 조정해야한다면 좋은 선택지, 꼭필요한게 아니라면 피하자
//STAGING - GPU에서 CPU에 데이터를 복사, CPU에서 읽어야 한다면 사용, Dynamic과 같은 이유로 GPU->CPU 메모리로 복사를 해야하는 느린 연산을 사용한다.

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::VIBUFFER;
	typedef struct tagVIBufferOriginDesc
	{
		D3D11_USAGE VB_Usage = { D3D11_USAGE::D3D11_USAGE_DEFAULT };
		D3D11_USAGE IB_Usage = { D3D11_USAGE::D3D11_USAGE_IMMUTABLE };
	}VIBUFFER_ORIGIN_DESC;
protected:
	CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

	virtual HRESULT Initialize_Prototype(void *pArg);
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Render() override;
	virtual HRESULT Bind_Resource();

	D3D11_USAGE Get_VB_Usage() const { return m_VB_Usage; }
	D3D11_USAGE Get_IB_Usage() const { return m_IB_Usage; }
	const Vec3* Get_VertexPositionData() const { return m_pVertexPositions; }
	const _uint* Get_IndicesData() const { return m_pIndices; }
	_uint Get_IndicesCount() const { return m_iIndexCount; }

	// 인스턴싱용으로 들고가자.
	ID3D11Buffer*& Get_VBBuffer() { return m_pVB; }
	ID3D11Buffer*& Get_IBBuffer() { return m_pIB; }

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	Vec3* m_pVertexPositions = { nullptr };
	_uint* m_pIndices = { nullptr };
protected:
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveType = { D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	//VB
	D3D11_USAGE m_VB_Usage = { D3D11_USAGE::D3D11_USAGE_DEFAULT };
	_uint m_VB_CPUAccesFlag = { 0 };
	_uint m_iVertexBufferCount = { 0 };
	_uint m_iVertexCount = { 0 };
	_uint m_iVertexStride = { 0 };
	
	//IB
	D3D11_USAGE m_IB_Usage = { D3D11_USAGE::D3D11_USAGE_IMMUTABLE };
	_uint m_IB_CPUAccesFlag = { 0 };
	DXGI_FORMAT m_eIndexFormat = { DXGI_FORMAT::DXGI_FORMAT_R16_UINT };
	_uint m_iIndexCount = { 0 };
	_uint m_iIndexStride = { 0 };
public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END