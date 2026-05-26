#pragma once

#include "Base.h"
#include "UserModel.h"
#include "CharacterSyncPacket.h"

NS_BEGIN(Engine)

struct CHARACTER_SYNC_PACKET;

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

    CHARACTER_SYNC_PACKET GetUserSyncData(UINT32 clientID) { return m_DicUserSync[clientID]; }

private:
    void RecvTCPThread();
    void RecvUDPThread();

    //TODO: packet manager
    void RoomJoinUser(char* pData);
    void RoomLeaveUser(char* pData);

    SOCKET m_TCPSocket = INVALID_SOCKET;
    SOCKET m_UDPSocket = INVALID_SOCKET;
    SOCKADDR_IN m_ServerUDPAddr = {};

    thread m_TCPRecvThread;
    thread m_UDPRecvThread;
    bool m_bIsRunning = false;

    INT32 m_iClientIndex = { -1 };
    string m_strUserId = { "" };

    mutex m_RecvMutex;
    queue<vector<char>> m_RecvQueue;

    list<shared_ptr<UserModel>> m_UserList;
    unordered_map<int, CHARACTER_SYNC_PACKET> m_DicUserSync;

public:
    static CNetwork_Manager* Create(const char* ip, int tcpPort, int udpPort);
    virtual void Free() override;
};

NS_END