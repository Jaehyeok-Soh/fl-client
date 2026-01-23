#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Line_Color final : public CVIBuffer
{
    using Super = CVIBuffer;
public:
    // 생성시 최대 라인 수, 색상만 넘기면 됨
    typedef struct tagViBufferLineOriginDesc : public Super::VIBUFFER_ORIGIN_DESC
    {
        _uint   iMaxLineCount = 2048;                // 최대 라인 수 (N segments)
        Vec4 vColor = {0.f, 1.f, 0.f, 1.f}; // 디폴트 노란색
    }VIBUFFER_LINE_ORIGIN_DESC;
private:
    CVIBuffer_Line_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CVIBuffer_Line_Color(const CVIBuffer_Line_Color& rhs);
    virtual ~CVIBuffer_Line_Color() = default;

    virtual HRESULT Initialize_Prototype(void* pArg) override;
    virtual HRESULT Initialize(void* pArg) override;
public:
    // 폴리곤 윤곽 업데이트: points[0..n-1], 자동으로 0-1,1-2,...,n-2,n-1,n-1-0 연결
    HRESULT Update_PolygonEdges(const std::vector<Vec3>& points);

    // (선택) 임의 라인 집합: lineCount 개의 선분, vertices.size() == lineCount*2 로 맞춰 쓰는 버전도 가능
    HRESULT Update_Lines(const std::vector<Vec3>& lineVertices);
    _bool IntersectWithPlane(OUT Vec3& vOut);
public:
    static CVIBuffer_Line_Color* Create(
        ID3D11Device* pDevice,
        ID3D11DeviceContext* pDeviceContext,
        void* pArg);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

private:
    Vec4 m_vColor = {0.f, 1.f, 0.f, 1.f};
    _uint   m_iMaxLineCount = 0;   // capacity
};

NS_END