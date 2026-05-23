#pragma once

#pragma comment(lib, "ws2_32")
#include <winSock2.h>
#include <ws2tcpip.h>


#include "Base.h"
class CNetwork_Manager :
    public CBase
{
    using Super = CBase;
private:
    CNetwork_Manager();
    virtual ~CNetwork_Manager();

    HRESULT Initialize(const char* ip, int tcpPort, int udpPort);

public:
    void Update();
    void Destroy();

    void SendTCP(char* pData, UINT32 size);
    void SendUDP(char* pData, UINT32 size);

    INT32 GetClientIndex() { return m_iClientIndex; }
    void SetClientIndex(INT32 idx) { m_iClientIndex = idx; }

private:
    void RecvTCPThread();
    void RecvUDPThread();

    SOCKET m_TCPSocket = INVALID_SOCKET;
    SOCKET m_UDPSocket = INVALID_SOCKET;
    SOCKADDR_IN m_ServerUDPAddr = {};

    thread m_TCPRecvThread;
    thread m_UDPRecvThread;
    bool m_bIsRunning = false;

    INT32 m_iClientIndex = { -1 };

    mutex m_RecvMutex;
    queue<vector<char>> m_RecvQueue;


public:
    static CNetwork_Manager* Create(const char* ip, int tcpPort, int udpPort);
    virtual void Free() override;
};

