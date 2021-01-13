#pragma once

#include "../../Client/NaveNet.h"
#include "../../Client/NFNetClientS.h"

#include "Packet.h"

class ClientNet : public NaveClientNetLib::NFNetClientS
{
public:
	ClientNet(void);
	~ClientNet(void);

public:
	//////////////////////////////////////////////////////////////////////
	// Socket -> Client�� ��Ŷ�� �����ϱ� ���� �����ϴ� Listener�̴�.
	//////////////////////////////////////////////////////////////////////
	class IEventListener
	{
	public:
		virtual ~IEventListener() {}
		virtual void EventIRC(char* strCmd, char* strMsg) = 0;
		virtual void EventTEST(char* strMsg) = 0;
		virtual void EventConnect(bool bConnect) = 0;
		virtual void EventSocketClose() = 0;
	};

	void	SetEventListener(IEventListener* event)
	{
		m_event = event;
	}

public:
	virtual void	Update();

	//////////////////////////////////////////////////////////////////////
	// ToServer�� ���ǵ� ��Ŷ���
	// ���޹��� ��Ŷ�� ó���� ��Ŷ�Լ� ������ Recv_��Ŷ�̸�(CHAR* Packet, int Len) �̵ȴ�.
	//////////////////////////////////////////////////////////////////////
	void	Send_IRC(const char* strCommand, const  char* strMsg);

	void	Send_TEST(const  char* strMsg);

	void	Disconnect();

private:
	//////////////////////////////////////////////////////////////////////
	// CNetTest���� ���Ǵ� ��������
	//////////////////////////////////////////////////////////////////////
	//	Nave::Sync::CSync					m_Cs;

	// Send�� �ϱ����� ��Ŷ�� ���鶧 ���ȴ�.
	NaveClientNetLib::NFPacket					m_SendPacket;

	IEventListener*						m_event;

private:
	void	(ClientNet::*OnMsgMap[MaxPacket])(char* Packet, int Len);

	// ���� �κп��� �������� �̺�Ʈ�� ó���Ҷ�
	void	OnSocketEvent(DWORD dID, DWORD dEvent);

	//////////////////////////////////////////////////////////////////////
	// FromServer�� ���ǵ� ��Ŷ��� (Ŭ���̾�Ʈ�� �������.)
	// ���޹��� ��Ŷ�� ó���� ��Ŷ�Լ� ������ Recv_��Ŷ�̸�(CHAR* Packet, int Len) �̵ȴ�.
	//////////////////////////////////////////////////////////////////////

	// IRC Packet ó���Լ�
	void	Parsed_IRC(char* Packet, int Len);
	void	Parsed_TEST(char* Packet, int Len);

	void	ProcessPacket(NaveClientNetLib::NFPacket* pPacket, int PacketLen);

};