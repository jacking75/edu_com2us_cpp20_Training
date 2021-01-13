#include "ClientNet.h"


CNetTest::CNetTest(void) : m_event(0)
{
	// �Լ��� Map���� �����Ѵ�.
	OnMsgMap[IRC] = &CNetTest::Parsed_IRC;
	OnMsgMap[TEST] = &CNetTest::Parsed_TEST;
}

CNetTest::~CNetTest(void)
{
	Disconnect();
}

void CNetTest::Disconnect()
{
	NFNetClientS::Disconnect();

	m_SendPacket.Init();
}

void CNetTest::OnSocketEvent(DWORD dID, DWORD dEvent)
{
	// ���Ͽ��� �Ͼ �̺�Ʈ�� ó���ϱ� ���Ѱ�.
	if (dID == WM_CONNECT)			// ���� �̺�Ʈ
	{
		if (m_event)
			m_event->EventConnect(dEvent);
	}
	else if (dID == WM_SOCK_CLOSE)	// ���� ���� �̺�Ʈ
	{
		if (m_event)
			m_event->EventSocketClose();
	}
	else if (dID == WM_RECV_MSG)		// ��Ŷ �Ѱ� ���� �̺�Ʈ
	{
		// Test�Ҷ��� ���ȴ� ���߿� �������� ���� ������.
		//			::PostMessage(m_hWnd, dID, dEvent, NULL); // ������ Packet Data return
	}
}

void CNetTest::Update()
{
	// Socket Update;
	NFNetClientS::Update();


	// ��Ÿó��
}

void CNetTest::ProcessPacket(NFPacket* pPacket, int PacketLen)
{
	/*	if(pPacket->GetCommand() == 0)
	Parsed_IRC(pPacket->m_Packet, pPacket->GetSize());
	else
	Parsed_TEST(pPacket->m_Packet, pPacket->GetSize());
	*/
	// �Լ� ȣ��
	(this->*OnMsgMap[pPacket->GetCommand()])(pPacket->m_Packet, pPacket->GetSize());
}

void CNetTest::Parsed_IRC(CHAR* Packet, int Len)
{
	LPPKIRC pkIRC = (LPPKIRC)Packet;

	if (m_event)
		m_event->EventIRC(pkIRC->Key, pkIRC->Message);
}

void CNetTest::Parsed_TEST(CHAR* Packet, int Len)
{
	LPPKTEST pkTest = (LPPKTEST)Packet;

	if (m_event)
		m_event->EventTEST(pkTest->Message);
}

void CNetTest::Send_IRC(const char* strCommand, const char* strMsg)
{
	//	Sync::CLive CL(&m_Cs);

	m_SendPacket.SetCommand(IRC);
	m_SendPacket.SetSize(sizeof(PKIRC));

	LPPKIRC lpIRC = (LPPKIRC)m_SendPacket.m_Packet;

	strcpy(lpIRC->Key, strCommand);
	strcpy(lpIRC->Message, strMsg);

	SendPost(m_SendPacket);
}

void CNetTest::Send_TEST(const  char* strMsg)
{
	//	Sync::CLive CL(&m_Cs);

	m_SendPacket.SetCommand(TEST);
	m_SendPacket.SetSize(sizeof(PKTEST));

	LPPKTEST lpTest = (LPPKTEST)m_SendPacket.m_Packet;

	lpTest->TestKey = timeGetTime();
	sprintf(lpTest->Message, "%d : %s", lpTest->TestKey, strMsg);

	SendPost(m_SendPacket);
}