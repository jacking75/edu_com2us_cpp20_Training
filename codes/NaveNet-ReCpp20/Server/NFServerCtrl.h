/** 
 *  @file  		NFServerCtrl.h
 *  @brief 		IOCP Base ��Ʈ�ѷ� Ŭ����
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#include "NFConnection.h"
#include <vector>
#include <thread>

namespace NaveNetLib {

//	extern INT iMaxRecvPacket;

	typedef struct LISTENER
	{
		SOCKET	s;		// Listen Socket
		INT		nPort;	// ��Ʈ
		CHAR	cBindQue;	// ���ε� ť

		void Init()
		{
			s = INVALID_SOCKET;		// Listen Socket
			nPort = 0;	// ��Ʈ
			cBindQue = 0;
		}

		void Release()
		{
			// Ŭ���� ���� ���� ť�� ����Ÿ�� ������ ���� �����ϴ� �ɼ�
			struct linger li = {1, 0};	// Default: SO_DONTLINGER
			shutdown(s, SD_BOTH);
			setsockopt(s, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
			closesocket(s );

			Init();
		}
	}*LPLISTENER;

	class NFServerCtrl
	{
	public:
		NFServerCtrl();
		virtual ~NFServerCtrl();

	public:
		SOCKET						CreateListenSocket(INT nServerPort, CHAR cBindQue = 32);		// Listen Socket ���� 
		HANDLE						ConnectIOCPSocket(SOCKET sckListener);		// Listen Socket �� iocp ���Ͽ� ����

		inline HANDLE				GetIOCP() { return m_hIOCP; };

		inline bool					IsRun() { return m_bServerRun; };
		inline bool					IsPause() { return m_bPause; };

		//static unsigned __stdcall	WINAPI	Thread_MainEx(LPVOID lpvoid);		// IOCP ���� ���� Thread
		//static unsigned __stdcall	WINAPI	Process_MainEx(LPVOID lpvoid);		// Process ó�� Thread
		//static unsigned __stdcall	WINAPI	Packet_MainEx(LPVOID lpvoid);		// Packet ó�� Thread
		void Thread_MainEx();		// IOCP ���� ���� Thread
		void Process_MainEx();		// Process ó�� Thread
		void Packet_MainEx();		// Packet ó�� Thread

		virtual void				Update();									// Process ó�� �Լ�

		virtual void				ShowServerInfo();							// ���۽� ȭ�鿡 ���� ���� ǥ��

		virtual bool				CreateSession(SOCKET sckListener) { return true; }		// Client Session �� �����Ѵ�.
		virtual bool				ReleaseSession() { return true; }	// Client Session �� �����Ѵ�.

	protected:

		HANDLE						CreateIOCP();								// IOCP �ڵ� ����
		bool						InitSocket();								// ���� ���̺귯�� Ȱ��
		INT							GetNumberOfProcess();						// CPU���� ���Ѵ� => ������ ������ �̿� 
		INT							GetNumberOfThread() { return m_nMaxThreadNum; };	// ������ �������� ������ ���Ѵ�.

		bool						Start(INT nPort, INT nMaxConn, INT nMaxThreadNum = 0);	// ���� ����
		void						Pause(bool bPause);
		bool						Stop();										// ���� ���� 

		void WSAGetLastErrorToTextMessage(char *pMsg);

	protected:
		INT							m_nMaxThreadNum;							// �ִ� ������ �� ���� 
		//HANDLE*						m_pWorkThread;								// IOCP Work ������ �ڵ� 
		HANDLE						m_hIOCP;									// IOCP �ڵ� 

		std::vector<LISTENER>		m_vecListener;								// ������ ����Ʈ �������� �����ʸ� ������ ���� �ִ�.

		//HANDLE						m_hProcThread;								// ���μ����� ���� ������
		//HANDLE						m_hPacketThread;							// PacketUpdate�� ���� ������

		std::vector<std::thread> m_MainThreadList;
		std::thread m_ProcessThread;
		std::thread m_PacketThread;

		INT							m_iPort;
		INT							m_iMaxConn;

		bool						m_bPause;
		bool						m_bServerRun;								// ���� ���� ���� ����Ǹ� true����.

		NFUpdateManager*			m_pUpdateManager;
	};

}