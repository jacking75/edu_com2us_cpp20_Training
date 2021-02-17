
#include <stdio.h>
#include <iostream>

#include "../../Server/NFConnectionManager.h"

#include "TestConnection.h"


using namespace NaveNetLib;


void TestConnection::Init(NaveNetLib::NFConnectionManager* pNFConnectionManager)
{
	m_pNFConnectionManager = pNFConnectionManager;

	// 함수를 Map으로 선언한다.
	OnMsgMap[(int)PACKETLIST::IRC] = &TestConnection::Parsed_IRC;
	OnMsgMap[(int)PACKETLIST::TEST] = &TestConnection::Parsed_TEST;
}

//////////////////////////////////////////////////////////////////////
// 변수 초기화..
//////////////////////////////////////////////////////////////////////
void TestConnection::Clear()
{
	m_SendPacket.Init();
}

void TestConnection::DispatchPacket(NFPacket& Packet)
{
	//	InterlockedExchange((LONG*)&g_iProcessPacket,g_iProcessPacket+1);

	// 함수 호출
	(this->*OnMsgMap[Packet.GetCommand()])(Packet.m_Packet, Packet.GetSize());
	/*
	if(Packet.GetCommand() == 0)
	Parsed_IRC(Packet.m_Packet, Packet.GetSize());
	else
	Parsed_TEST(Packet.m_Packet, Packet.GetSize());
	*/
}

void TestConnection::OnConnect(bool bConnect)
{
	if (bConnect)
	{
		char logMsg[DEF_PACKETSIZE];	memset(logMsg, 0, sizeof(logMsg));
		_snprintf_s(logMsg, _countof(logMsg), _TRUNCATE, "%d : Connect User", GetIndex());
		std::cout << logMsg << std::endl;

		m_pNFConnectionManager->Join(this);
	}
}

void TestConnection::OnDisconnect()
{
	m_pNFConnectionManager->Levae(GetIndex());

	char logMsg[DEF_PACKETSIZE];	memset(logMsg, 0, sizeof(logMsg));
	_snprintf_s(logMsg, _countof(logMsg), _TRUNCATE, "%d : Disconnect User", GetIndex());
	std::cout << logMsg << std::endl;

	m_SendPacket.Init();
}

void TestConnection::Parsed_IRC(char* Packet, [[maybe_unused]] int len)
{
	auto pkIRC = (LPPKIRC)Packet;
		
	char logMsg[DEF_PACKETSIZE];	
	memset(logMsg, 0, sizeof(logMsg));
	_snprintf_s(logMsg, _countof(logMsg), _TRUNCATE, "%s : %s", pkIRC->Key, pkIRC->Message);
	std::cout << logMsg << std::endl;
}

void TestConnection::Parsed_TEST(char* Packet, int Len)
{
	LPPKTEST pkTest = (LPPKTEST)Packet;

	m_SendPacket.SetCommand((USHORT)PACKETLIST::TEST);
	m_SendPacket.SetSize(sizeof(PKTEST));
	memcpy(m_SendPacket.m_Packet, pkTest->Message, Len);

	SendPost(m_SendPacket);
}

void TestConnection::Send_IRC(const char* strCommand, const char* strMsg)
{
	m_SendPacket.SetCommand((USHORT)PACKETLIST::IRC);
	m_SendPacket.SetSize(sizeof(PKIRC));

	LPPKIRC lpIRC = (LPPKIRC)m_SendPacket.m_Packet;

	strcpy_s(lpIRC->Key, 64, strCommand);
	strcpy_s(lpIRC->Message, 512, strMsg);

	SendPost(m_SendPacket);
}

