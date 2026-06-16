#include "Engine_pch.h"
#include "Network_Manager.h"
#include "EngineConsole.h"

#include "PacketHeader.h"

#include "Sys_ConnectResponsePacket.h"
#include "LoginPacket.h"
#include "RoomChatPacket.h"
#include "RoomPacket.h"

CNetwork_Manager::CNetwork_Manager()
{
}

CNetwork_Manager::~CNetwork_Manager()
{
}

HRESULT CNetwork_Manager::Initialize(const char* ip, int tcpPort, int udpPort)
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);

#ifdef _DEBUG
	m_TCPSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = ::htons(tcpPort);

	//IP
	::inet_pton(AF_INET, ip, &serverAddr.sin_addr);
	if (::connect(m_TCPSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		CEngineConsole::Log(ELogLevel::Error, "TCP Connect failed");
		return E_FAIL;
	}

	CEngineConsole::Log(ELogLevel::Info, "TCP Connect Success");

	m_ServerUDPAddr = {};
	m_ServerUDPAddr.sin_family = AF_INET;
	m_ServerUDPAddr.sin_port = ::htons(udpPort);
	::inet_pton(AF_INET, ip, &m_ServerUDPAddr.sin_addr);

	m_UDPSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN localAddr = {};
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	localAddr.sin_port = ::htons(0);
	::bind(m_UDPSocket, (SOCKADDR*)&localAddr, sizeof(localAddr));
#else
	m_TCPSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = ::htons(tcpPort);

	//DNS
	ADDRINFO hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	ADDRINFO* result = nullptr;
	if (::getaddrinfo(ip, nullptr, &hints, &result) != 0)
	{
		CEngineConsole::Log(ELogLevel::Error, "DNS Resolution failed");
		return E_FAIL;
	}

	SOCKADDR_IN* ipv4 = (SOCKADDR_IN*)result->ai_addr;
	serverAddr.sin_addr = ipv4->sin_addr;

	m_ServerUDPAddr = {};
	m_ServerUDPAddr.sin_family = AF_INET;
	m_ServerUDPAddr.sin_port = ::htons(udpPort);
	m_ServerUDPAddr.sin_addr = ipv4->sin_addr;
	//::inet_pton(AF_INET, ip, &m_ServerUDPAddr.sin_addr);

	::freeaddrinfo(result);

	if (::connect(m_TCPSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		CEngineConsole::Log(ELogLevel::Error, "TCP Connect failed");
		return E_FAIL;
	}

	CEngineConsole::Log(ELogLevel::Info, "TCP Connect Success");

	m_UDPSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN localAddr = {};
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	localAddr.sin_port = ::htons(0);
	::bind(m_UDPSocket, (SOCKADDR*)&localAddr, sizeof(localAddr));
#endif // _DEBUG

	m_bIsRunning = true;
	m_TCPRecvThread = thread([this]() { RecvTCPThread(); });
	m_UDPRecvThread = thread([this]() { RecvUDPThread(); });

	return S_OK;
}

void CNetwork_Manager::Update()
{
	lock_guard<mutex> lock(m_RecvMutex);
	while (!m_RecvQueue.empty())
	{
		auto packet = m_RecvQueue.front();
		m_RecvQueue.pop();

		auto* pHeader = reinterpret_cast<PACKET_HEADER*>(packet.data());

		switch (pHeader->PacketId)
		{
		case PACKET_ID::SYS_USER_CONNECT_RESPONSE:
		{
			auto* pRes = reinterpret_cast<SYS_CONNECT_RESPONSE_PACKET*>(packet.data());
			CEngineConsole::Log(ELogLevel::Info, "Connect");
			SetClientIndex(pRes->ClientId);
			
			LOGIN_REQUEST_PACKET loginResPacket = {};
			loginResPacket.PacketId = PACKET_ID::LOGIN_REQUEST;
			loginResPacket.PacketLength = sizeof(LOGIN_REQUEST_PACKET);
			m_strUserId = ::to_string(m_iClientIndex);
			strcpy_s(loginResPacket.UserID, m_strUserId.c_str());
			SendTCP(reinterpret_cast<char*>(&loginResPacket), loginResPacket.PacketLength);
		}
		break;
		case PACKET_ID::LOGIN_RESPONSE:
		{
			auto* pRes = reinterpret_cast<LOGIN_RESPONSE_PACKET*>(packet.data());
			CEngineConsole::Log(ELogLevel::Info, "Login");
		}
		break;
		case PACKET_ID::CHARACTER_SYNC_BROADCAST:
		{
			auto* pPos = reinterpret_cast<CHARACTER_SYNC_PACKET*>(packet.data());
			if (pPos->ClientIndex == m_iClientIndex)
				break;
			else
				m_DicUserSync[pPos->ClientIndex] = *pPos;
		}
		break;
		case PACKET_ID::ROOM_JOIN_NOTIFY:
		{
			RoomJoinUser(packet.data());
		}
		break;
		case PACKET_ID::ROOM_LEAVE_NOTIFY:
		{
			RoomLeaveUser(packet.data());
		}
		break;
		}
	}
}

void CNetwork_Manager::Destroy()
{
	m_bIsRunning = false;
	::closesocket(m_TCPSocket);
	::closesocket(m_UDPSocket);
	if (m_TCPRecvThread.joinable())
		m_TCPRecvThread.join();
	if (m_UDPRecvThread.joinable())
		m_UDPRecvThread.join();
	::WSACleanup();
}

void CNetwork_Manager::SendTCP(char* pData, UINT32 size)
{
	::send(m_TCPSocket, pData, size, 0);
}

void CNetwork_Manager::SendUDP(char* pData, UINT32 size)
{
	::sendto(m_UDPSocket, pData, size, 0, (SOCKADDR*)&m_ServerUDPAddr, sizeof(m_ServerUDPAddr));
}

void CNetwork_Manager::RoomEnterRequest()
{
	ROOM_ENTER_REQUEST_PACKET roomEnterResPacket = {};
	roomEnterResPacket.PacketId = PACKET_ID::ROOM_ENTER_REQUEST;
	roomEnterResPacket.PacketLength = sizeof(ROOM_ENTER_REQUEST_PACKET);
	roomEnterResPacket.RoomNumber = 0;
	SendTCP(reinterpret_cast<char*>(&roomEnterResPacket), roomEnterResPacket.PacketLength);
}

void CNetwork_Manager::RecvTCPThread()
{
	char headerBuf[sizeof(PACKET_HEADER)];
	while (m_bIsRunning)
	{
		int recvLen = ::recv(m_TCPSocket, headerBuf, sizeof(PACKET_HEADER), MSG_WAITALL);
		if (recvLen <= 0)
			break;

		auto* pHeader = (PACKET_HEADER*)headerBuf;
		UINT32 bodySize = pHeader->PacketLength - sizeof(PACKET_HEADER);

		vector<char> fullPacket(pHeader->PacketLength);
		CopyMemory(fullPacket.data(), headerBuf, sizeof(PACKET_HEADER));

		if (bodySize > 0)
			::recv(m_TCPSocket, fullPacket.data() + sizeof(PACKET_HEADER), bodySize, MSG_WAITALL);
		lock_guard<mutex> lock(m_RecvMutex);
		m_RecvQueue.push(fullPacket);
	}
}

void CNetwork_Manager::RecvUDPThread()
{
	char buf[1024];
	SOCKADDR_IN fromAddr;
	int fromLen = sizeof(fromAddr);

	while (m_bIsRunning)
	{
		int recvLen = ::recvfrom(m_UDPSocket, buf, sizeof(buf), 0, (SOCKADDR*)&fromAddr, &fromLen);
		if (recvLen == SOCKET_ERROR)
			break;
		
		if (recvLen == 0)
			continue;

		vector<char> packet(buf, buf + recvLen);
		lock_guard<mutex> lock(m_RecvMutex);
		m_RecvQueue.push(packet);
	}
}

void CNetwork_Manager::RoomJoinUser(char* pData)
{
	auto* pRes = reinterpret_cast<ROOM_JOIN_PACKET*>(pData);
	auto pUser = make_shared<UserModel>();
	pUser->ClientIndex = pRes->ClientIndex;
	strncpy_s(pUser->UserID, pRes->UserID, sizeof(pRes->UserID));
	m_UserList.push_back(pUser);

	m_funcJoinedUser(*pUser);
}

void CNetwork_Manager::RoomLeaveUser(char* pData)
{
	auto* pRes = reinterpret_cast<ROOM_LEAVE_PACKET*>(pData);

	m_UserList.remove_if([leaveUserId = pRes->ClientIndex](shared_ptr<UserModel> pUser)
		{
			return leaveUserId == pUser->ClientIndex;
		});

	m_funcLeftUser(pRes->ClientIndex);
}

CNetwork_Manager* CNetwork_Manager::Create(const char* ip, int tcpPort, int udpPort)
{
	CNetwork_Manager* pInstance = new CNetwork_Manager();

	if (FAILED(pInstance->Initialize(ip, tcpPort, udpPort)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CNetwork_Manager");
	}

	return pInstance;
}

void CNetwork_Manager::Free()
{
	Destroy();
}
