#include "NaveServer.h"
#include "NFConnection.h"
#include "NFServerLink.h"
#include "NFUpdateManager.h"
//#include <Nave/NFLog.h>

// 1.Server -> 2.Server �� �����ϱ� ���� 1.Server���� ����ϴ� Connect�� Ŭ�����̴�.
// �̷��� �ϸ� 1.Server���� 2.Server�� �����Ҷ� IOCP�� ����� Send,Recv���� �Ѵ�.

namespace NaveNetLib {
		
	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ��� ���� ���� �� ��ü �ʱ�ȭ				//
	// [2]PARAMETER : dwIndex - ��ü ���� ID						//
	//				  hIOCP - IOCP Handle							//
	//				  listener - Listen Socket						//
	//				  pPacketPool - ��Ŷ Pool ������				//
	//				  pMsg - BroadCasting�� ���� �޼��� ó�� ������ //
	//				  MsgWait - �޼��� Send�� ���� �̺�Ʈ �ڵ�		// 
	// [3]RETURN : true - ���� ó��, false - ���� 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	bool NFServerLink::Create(DWORD			dwIndex,
							HANDLE			hIOCP,
							NFPacketPool*	pPacketPool,
							wchar_t*			sIP,
							int				nPort,
							int				nMaxBuf)
	{	
		wcscpy_s(m_strIP, 32, sIP);
		m_iPort = nPort;

		if(NFConnection::Create(dwIndex, hIOCP, NULL, pPacketPool, nMaxBuf) == false)
		{
			OnConnect(false);
			
			// �����ϸ� ��� �ؾ� �ϴ°�? �� ���ؼ� Ŭ������ ���� Ŭ������ �ȴ�.
			return false;
		}

		OnConnect(true);
		return true;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ��ü �ʱ�ȭ, ���� ���� ����					//
	// [2]PARAMETER : void											//
	// [3]RETURN : true - ����ó�� , false - ���� 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	bool NFServerLink::Open()
	{
		// MRS�� Open�� 1->2 ������ Connect �Ҷ� ���ȴ�.
		// ���� �� �־�� ��.
		
		//TODO: �����. �Ʒ� �� ������ �����ؼ� ��Ȳ�� �°� ó���ϱ�
		assert(m_nMaxBuf);
		assert(m_nMaxPacketSize);

		// ��Ŷ Pool�� ����� ���� �Ǿ����� �˻� 
		if (m_pPacketPool == nullptr) {
			return false;
		}

		m_RecvIO.InitIOBuf();	// ��Ŷ���۸� �ʱ�ȭ�Ѵ�.

 		// create socket for send/recv
		m_Socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP,NULL,0,WSA_FLAG_OVERLAPPED );

		// ���� ������ ����� ���� �Ǿ����� �˻� 
		if (m_Socket == NULL) {
			return false;
		}

		// ���� ���� ���� ���� �� �ʱ�ȭ 
		SOCKADDR_IN		saServer;		
		memset(&saServer,0,sizeof(saServer));

		// �����ڵ尡 �⺻ ���α׷��� �̱� ������ �ڵ����� ��Ƽ����Ʈ�� �����Ѵ�. 
		// WideCharToMultiByte �Լ��� ����� ������ ���к��ϰ� Nave ���̺귯���� ��ũ���� �ʱ� ���ؼ�
		// �� ������ ���ؼ� Nave�� StringUtil�� ����ϴ°� ���ɳ���
		char strIP[16];
		int Ret = (int)WideCharToMultiByte(CP_ACP, 0, m_strIP, (int)wcslen(m_strIP), strIP, 15, NULL, NULL);
		strIP[Ret] = 0;

		// ���� Ÿ��, IP, PORT ���� 
		saServer.sin_family = AF_INET;
		//saServer.sin_addr.s_addr = inet_addr(strIP);
		//saServer.sin_port = htons(m_iPort);
		auto ret = inet_pton(AF_INET, strIP, (void *)&saServer.sin_addr.s_addr);
		saServer.sin_port = htons(m_iPort);

		// ������ Connect
		INT nRet = connect(m_Socket,(sockaddr*)&saServer, sizeof(saServer));

		// ���� �����̰ų� ��ŷ�� �Ǿ��ٸ� 
		if (nRet == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK)
		{
			//LOG_ERROR((L"Socket Connect Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}

		//////////////////////////////
		// �޼��� ����� ������ IOCP���� ����� 
		send(m_Socket,CONNECT_CHECK_DATA,CONNECT_CHECK_SIZE,MSG_DONTROUTE);

		// IOCP �� ���� 
		if(CreateIoCompletionPort((HANDLE)m_Socket,m_hIOCP,0,0) == 0)
		{
			//LOG_ERROR((L"IOCP Connect Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}
		
		// ���� Ȯ�� ���� ON !!!!!
		InterlockedIncrement((LONG*)&m_bIsConnect);

		// RECEIVE ���·� ��ü ��ȯ 
		if(!RecvPost())
		{
			//LOG_ERROR((L"RecvPost Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}
		
		/////////////////////////////////
		// ������ ���� ����ȭ�� ���� ���� 
		INT zero = 0;
		INT err = 0;

		// Send Buffer�� ���� ����
		if( (err = setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (CHAR *)&zero, sizeof(zero))) == SOCKET_ERROR)
		{
			//LOG_ERROR((L"Send Buffer setsockopt Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}

		// Receive Buffer�� ���� ���� 
		if((err = setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (CHAR *)&zero, sizeof(zero) )) == SOCKET_ERROR)
		{
			//LOG_ERROR((L"Receive Buffer setsockopt Faild : SOCKET_ERROR, %d", WSAGetLastError()));
			closesocket(m_Socket);
			m_Socket = NULL;
			return false;
		}

		// Server�� ������ ���������� ó������ ���� ��Ŷ�̴�.
		return SendOpenPacket();
	}

	bool NFServerLink::Close_Open( bool bForce )
	{
		// ���ϰ� ������ ���� Ȯ�� 
		if(m_Socket)
		{
			ShowMessage(CLOSE_SOCKET);
			
			struct linger li = {0, 0};	// Default: SO_DONTLINGER

			// ���Ͽ� ó�� ���� ���� ����Ÿ�� ���� ���ۿ� �����ִٸ�,
			if (bForce) {
				li.l_onoff = 1; // SO_LINGER, timeout = 0
			}

			shutdown(m_Socket, SD_BOTH );						// ����? �̰� ����? ^^;; �㿡 ã�ƺ��� 
																// 2001�� 9�� 6�� 

			// �ܿ� ����Ÿ�� ������ ����, ������.. ���? ^^;; �� �׷���..
			setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
			closesocket(m_Socket);								// ���� �ݱ� 
			m_Socket = NULL;
		}

		// ���� �ʱ�ȭ 
		InterlockedExchange((LONG*)&m_bIsConnect,0);
		m_uRecvTickCnt = 0;

		OnDisconnect();

		Clear();
		return true;												// ���� ó�� 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : �޼��� ���� �� Receive ��ȣ �߻�, Socket����//
	// [2]PARAMETER : LPOVERLAPPEDPLUS - ������ overlappedplus����ü//
	// [3]RETURN : true - ���� ó��									//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	bool NFServerLink::DispatchPacket(LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager)
	{
		CHAR*  psrcbuf     =    &( lpOverlapPlus->wbuf.buf[0] );
		INT     srclen     =    lpOverlapPlus->dwBytes;

		// Packet���� ��ü�� [H 2byte][P size] �����̴�.
		m_RecvIO.Append(psrcbuf, srclen);

		m_RecvIO.Lock();

		// IOCP�� ������ ��Ŷó���� ���� ��������� �� �ɷ��̴�
		// �׷��� �Ʒ��� ���� UpdateManaget�� ��Ŷ�� ������ �� �����忡�� Update�� ó���ϸ�
		// ��������� ������ ������ ������ ����ó���� ���� ��������� ������ �ʴ´�.
		// ��Ŷ�� ������ ���¿��� �ٷ� ó���ϰ� �����ϰ� UpdateManager�� Ŀ��Ʈ �÷�����
		// ������Ʈ �ϴ°ɷ� �����غ���. 
#ifdef ENABLE_UPDATEQUE
		if(m_RecvIO.GetPacket(&m_RecvPacket) == 1)
		{
			if (m_RecvPacket.IsAliveChecksum()) {
				pUpdateManager->Add(this, &m_RecvPacket);
			}

			m_RecvPacket.Init();
		}
		else
		{
			m_RecvIO.UnLock();
		}
#else
		// �Ʒ��� ���� �����忡�� ��Ŷ�� ó���ϰ� �Ǹ� 
		// Dispatch�� ũ��Ƽ�� ������ ����־� ������� �����ؾ��Ѵ�.
		if(m_RecvIO.GetPacket(&m_RecvPacket) == 1)
		{
			DispatchPacket(m_RecvPacket);

			m_RecvPacket.Init();
		}
		else
			m_RecvIO.UnLock();
#endif

		// �׸��� ���ο� recieve buffer�� �غ��Ͽ� Post�Ѵ�.
		return RecvPost();
	}

}