//--------------------------------------------------------------------------------------
// File: DebugDraw.h
//
// Helpers for drawing various debug shapes using PrimitiveBatch
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#pragma once

#include "Engine_Define.h"

namespace DX
{
    //void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    const DirectX::BoundingSphere& sphere,
    //    DirectX::FXMVECTOR color = DirectX::Colors::White);

    //void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    const DirectX::BoundingBox& box,
    //    DirectX::FXMVECTOR color = DirectX::Colors::White);

    //void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    const DirectX::BoundingOrientedBox& obb,
    //    DirectX::FXMVECTOR color = DirectX::Colors::White);

    //void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    const DirectX::BoundingFrustum& frustum,
    //    DirectX::FXMVECTOR color = DirectX::Colors::White);

    //void XM_CALLCONV DrawGrid(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis,
    //    DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs,
    //    DirectX::GXMVECTOR color = DirectX::Colors::White);

    //void XM_CALLCONV DrawRing(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    DirectX::FXMVECTOR origin, DirectX::FXMVECTOR majorAxis, DirectX::FXMVECTOR minorAxis,
    //    DirectX::GXMVECTOR color = DirectX::Colors::White);

    //void XM_CALLCONV DrawRay(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    DirectX::FXMVECTOR origin, DirectX::FXMVECTOR direction, bool normalize = true,
    //    DirectX::FXMVECTOR color = DirectX::Colors::White);

    //void XM_CALLCONV DrawTriangle(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
    //    DirectX::FXMVECTOR pointA, DirectX::FXMVECTOR pointB, DirectX::FXMVECTOR pointC,
    //    DirectX::GXMVECTOR color = DirectX::Colors::White);

    inline void XM_CALLCONV DrawRing(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR origin,
        FXMVECTOR majorAxis,
        FXMVECTOR minorAxis,
        GXMVECTOR color = DirectX::Colors::White);


    inline void XM_CALLCONV DrawCube(PrimitiveBatch<VertexPositionColor>* batch,
        CXMMATRIX matWorld,
        FXMVECTOR color = DirectX::Colors::White)
    {
        static const XMVECTORF32 s_verts[8] =
        {
            { { { -1.f, -1.f, -1.f, 0.f } } },
            { { {  1.f, -1.f, -1.f, 0.f } } },
            { { {  1.f, -1.f,  1.f, 0.f } } },
            { { { -1.f, -1.f,  1.f, 0.f } } },
            { { { -1.f,  1.f, -1.f, 0.f } } },
            { { {  1.f,  1.f, -1.f, 0.f } } },
            { { {  1.f,  1.f,  1.f, 0.f } } },
            { { { -1.f,  1.f,  1.f, 0.f } } }
        };

        static const WORD s_indices[] =
        {
            0, 1,
            1, 2,
            2, 3,
            3, 0,
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            0, 4,
            1, 5,
            2, 6,
            3, 7
        };

        VertexPositionColor verts[8];
        for (size_t i = 0; i < 8; ++i)
        {
            XMVECTOR v = XMVector3Transform(s_verts[i], matWorld);
            XMStoreFloat3(&verts[i].position, v);
            XMStoreFloat4(&verts[i].color, color);
        }

        batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, _countof(s_indices), verts, 8);
    }

    inline void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingSphere& sphere,
        FXMVECTOR color = DirectX::Colors::White)
    {
        XMVECTOR origin = XMLoadFloat3(&sphere.Center);

        const float radius = sphere.Radius;

        XMVECTOR xaxis = g_XMIdentityR0 * radius;
        XMVECTOR yaxis = g_XMIdentityR1 * radius;
        XMVECTOR zaxis = g_XMIdentityR2 * radius;

        DrawRing(batch, origin, xaxis, zaxis, color);
        DrawRing(batch, origin, xaxis, yaxis, color);
        DrawRing(batch, origin, yaxis, zaxis, color);
    }


    inline void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingBox& box,
        FXMVECTOR color = DirectX::Colors::White)
    {
        XMMATRIX matWorld = XMMatrixScaling(box.Extents.x, box.Extents.y, box.Extents.z);
        XMVECTOR position = XMLoadFloat3(&box.Center);
        matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

        DrawCube(batch, matWorld, color);
    }


    inline void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingOrientedBox& obb,
        FXMVECTOR color = DirectX::Colors::White)
    {
        XMMATRIX matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&obb.Orientation));
        XMMATRIX matScale = XMMatrixScaling(obb.Extents.x, obb.Extents.y, obb.Extents.z);
        matWorld = XMMatrixMultiply(matScale, matWorld);
        XMVECTOR position = XMLoadFloat3(&obb.Center);
        matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

        DrawCube(batch, matWorld, color);
    }


    inline void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingFrustum& frustum,
        FXMVECTOR color = DirectX::Colors::White)
    {
        XMFLOAT3 corners[BoundingFrustum::CORNER_COUNT];
        frustum.GetCorners(corners);

        VertexPositionColor verts[24] = {};
        verts[0].position = corners[0];
        verts[1].position = corners[1];
        verts[2].position = corners[1];
        verts[3].position = corners[2];
        verts[4].position = corners[2];
        verts[5].position = corners[3];
        verts[6].position = corners[3];
        verts[7].position = corners[0];

        verts[8].position = corners[0];
        verts[9].position = corners[4];
        verts[10].position = corners[1];
        verts[11].position = corners[5];
        verts[12].position = corners[2];
        verts[13].position = corners[6];
        verts[14].position = corners[3];
        verts[15].position = corners[7];

        verts[16].position = corners[4];
        verts[17].position = corners[5];
        verts[18].position = corners[5];
        verts[19].position = corners[6];
        verts[20].position = corners[6];
        verts[21].position = corners[7];
        verts[22].position = corners[7];
        verts[23].position = corners[4];

        for (size_t j = 0; j < _countof(verts); ++j)
        {
            XMStoreFloat4(&verts[j].color, color);
        }

        batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, _countof(verts));
    }


    inline void XM_CALLCONV DrawGrid(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR xAxis,
        FXMVECTOR yAxis,
        FXMVECTOR origin,
        size_t xdivs,
        size_t ydivs,
        GXMVECTOR color = DirectX::Colors::White)
    {
        xdivs = std::max<size_t>(1, xdivs);
        ydivs = std::max<size_t>(1, ydivs);

         for (size_t i = 0; i <= xdivs; ++i)
        {
            float percent = float(i) / float(xdivs);
            percent = (percent * 2.f) - 1.f;
            XMVECTOR scale = XMVectorScale(xAxis, percent);
            scale = XMVectorAdd(scale, origin);

            VertexPositionColor v1(XMVectorSubtract(scale, yAxis), color);
            VertexPositionColor v2(XMVectorAdd(scale, yAxis), color);
            batch->DrawLine(v1, v2);
        }

        for (size_t i = 0; i <= ydivs; i++)
        {
            FLOAT percent = float(i) / float(ydivs);
            percent = (percent * 2.f) - 1.f;
            XMVECTOR scale = XMVectorScale(yAxis, percent);
            scale = XMVectorAdd(scale, origin);

            VertexPositionColor v1(XMVectorSubtract(scale, xAxis), color);
            VertexPositionColor v2(XMVectorAdd(scale, xAxis), color);
            batch->DrawLine(v1, v2);
        }
    }


    inline void XM_CALLCONV DrawRing(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR origin,
        FXMVECTOR majorAxis,
        FXMVECTOR minorAxis,
        GXMVECTOR color)
    {
        static const size_t c_ringSegments = 32;

        VertexPositionColor verts[c_ringSegments + 1];

        FLOAT fAngleDelta = XM_2PI / float(c_ringSegments);
        // Instead of calling cos/sin for each segment we calculate
        // the sign of the angle delta and then incrementally calculate sin
        // and cosine from then on.
        XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
        XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
        XMVECTOR incrementalSin = XMVectorZero();
        static const XMVECTORF32 s_initialCos =
        {
            { { 1.f, 1.f, 1.f, 1.f } }
        };
        XMVECTOR incrementalCos = s_initialCos.v;
        for (size_t i = 0; i < c_ringSegments; i++)
        {
            XMVECTOR pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
            pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
            XMStoreFloat3(&verts[i].position, pos);
            XMStoreFloat4(&verts[i].color, color);
            // Standard formula to rotate a vector.
            XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
            XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
            incrementalCos = newCos;
            incrementalSin = newSin;
        }
        verts[c_ringSegments] = verts[0];

        batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
    }


    inline void XM_CALLCONV DrawRay(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR origin,
        FXMVECTOR direction,
        bool normalize,
        FXMVECTOR color = DirectX::Colors::White)
    {
        VertexPositionColor verts[3];
        XMStoreFloat3(&verts[0].position, origin);

        XMVECTOR normDirection = XMVector3Normalize(direction);
        XMVECTOR rayDirection = (normalize) ? normDirection : direction;

        XMVECTOR perpVector = XMVector3Cross(normDirection, g_XMIdentityR1);

        if (XMVector3Equal(XMVector3LengthSq(perpVector), g_XMZero))
        {
            perpVector = XMVector3Cross(normDirection, g_XMIdentityR2);
        }
        perpVector = XMVector3Normalize(perpVector);

        XMStoreFloat3(&verts[1].position, XMVectorAdd(rayDirection, origin));
        perpVector = XMVectorScale(perpVector, 0.0625f);
        normDirection = XMVectorScale(normDirection, -0.25f);
        rayDirection = XMVectorAdd(perpVector, rayDirection);
        rayDirection = XMVectorAdd(normDirection, rayDirection);
        XMStoreFloat3(&verts[2].position, XMVectorAdd(rayDirection, origin));

        XMStoreFloat4(&verts[0].color, color);
        XMStoreFloat4(&verts[1].color, color);
        XMStoreFloat4(&verts[2].color, color);

        batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
    }


    inline void XM_CALLCONV DrawTriangle(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR pointA,
        FXMVECTOR pointB,
        FXMVECTOR pointC,
        GXMVECTOR color = DirectX::Colors::White)
    {
        VertexPositionColor verts[4];
        XMStoreFloat3(&verts[0].position, pointA);
        XMStoreFloat3(&verts[1].position, pointB);
        XMStoreFloat3(&verts[2].position, pointC);
        XMStoreFloat3(&verts[3].position, pointA);

        XMStoreFloat4(&verts[0].color, color);
        XMStoreFloat4(&verts[1].color, color);
        XMStoreFloat4(&verts[2].color, color);
        XMStoreFloat4(&verts[3].color, color);

        batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 4);
    }

    inline void XM_CALLCONV DrawMesh(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        const PxGeometryHolder& geom,
        const PxTransform& globalPose,
        CXMMATRIX matWorld,
        DirectX::GXMVECTOR color = DirectX::Colors::White)
    {
        if (geom.getType() != PxGeometryType::eTRIANGLEMESH)
            return;

        PxTriangleMeshGeometry triMeshGeom = geom.triangleMesh();
        PxTriangleMesh* triMesh = triMeshGeom.triangleMesh;
        if (!triMesh)
            return;

        const PxVec3* pxVerts = triMesh->getVertices();
        const void* pxTris = triMesh->getTriangles();
        const PxU32 numVerts = triMesh->getNbVertices();
        const PxU32 numTris = triMesh->getNbTriangles();

        _bool is16Bit = triMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;

        static vector<VertexPositionColor> batchVerts;
        batchVerts.clear();
        batchVerts.reserve(numVerts);

        PxMeshScale meshScale = triMeshGeom.scale;

        XMFLOAT4 vColor;
        XMStoreFloat4(&vColor, color);

        for (PxU32 i = 0; i < numVerts; i++)
        {
            PxVec3 v = meshScale.transform(pxVerts[i]);
            Vec4 vPos = XMVector3TransformCoord(XMLoadFloat3((XMFLOAT3*)&v), matWorld);

            VertexPositionColor vert;
            XMStoreFloat3(&vert.position, vPos);
            vert.color = vColor;

            batchVerts.push_back(vert);
        }

        static vector<_ushort> batchIndices;
        batchIndices.clear();
        batchIndices.reserve(numTris * 6); // 삼각형 1개당 선 3개(인덱스 6개)

        for (PxU32 i = 0; i < numTris; i++)
        {
            _uint i0, i1, i2;

            if (is16Bit)
            {
                const PxU16* indices = (const PxU16*)pxTris;
                i0 = indices[i * 3 + 0];
                i1 = indices[i * 3 + 1];
                i2 = indices[i * 3 + 2];
            }
            else
            {
                const PxU32* indices = (const PxU32*)pxTris;
                i0 = indices[i * 3 + 0];
                i1 = indices[i * 3 + 1];
                i2 = indices[i * 3 + 2];
            }

            batchIndices.push_back((_ushort)i0);
            batchIndices.push_back((_ushort)i1);

            batchIndices.push_back((_ushort)i1);
            batchIndices.push_back((_ushort)i2);

            batchIndices.push_back((_ushort)i2);
            batchIndices.push_back((_ushort)i0);
        }

        batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            batchIndices.data(),
            batchIndices.size(),
            batchVerts.data(),
            batchVerts.size());
    }

    inline void XM_CALLCONV DrawCapsule(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        const DirectX::BoundingSphere& sphere,
        _float halfHeight,
        DirectX::GXMVECTOR color = DirectX::Colors::White)
    {
        ///
        /// Head
        ///
        XMVECTOR originHead = XMLoadFloat3(&sphere.Center);
        originHead = XMVectorSetY(originHead, XMVectorGetY(originHead) + halfHeight);

        const float radius = sphere.Radius;

        XMVECTOR xaxisHead = g_XMIdentityR0 * radius;
        XMVECTOR yaxisHead = g_XMIdentityR1 * radius;
        XMVECTOR zaxisHead = g_XMIdentityR2 * radius;

        DrawRing(batch, originHead, xaxisHead, zaxisHead, color);
        DrawRing(batch, originHead, xaxisHead, yaxisHead, color);
        DrawRing(batch, originHead, yaxisHead, zaxisHead, color);



        ///
        /// Foot
        ///
        XMVECTOR originFoot = XMLoadFloat3(&sphere.Center);
        originFoot = XMVectorSetY(originFoot, XMVectorGetY(originFoot) - halfHeight);

        XMVECTOR xaxisFoot = g_XMIdentityR0 * radius;
        XMVECTOR yaxisFoot = g_XMIdentityR1 * radius;
        XMVECTOR zaxisFoot = g_XMIdentityR2 * radius;

        DrawRing(batch, originFoot, xaxisFoot, zaxisFoot, color);
        DrawRing(batch, originFoot, xaxisFoot, yaxisFoot, color);
        DrawRing(batch, originFoot, yaxisFoot, zaxisFoot, color);



        ///
        /// Body
        ///
        XMVECTOR vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        XMVECTOR vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

        XMFLOAT4 vColor;
        XMStoreFloat4(&vColor, color);

        VertexPositionColor v1;
        VertexPositionColor v2;
        v1.color = vColor;
        v2.color = vColor;

        {
            XMStoreFloat3(&v1.position, originHead + vRight * radius);
            XMStoreFloat3(&v2.position, originFoot + vRight * radius);
            batch->DrawLine(v1, v2);
        }

        {
            XMStoreFloat3(&v1.position, originHead - vRight * radius);
            XMStoreFloat3(&v2.position, originFoot - vRight * radius);
            batch->DrawLine(v1, v2);
        }

        {
            XMStoreFloat3(&v1.position, originHead + vLook * radius);
            XMStoreFloat3(&v2.position, originFoot + vLook * radius);
            batch->DrawLine(v1, v2);
        }

        {
            XMStoreFloat3(&v1.position, originHead - vLook * radius);
            XMStoreFloat3(&v2.position, originFoot - vLook * radius);
            batch->DrawLine(v1, v2);
        }
    }
}


