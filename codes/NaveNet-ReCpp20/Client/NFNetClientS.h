/** 
 *  @file		NFNetClientS.h
 *  @brief		Network Client �̱۽����� Ŭ����
 *  @remarks	
 *  @author		������(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once

#include "NFIOBuffer.h"

namespace NaveClientNetLib {

	/** 
	 *  @class        NFNetClientS
	 *  @brief        Ŭ���̾�Ʈ���� ������ �����ϱ� ���� Network��ü				\r\n
	 *                �̱� ����������� ���۵� Ŭ������ Update���� ��ŶRecv,Sendó��\r\n
	 *                MainLoop���� Update�Լ� ȣ��� ��Ŷť�� ���ŵ�				\r\n
	 *  @remarks      
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFNetClientS 
	{
	public:
		/// NFNetClient ������
		NFNetClientS();
		/// NFNetClient �Ҹ���
		virtual ~NFNetClientS();

	public:
		/// ���� IP�� ���ɴϴ�.
		void			GetLocalIP(CHAR* LocalIP);

		/// Connect���¸� ���ɴϴ�.
		BOOL			IsConnect() { return m_bConnect; }

		/// ��Ŷ�� Send �մϴ�.
		BOOL			SendPost(NFPacket& Packet);

		/**
		 * @brief		Ŭ���� �ʱ�ȭ.
		 * @param szIP	������ ������
		 * @param nPort  ������ ��Ʈ
		 * @return			��������
		 */
		BOOL			Init(LPCSTR szIP, int nPort);

		/// Disconnect ��ŵ�ϴ�.
		virtual void	Disconnect();

		/// Network�� ������Ʈ �մϴ�.
		virtual void	Update();

		/// Ŭ���̾�Ʈ ���� 
		void			Stop();					
		/// ��� ��ü�� Close �մϴ�.
		void			CloseAll();

	private:
		/// ���� �̺�Ʈ 
		virtual void	OnSocketEvent(DWORD dID, DWORD dEvent) {};
		/// ��Ŷ ���μ���
		virtual void	ProcessPacket(NFPacket* Packet, int PacketLen) { };

		/// ���� �̺�Ʈ �ڵ鷯 
		BOOL			NetworkEventHanlder(LONG lEvent);						

	private:
		/// Event Thread 
		static DWORD WINAPI EventThreadProc(LPVOID lParam);	

		/// Send �̺�Ʈ ó��
		VOID	OnSendPacketData();
		/// Recv �̺�Ʈ ó��
		VOID	OnReadPacketData();
	
		/// RecvQue�� ����
		int		GetQueCnt();
		/// ù ��Ŷ ������
		int		GetPacket(NFPacket** Packet);
		/// ť���� ��Ŷ�� ������.
		void	PopPacket();
		
		/// ť�� ������Ʈ �Ѵ�.
		void	UpdateQue();

		/// Connect �����̺�Ʈ
		BOOL	OnConnect();					
		/// Close �����̺�Ʈ
		BOOL	OnClose();						

		/// ������ Connect�� �մϴ�.
		BOOL	Connect();
		/// ���� �ʱ�ȭ
		BOOL	WinSockInit();

	private:
		/// Ŭ���̾�Ʈ ���� 
		SOCKET				m_hSocket;			
		/// ��Ʈ 
		UINT				m_nPort;			
		/// Server IP���� 
		CHAR				m_strIPAddr[32];	

		/// Recv Queue
		NFPacketQueue		m_RecvQueue;		

		/// Send Queue
		NFPacketQueue		m_SendQueue;		

		/// ��Ʈ��ũ �̺�Ʈ �ڵ鷯 
		WSAEVENT			m_hEvent;			

		/// Close �Ȼ������� 
		BOOL				m_bClose;
		/// ���� ���� �÷��� 
		BOOL				m_bConnect;			

		/// RecvIO Buffer (�ϼ��� ��Ŷ�� �����)
		NFPacketIOBuffer		m_RecvIO;
		/// RecvPacket ���� (�ϼ��� �Ѱ��� ��Ŷ)
		NFPacket				m_RecvPacket;

		/// Recv Buffer (�����忡�� ��Ŷ�� ������ ���)
		CHAR				m_RecvBuffer[DEF_MAXPACKETSIZE];

		/// Recv ������ �ڵ� 
		HANDLE				m_hEventThread;		
	};


}