//#include <Nave/Nave.h>
//#include <Nave/NFException.h>
//#include <Nave/NFLog.h>
#include "NaveNet.h"
#include "NFNetClient.h"


namespace NaveClientNetLib {

	NFNetClient::NFNetClient() : m_hSocket(NULL), m_nPort(0), m_hRecvThread(NULL), 
								m_hEventThread(NULL), m_hEvent(NULL), m_bConnect(FALSE), m_bClose(FALSE)
	{
		m_SendQueue.Create(32);
		m_RecvQueue.Create();
		m_RecvIO.NewIOBuf(0);			// 0���� �ϸ� �⺻������ DefaultPacketSize * 1024

		ZeroMemory(m_strIPAddr, sizeof(m_strIPAddr));	// Server IP���� 

		WinSockInit();
	}

	NFNetClient::~NFNetClient()
	{
		// �ٽ� Ŀ���� ����
		OnClose();
		Stop();							// ���� �Լ� ȣ�� 

		// ���ۻ���
		m_SendQueue.Release();
		m_RecvQueue.Release();
		m_RecvIO.DeleteIOBuf();

		WSACleanup();
	}

	BOOL NFNetClient::WinSockInit()
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD( 2, 2 );
	 
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			WSACleanup();
			// Tell the user that we could not find a usable 
			// WinSock DLL.                                  
			return FALSE;
		}
	 
		if ( LOBYTE( wsaData.wVersion ) != 2 ||
				HIBYTE( wsaData.wVersion ) != 2 ) {
			// Tell the user that we could not find a usable //
			// WinSock DLL.   //
			WSACleanup( );
			return FALSE; 
		}
		return TRUE;
	}

	BOOL NFNetClient::Init( LPCSTR szIP, int nPort )
	{	
		m_nPort = nPort;											// ��Ʈ ��ȣ 
		unsigned long dwThreadId = 0;								// ������ ������ ���� ���� 

		// ip ��巹�� 
		IN_ADDR LoginAddress;
		if (GetHostIPByDomain(LoginAddress, szIP))    
		{ 
			strncpy(m_strIPAddr, inet_ntoa(LoginAddress), MAX_PATH); 
		}

		// ������ ��Ų��. ������.. ���� �������ῡ �����Ѵٸ�
		// Netword Event ���� FW_CLOSE�� �߻��� ������ Close�ȴ�.
		if(!Connect())
		{
			// ����������� �����Ѵ�.
			Sleep(100);													// Sleep...
			OnClose();	

			//LOG_ERROR((L"ClientCtrl Init Faild."));
			return FALSE;
		}

		m_bClose = FALSE;

		m_hRecvThread =
				(HANDLE)CreateThread(NULL,							// Security
					0,												// Stack size - use default
					SocketThreadProc,     							// Thread fn entry point
					(void*) this,	      
					0,												// Init flag
					&dwThreadId);									// Thread address

		m_hEventThread =
				(HANDLE)CreateThread(NULL,							// Security
					0,												// Stack size - use default
					EventThreadProc,     							// Thread fn entry point
					(void*) this,	      
					0,												// Init flag
					&dwThreadId);									// Thread address

		return TRUE;
	}

	void NFNetClient::Disconnect()
	{
		// �̰� ������ Close������ϱ� ������  CloseAll() ȣ������ �ʰ� ���� ����.
		OnClose();
		Stop();								// ���� �Լ� ȣ�� 
	}

	void NFNetClient::Stop()
	{
		if (m_hSocket)
		{
			struct linger li = {0, 0};	// Default: SO_DONTLINGER

			li.l_onoff = 1; // SO_LINGER, timeout = 0
			shutdown(m_hSocket, SD_BOTH );						// ����? �̰� ����? ^^;; �㿡 ã�ƺ��� 
																// 2001�� 9�� 6�� 

			// Ŭ���� ���� ���� ť�� ����Ÿ�� ������ ���� �����ϴ� �ɼ�
			setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
			closesocket(m_hSocket);								// ���� �ݱ� 
			m_hSocket = NULL;
		}

		if(m_hRecvThread) CloseHandle(m_hRecvThread);
		m_hRecvThread = NULL;

		if(m_hEventThread) CloseHandle(m_hEventThread);
		m_hEventThread = NULL;

		if(m_hEvent) WSACloseEvent(m_hEvent);
		m_hEvent = NULL;
	}

	void NFNetClient::CloseAll()
	{
		if(m_bConnect)
		{
			OnClose();
			Stop();								// ���� �Լ� ȣ�� 
		}
	}

	void NFNetClient::UpdateQue()
	{
		while(GetQueCnt() != 0)
		{
			// ��Ŷ�� ���޵ƴ��� �˻�.
			NFPacket* pPacket = NULL;
			int Cnt = GetPacket(&pPacket);

			// ����� 0���� �۰ų� Packet �� NULL�̸�.
			if(Cnt <= 0 || pPacket == NULL)
			{
				//LOG_ERROR((L"NFNetClient::UpdateQue() Packet Error"));
				assert( "NFNetClient::UpdateQue() Packet Error.");
			}

			// �Լ� ȣ��
			ProcessPacket(pPacket, Cnt);

			// ��Ŷ ����
			PopPacket();
		}
	}

	void NFNetClient::Update()
	{
		UpdateQue();
	}

	BOOL NFNetClient::OnClose()
	{
		m_bClose = TRUE;
		m_bConnect = FALSE;											// Connect ���� ���� 

		// ���� ���� 
		OnSocketEvent(WM_SOCK_CLOSE, 0);

		return FALSE;
	}

	BOOL NFNetClient::Connect()
	{	
		// �������̶�� 
		if(m_bConnect) return TRUE;

		// ������ ���� �ִٸ� 
		Stop();

		m_RecvQueue.Reset();
		m_SendQueue.Reset();
		m_RecvIO.InitIOBuf();

		// ���� ���� 
		m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		// ���� ���� �˻� 
		if (m_hSocket == INVALID_SOCKET)
		{
			OnSocketEvent(WM_CONNECT, FALSE);
			return FALSE;
		}

		// ��Ʈ��ũ �̺�Ʈ �ڵ� ���� 
		m_hEvent = WSACreateEvent();
		if (m_hEvent == WSA_INVALID_EVENT)
		{
			Stop();
			return FALSE;
		}

		// Request async notification
		int nRet = WSAEventSelect(m_hSocket,
								m_hEvent,
								FD_CLOSE | FD_CONNECT);	// ��ȣ�� �����Ͽ� �ް� �Ѵ� 
		
		// ������� 
		if (nRet == SOCKET_ERROR)
		{
			Stop();
			return FALSE;
		}

		// �񵿱� ���
		unsigned long ul = 1;
		nRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul);
		// ���� ���� �˻� 
		if (m_hSocket == SOCKET_ERROR)
		{
			OnSocketEvent(WM_CONNECT, FALSE);
			return FALSE;
		}

		/////////////////////////////////
		// ������ ���� ����ȭ�� ���� ���� 
		int zero = 0;
		int err = 0;

		// Send Buffer�� ���� ����
		if( (err = setsockopt( m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&zero, sizeof(zero))) == SOCKET_ERROR)
		{
			closesocket(m_hSocket);
			m_hSocket = NULL;
			return FALSE;
		}

		// Receive Buffer�� ���� ���� 
		if((err = setsockopt( m_hSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&zero, sizeof(zero) )) == SOCKET_ERROR)
		{
			closesocket(m_hSocket);
			m_hSocket = NULL;
			return FALSE;
		}

		SOCKADDR_IN		saServer;		

		memset(&saServer,0,sizeof(saServer));

		saServer.sin_family = AF_INET;
		saServer.sin_addr.s_addr = inet_addr(m_strIPAddr);
		saServer.sin_port = htons(m_nPort);

		// ������ Connect
		nRet = connect(m_hSocket,(sockaddr*)&saServer, sizeof(saServer));
		// ���� �����̰ų� ��ŷ�� �Ǿ��ٸ� 
		if (nRet == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Stop();
			return FALSE;
		}
	/*
		�̷��� Connect�� �ϸ� ������ AccetpEx�� �����Ͽ� ClientSceesion�� 
		Recv�����·� ����. 
		���� �̺�Ʈ�� Connect�� �����ϸ� FD_CONNECT�� �߻���Ų��.
	*/
		return TRUE;
	}

	DWORD WINAPI NFNetClient::SocketThreadProc(LPVOID lParam)
	{
		// Ŭ������ ������ ����
		NFNetClient* pThis = reinterpret_cast<NFNetClient*>(lParam);

		BOOL bThreadRun = TRUE;			// ���� ���� ���� 

		// ������ ���� ���� 
		while(bThreadRun)
		{
			if(pThis->m_bClose)
			{
				bThreadRun = FALSE;
				break;
			}

	 		if(!pThis->m_hSocket || !pThis->m_bConnect)
				continue;

			pThis->OnSendPacketData();	
			pThis->OnReadPacketData();	
			Sleep(1);
		}
		return 0;
	}

	DWORD WINAPI NFNetClient::EventThreadProc(LPVOID lParam)
	{
		// Ŭ������ ������ ����
		NFNetClient* pThis = reinterpret_cast<NFNetClient*>(lParam);

		WSANETWORKEVENTS events;		// ��Ʈ��ũ �̺�Ʈ ���� 
		BOOL bThreadRun = TRUE;			// ���� ���� ���� 

		// ������ ���� ���� 
		while(bThreadRun)
		{
			if(pThis->m_bClose)
			{
				bThreadRun = FALSE;
				break;
			}

			DWORD dwRet;
			dwRet = WSAWaitForMultipleEvents(1,
										&pThis->m_hEvent,
										FALSE,
										INFINITE,
										FALSE);
	 
	 		if(!pThis->m_hSocket)
			{	
				// ����
				bThreadRun = FALSE;
				break;
			}
			// Figure out what happened
			int nRet = WSAEnumNetworkEvents(pThis->m_hSocket,
									pThis->m_hEvent,
									&events);
			
			// ���� �������,
			if (nRet == SOCKET_ERROR)	
			{
				//LOG_ERROR((L"EventThread : SOCKET_ERROR, %d", WSAGetLastError()) );
				bThreadRun = FALSE;
				break;
			}

			///////////////////
			// Handle events //
			bThreadRun = pThis->NetworkEventHanlder(events.lNetworkEvents);
		}
		// �̸��� �����尡 ���� �Ǹ� Server�� ���� Ŭ���̾�Ʈ ����!!! <������ ����>
		// ������ �ʱ�ȭ�� ������ ���ֱ� ������ ���⼭ ������ �ʴ´�.
//		pThis->CloseAll();
		return 0;
	}

	VOID NFNetClient::OnSendPacketData()
	{
		//////////////////////////////////////////////////////////////////////////////
		// Send
		int rc = 0;
		int idx = 0,size = 0;
		CHAR* send_temp=NULL;
		NFPacket Packet;

		if((size = m_SendQueue.Get(Packet)) != -1)
		{
			// ���⼭ ��ȣȭ ���� ���� ���� ??
			// ������ ���� üũ���� ����.
			size = Packet.EncryptPacket();

			send_temp = (CHAR*)&Packet;
			while(size > 0)
			{
				// 10004 : WSACancelBlockingCall�� ȣ���Ͽ� ���� �۾��� �ߴܵǾ����ϴ�. 
				// 10058 : �ش� ������ ����Ǿ����Ƿ� ������ �����ų� ���� �� �����ϴ�. 
				// 10038 : ������ ������ ������ ����ҷ��� �Ҷ� ����.
				if((rc = send(m_hSocket, &send_temp[idx], size, 0)) == SOCKET_ERROR)
				{
					// ��ŷ ������� 
					if (GetLastError() != WSAEWOULDBLOCK) // ��ŷ ������ �ƴ϶�� 
					{				
						//LOG_ERROR((L"SendThread : SOCKET_ERROR, %d", WSAGetLastError()));
						break;
					}
				}
				else
				{
					// ������ ���ٸ� 
					size -= rc;
					idx += rc;
				}
			}
			idx = 0;
			size = 0;
		}
		//////////////////////////////////////////////////////////////////////////////
	}

	VOID NFNetClient::OnReadPacketData()
	{
		//////////////////////////////////////////////////////////////////////////////
		// Recv
		int Ret = recv(m_hSocket, m_RecvBuffer, DEF_MAXPACKETSIZE, 0);	// ����Ÿ Receive
		if(Ret == 0)        // Graceful close
		{
			CloseAll();
			return;
		}
		else if (Ret == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK ) // ��ŷ ������ �ƴ϶�� 
		{
			//LOG_ERROR((L"[Recv] Packet Size Error : SOCKET_ERROR, %d", WSAGetLastError()));
			CloseAll();
			return;							
		}

		if(m_RecvIO.Append(m_RecvBuffer, Ret) == -1)
		{
			//LOG_ERROR((L"m_RecvIO.Append : Buffer Overflow..") );
		}

		// ���� ��Ŷ�� IOBuffer�� �־� ó���Ѵ�.
		if(m_RecvIO.GetPacket(&m_RecvPacket) == 1)
		{
			m_RecvPacket.DecryptPacket();

			if(m_RecvPacket.IsAliveChecksum())
			{
				// ���⼭ ����Ŷ ó�� ��ƾ ȣ��
				m_RecvQueue.Add(m_RecvPacket);

				// Message Type �϶� �̰ɷ� ������. 
				// ���� Update �̺�Ʈ ȣ���̸� �� ��ƾ�� �ּ�ó�� ���ش�.
				OnSocketEvent(WM_RECV_MSG, m_RecvPacket.m_Header.Size);
			}

			m_RecvPacket.Init();
		}
	}

	BOOL NFNetClient::NetworkEventHanlder(LONG lEvent)
	{
		BOOL bFlag = TRUE;

//		if(lEvent & FD_READ)
//		{
//			bFlag = OnReadPacketData();	
//		}
		if(lEvent & FD_CLOSE)
		{
			bFlag = FALSE;
		}
		if(lEvent & FD_CONNECT)
		{
			bFlag = OnConnect();
		}

		return bFlag;
	}

	BOOL NFNetClient::OnConnect()
	{
		//////////////////////////////////////////////////////////
		// IOCP Ȱ�� 
		//////////////////////////////////////////////////////////
		int ErrorCode = send(m_hSocket, (const char*)CONNECT_CHECK_DATA,CONNECT_CHECK_SIZE,0);
		if(ErrorCode == SOCKET_ERROR)
		{
			m_bConnect = FALSE;						// ���� ���� ���� OFF
		}
		else
		{
			m_bConnect = TRUE;						// ���� ���� ���� ON
		}
		OnSocketEvent(WM_CONNECT, m_bConnect);

		return m_bConnect;
	}

	int NFNetClient::GetQueCnt()
	{
		return m_RecvQueue.GetQueCnt();
	}

	int NFNetClient::GetPacket(NFPacket** Packet)
	{
		return m_RecvQueue.GetPnt(Packet);
	}

	void NFNetClient::PopPacket()
	{
		m_RecvQueue.Del();
	}

	BOOL NFNetClient::SendPost(NFPacket& Packet)
	{
		// ������ Send �ϱ�..
		return m_SendQueue.Add(Packet);
	}

	void NFNetClient::GetLocalIP(CHAR* LocalIP)
	{
		CHAR name[256];
		CHAR* TempIp;
		PHOSTENT hostinfo;

		WinSockInit();

		if( gethostname ( name, sizeof(name)) == 0)
		{
			if((hostinfo = gethostbyname(name)) != NULL)
			{
				TempIp = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
				strcpy(LocalIP, TempIp);
			}
		}
	}

}