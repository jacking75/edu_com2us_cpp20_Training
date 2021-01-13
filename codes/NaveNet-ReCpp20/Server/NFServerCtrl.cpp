#include "NaveServer.h"
#include "NFServerCtrl.h"
#include "NFUpdateManager.h"
//#include <Nave/NFException.h>
//#include <Nave/NFLog.h>
//#include <Nave/NFSync.h>

namespace NaveNetLib {

	#ifndef _DELETE_ARRAY
	#define _DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
	#endif

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	NFServerCtrl::NFServerCtrl()
	{
		m_bServerRun	= false;							// ���� ��������
		m_bPause		= false;

		m_nMaxThreadNum	= (GetNumberOfProcess() * 2);								// CPU�� * 2���� Thread �� ���� 

		m_iPort			= 0;
		m_iMaxConn		= 50;

		//m_pWorkThread		= NULL;								// ���� ������ �ڵ� 
		//m_hProcThread	= 0;
		//m_hPacketThread  = 0;
		m_hIOCP			= NULL;								// IOCP �ڵ� 
		m_pUpdateManager = new NFUpdateManager();
	}

	NFServerCtrl::~NFServerCtrl()
	{
		m_bServerRun = false;
		
		if (m_pUpdateManager != nullptr)
		{
			delete m_pUpdateManager;
			m_pUpdateManager = nullptr;
		}
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : IOCP �ڵ� ����								//
	// [2]PARAMETER : void											//
	// [3]RETURN : HANDLE = ������ IOCP								//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	HANDLE NFServerCtrl::CreateIOCP()
	{
		auto handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		return handle;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Listen ���� ���� 							//
	// [2]PARAMETER : void											//
	// [3]RETURN : SOCKET = ������ LISTEN SOCKET					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	SOCKET NFServerCtrl::CreateListenSocket(INT nServerPort, CHAR cBindQue)
	{
		SOCKET Socket = NULL;								// a Socket Variable for using Listener
				
		// [1] Create Listen Socket
		Socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );

		if (Socket == INVALID_SOCKET) {
			return Socket;
		}

		//if (ConnectIOCPSocket(Socket) == NULL) { // ���� ���� ��
		//	return Socket;
		//}

		// [2] bind listen socket
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(nServerPort);
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(Socket, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
			return Socket;
		}

		// [3] listening for an concurrent incoming connections limited in 5
		if (listen(Socket, cBindQue) == SOCKET_ERROR) {
			return Socket;
		}

		LISTENER stLin;
		stLin.Init();

		stLin.s = Socket;
		stLin.nPort = nServerPort;
		stLin.cBindQue = cBindQue;

		m_vecListener.push_back(stLin);

		return Socket;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Listen Socket �� iocp ���Ͽ� ����			//
	// [2]PARAMETER : SOCKET sckListener							//
	// [3]RETURN : HANDLE = ����� IOCP								//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	HANDLE NFServerCtrl::ConnectIOCPSocket(SOCKET sckListener)
	{
		return CreateIoCompletionPort((HANDLE)sckListener,m_hIOCP,0,0);
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : SOCKET���� ���̺귯�� Ȱ��ȭ 				//
	// [2]PARAMETER : void											//
	// [3]RETURN : void							 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	bool NFServerCtrl::InitSocket()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : �ý����� CPU ���� ���� 						//
	// [2]PARAMETER : void											//
	// [3]RETURN : INT = ���� �ý����� CPU �� ��ȯ 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	INT NFServerCtrl::GetNumberOfProcess()
	{
		SYSTEM_INFO si;												// a System Info Structure Object
		GetSystemInfo( &si );										// Get the System Information
		return (INT)si.dwNumberOfProcessors;						// return the number of processors
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ���� ���� 			 						//
	// [2]PARAMETER : void											//
	// [3]RETURN : bool - �ǹ� ����				 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	bool NFServerCtrl::Stop()
	{
		//////////////////////////////////////////////////////////////////////
		//						Server Closing Process						//
		//////////////////////////////////////////////////////////////////////
		INT nCnt;
		// [01] Queue Suicide Packets into each IOCP Main Thread
		for( nCnt = 0; nCnt < m_nMaxThreadNum; nCnt++ )
		{
			if (PostQueuedCompletionStatus(m_hIOCP, 0, IOCP_SHUTDOWN, 0) == NULL) {
				return false;
			}
		}

		// [02] Wait for thread terminations
		//nCnt = WaitForMultipleObjects( m_nMaxThreadNum, m_pWorkThread, true, 15000 );

		//switch ( nCnt )
		//{
		//	case WAIT_TIMEOUT:
		//		//LOG_ERROR((L"Not All WorkThreads Died in Time."));
		//		break;
		//	case WAIT_FAILED:
		//		//LOG_ERROR((L"WAIT_FAILED, WaitForMultipleObjects()."));
		//		break;
		//	default:
		//		break;
		//}

		//// [03] Close Thread Handles
		//if( m_pWorkThread )
		//{
		//	for (nCnt = 0; nCnt < m_nMaxThreadNum; nCnt++) {
		//		CloseHandle(m_pWorkThread[nCnt]);
		//	}

		//	_DELETE_ARRAY(m_pWorkThread);
		//}
		for (auto& thread : m_MainThreadList) {
			if (thread.joinable()) {
				thread.join();
			}
		}

		// [04] Process Thread Stop and Close
		m_bServerRun = false;
		//CloseHandle(m_hProcThread);
		//CloseHandle(m_hPacketThread);
		if (m_ProcessThread.joinable()) {
			m_ProcessThread.join();
		}
		if (m_PacketThread.joinable()) {
			m_PacketThread.join();
		}

		if(ReleaseSession())
		{
			//LOG_IMPORTANT((L"shutdown session.."));
		}

		// [05] Close Listener Socket
		auto iListener = (int)m_vecListener.size();
		for(int i = 0; i < iListener; ++i)
		{
			m_vecListener[i].Release();
		}

		// [6] Close IOCP Handle
		if (m_hIOCP) {
			CloseHandle(m_hIOCP);
		}

		// [7] ���� ���̺귯�� ���� 
		WSACleanup();

		// [8] Show the Result of Close Processing
		//LOG_IMPORTANT((L"shutdown sequence finished.."));

		return true;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ���� ����			 						//
	// [2]PARAMETER : void											//
	// [3]RETURN : bool - �ǹ� ����				 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	bool NFServerCtrl::Start(INT nPort, INT nMaxConn, INT nMaxThreadNum)
	{
		m_iPort = nPort;
		m_iMaxConn = nMaxConn;

		// ���� ���Ǵ� ������� Work�� ������ CPU*2��
		// ���� ��Ŷ�̳� �̺�Ʈ���� ó���ϴ� Update�� ������ 1���� ����Ѵ�.
		// ���� ���ͳ� ��Ÿ �ٸ� ������ ���¸� ������Ʈ �ϱ����ؼ��� 
		// ���� �۾��� �����带 ������ ������ �ȴ�.
		/*
			�����ϴ°�
			IOCP�� ������ CPU������ŭ (IOCP�� ���� Accept, Recv, Send�� �ϱ� ���� ������)
			EventUpdate�� ������ 1��  (��Ŷ�� ������Ʈ �ϱ� ���� ť)
			������ ����(���͹ױ�Ÿ����)�� ������ ?�� 
		*/

		// 0�̾ƴϸ� �� ���� ���
		if (nMaxThreadNum != 0) {
			m_nMaxThreadNum = nMaxThreadNum;
		}
		else {
			m_nMaxThreadNum = (GetNumberOfProcess() * 2);			// CPU�� * 2���� Thread �� ���� 
		}

		// [00] ���� �� ��ü ���� 
		INT				nCnt = 0;									// ���� ����  
		//UINT			nDummy;										// ������ �� ó�� 
		SOCKET			skListener;

		// [01] initialize socket library
		if (!InitSocket()) {
			goto Error_Procss;
		}

		// [02] Create IOCP 
		if ((m_hIOCP = CreateIOCP()) == NULL) {
			goto Error_Procss;
		}

		// [03] Create Listener Socket
		if ((skListener = CreateListenSocket(m_iPort)) == INVALID_SOCKET) {
			goto Error_Procss;
		}

		// [04] Connect listener socket to IOCP
		if (ConnectIOCPSocket(skListener) == NULL) { // ���� �ּ� �޾Ҵ�
			goto Error_Procss;
		}

		// [05] ȭ�鿡 ���� ���� ǥ�� 
		ShowServerInfo();

		// [06] Create Session..  // ���� �ּ� �޾Ҵ�
		if (CreateSession(skListener) == false) {
			goto Error_Procss;
		}

		// [07] Update Session..
		//if ((m_pWorkThread = new HANDLE[m_nMaxThreadNum]) == NULL) {
		//	goto Error_Procss;	// Create thread Control Handle
		//}
		//for(nCnt = 0; nCnt < m_nMaxThreadNum; nCnt++)								// Run Thread 
		//{
		//	if ((m_pWorkThread[nCnt] = (HANDLE)_beginthreadex(0, 0, Thread_MainEx, m_hIOCP, 0, &nDummy)) == NULL) {
		//		goto Error_Procss;
		//	}
		//}
		for (nCnt = 0; nCnt < m_nMaxThreadNum; nCnt++)								// Run Thread 
		{
			m_MainThreadList.emplace_back([this]() { Thread_MainEx();  });
		}

		//--------------> Server Initializing has been done <---------------//
		// [8] Process Thread �����ϱ�
		m_bServerRun = true;
		/*if ((m_hProcThread = (HANDLE)_beginthreadex(0, 0, Process_MainEx, this, 0, &nDummy)) == NULL) {
			goto Error_Procss;
		}*/
		m_ProcessThread = std::thread([this]() { Process_MainEx();  });

		// [9] Process Thread �����ϱ�
		/*if ((m_hPacketThread = (HANDLE)_beginthreadex(0, 0, Packet_MainEx, this, 0, &nDummy)) == NULL) {
			goto Error_Procss;
		}*/
		m_PacketThread = std::thread([this]() { Packet_MainEx();  });

		return true;

	Error_Procss:
		Stop();
		m_bServerRun = false;
		return false;
	}

	void NFServerCtrl::ShowServerInfo()
	{
		//wchar_t	szDate[32], szTime[32];

		//_tzset();
		//_wstrdate( szDate );
		//_wstrtime( szTime );
		//LOG_IMPORTANT((L"------------------------------------------------"));
		//LOG_IMPORTANT((L" %s initialized at %s, %s", L"Server", szDate, szTime) );
		//LOG_IMPORTANT((L"------------------------------------------------"));

		//////////////////////////////////////////////////////////////////////
		//							Server start							//
		//////////////////////////////////////////////////////////////////////
		//LOG_IMPORTANT((L"------------------------------------------------"));
		//LOG_IMPORTANT((L"|                 SERVER START                 |"));
		//LOG_IMPORTANT((L"------------------------------------------------"));

		std::string Ip;
		GetLocalIP(Ip);
		//LOG_IMPORTANT((L"IP(%s) Port(%d)", Ip, m_iPort ));
	}

	void NFServerCtrl::Pause(bool bPause)
	{
		m_bPause = bPause;
	}

	void NFServerCtrl::Update()
	{
/*		if(timeGetTime() - uRecvTickCnt > 1000)
		{
			InterlockedExchange((LONG*)&iMaxRecvPacket,iRecvPacket);
			InterlockedExchange((LONG*)&iRecvPacket,0);
			InterlockedExchange((LONG*)&uRecvTickCnt,timeGetTime());
		}
*/
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : IOCPó���� ����ϴ� ���� ������ 			//
	// [2]PARAMETER : lpvoid - IOCP Handle							//
	// [3]RETURN : bool - �ǹ� ����				 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	/*unsigned NFServerCtrl::Thread_MainEx(LPVOID lpvoid)
	{
		DWORD			dwIoSize;									// �Ϸ�ó�� ������ ���� 
		PULONG_PTR		lCompletionKey;								// ���� ���� Ż�� ���� �� 
		bool			bSuccess;									// ��ŷ ó�� ���� Ȯ��  
		HANDLE			hIOCP = (HANDLE)lpvoid;					// IOCP �ڵ� ���� 
		
		LPOVERLAPPED	lpOverlapped;								// ��ø Ȯ�� ������ 

		////////////
		// ���� ���� 
		while( true )
		{
			// IOCP ó���� ��ٸ��� BLOCKING MODE
			bSuccess = GetQueuedCompletionStatus(hIOCP,										// IOCP Handle
												&dwIoSize,									// ó�� ������ 
												lCompletionKey,							// �Ϸ� Ű 
												(LPOVERLAPPED*) &lpOverlapped,				// ��ø Ȯ�� 
												INFINITE);									// Waiting Time 

			LPOVERLAPPEDPLUS lpOverlapPlus = (LPOVERLAPPEDPLUS)lpOverlapped;

			if(bSuccess)
			{
				// ���� ��ȣ�� ���Դٸ�, ���� Ż�� 
				if (*lCompletionKey == IOCP_SHUTDOWN) {
					break;
				}

				if( NULL != lpOverlapPlus )
				{
					///////////////////////////////////////////////
					// ó���� ���������� �̷�����ٸ� ��������...
					lpOverlapPlus->dwBytes = dwIoSize;				// ó�� ����Ÿ Size
					// ó�� ���� Cast ��ȯ 
					// �ӵ��� ���� ���������ӿ��� try~catch�� ����.
					// �˼� ���� ������ �˻��ϱ� ���ؼ� Exception::EnableUnhandledExceptioinFilter(true)�� ����Ѵ�

					// Recv ī��Ʈ ó��
					//if(lpOverlapPlus->nConnState == ClientIoRead)
					//{
					//	InterlockedExchange((LONG*)&iRecvPacket,iRecvPacket+1);
					//}

					NFConnection* lpClientConn = (NFConnection*) lpOverlapPlus->pClientConn;
					lpClientConn->DoIo(lpOverlapPlus);				// IOCP ó�� �ڵ鸵 
				}
			}
			else
			{
				if(!lpOverlapPlus)
				{
					//LOG_ERROR((L"Critical Error on GetQueuedCompletionStatus()."));
				}
				else
				{
					// ó�� ���� Cast ��ȯ
					NFConnection* lpClientConn = (NFConnection*) lpOverlapPlus->pClientConn;
					// ������ Clear���ָ� �ȵȴ�. (����Ÿ�� ����� �ʱ�ȭ ���� �������� �ִ�)
//					lpClientConn->Clear();
//					LOG_ERROR(("[%04d] IOCP OverlapPlus Error, Close_Open()ȣ��. SOCKET_ERROR, %d", lpClientConn->GetIndex(), WSAGetLastError()));
					lpClientConn->SetClose_Open(lpOverlapPlus, true);	// ���� ����
				}
			}
		}

		return 0;
	}*/
	void NFServerCtrl::Thread_MainEx()
	{
		DWORD			dwIoSize = 0;									// �Ϸ�ó�� ������ ���� 
		ULONG_PTR		lCompletionKey = 0;					// ���� ���� Ż�� ���� �� 
		bool			bSuccess;									// ��ŷ ó�� ���� Ȯ��  
		HANDLE			hIOCP = m_hIOCP;					// IOCP �ڵ� ���� 
		LPOVERLAPPED	lpOverlapped = NULL;								// ��ø Ȯ�� ������ 

																	////////////
																	// ���� ���� 
		while (true)
		{
			// IOCP ó���� ��ٸ��� BLOCKING MODE
			bSuccess = GetQueuedCompletionStatus(hIOCP,										// IOCP Handle
												&dwIoSize,									// ó�� ������ 
				(PULONG_PTR)&lCompletionKey,							// �Ϸ� Ű 
												(LPOVERLAPPED*)&lpOverlapped,				// ��ø Ȯ�� 
												INFINITE);									// Waiting Time 

			LPOVERLAPPEDPLUS lpOverlapPlus = (LPOVERLAPPEDPLUS)lpOverlapped;

			if (bSuccess)
			{
				// ���� ��ȣ�� ���Դٸ�, ���� Ż�� 
				if (lCompletionKey == IOCP_SHUTDOWN) {
					break;
				}
				
				if (NULL != lpOverlapPlus)
				{
					///////////////////////////////////////////////
					// ó���� ���������� �̷�����ٸ� ��������...
					lpOverlapPlus->dwBytes = dwIoSize;				// ó�� ����Ÿ Size
																	// ó�� ���� Cast ��ȯ 
																	// �ӵ��� ���� ���������ӿ��� try~catch�� ����.
																	// �˼� ���� ������ �˻��ϱ� ���ؼ� Exception::EnableUnhandledExceptioinFilter(true)�� ����Ѵ�
																	/*
																	// Recv ī��Ʈ ó��
																	if(lpOverlapPlus->nConnState == ClientIoRead)
																	{
																	InterlockedExchange((LONG*)&iRecvPacket,iRecvPacket+1);
																	}
																	*/
					NFConnection* lpClientConn = (NFConnection*)lpOverlapPlus->pClientConn;
					lpClientConn->DoIo(lpOverlapPlus, m_pUpdateManager);				// IOCP ó�� �ڵ鸵 
				}
			}
			else
			{
				char errorMsg[1024] = { 0, };
				WSAGetLastErrorToTextMessage(errorMsg);

				if (!lpOverlapPlus)
				{
					//LOG_ERROR((L"Critical Error on GetQueuedCompletionStatus()."));
				}
				else
				{
					// ó�� ���� Cast ��ȯ
					NFConnection* lpClientConn = (NFConnection*)lpOverlapPlus->pClientConn;
					// ������ Clear���ָ� �ȵȴ�. (����Ÿ�� ����� �ʱ�ȭ ���� �������� �ִ�)
					//					lpClientConn->Clear();
					//					LOG_ERROR(("[%04d] IOCP OverlapPlus Error, Close_Open()ȣ��. SOCKET_ERROR, %d", lpClientConn->GetIndex(), WSAGetLastError()));
					lpClientConn->SetClose_Open(lpOverlapPlus, m_pUpdateManager, true);	// ���� ����
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : IOCPó���� ����ϴ� ���� ������ 			//
	// [2]PARAMETER : lpvoid - IOCP Handle							//
	// [3]RETURN : bool - �ǹ� ����				 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	/*unsigned NFServerCtrl::Process_MainEx(LPVOID lpvoid)
	{
		NFServerCtrl*	pCtrl = (NFServerCtrl*)lpvoid;					// IOCP �ڵ� ���� 

		assert(pCtrl);

		while( pCtrl->IsRun())
		{
			if(pCtrl->IsPause())
			{
				Sleep(1);
				continue;
			}

			pCtrl->Update();
			Sleep(1);
		}

		return 0;
	}*/
	void NFServerCtrl::Process_MainEx()
	{
		while (IsRun())
		{
			if (IsPause())
			{
				Sleep(1);
				continue;
			}

			Update();
			Sleep(1);
		}
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : UpateQueó���� ����ϴ� ���� ������ 		//
	// [2]PARAMETER : lpvoid - IOCP Handle							//
	// [3]RETURN : bool - �ǹ� ����				 					//
	// [4]DATE : 2003�� 10�� 24��									//
	//////////////////////////////////////////////////////////////////
	/*unsigned NFServerCtrl::Packet_MainEx(LPVOID lpvoid)
	{
		NFServerCtrl*	pCtrl = (NFServerCtrl*)lpvoid;					// IOCP �ڵ� ���� 
		//NFUpdateManager* pUpdateManager = NFUpdateManager::GetInstancePtr();

		assert(pCtrl);
		assert(pCtrl->m_pUpdateManager);

		while( pCtrl->IsRun())
		{
			pCtrl->m_pUpdateManager->Update();
			Sleep(1);
		}

		return 0;
	}*/
	void NFServerCtrl::Packet_MainEx()
	{
		while (IsRun())
		{
			m_pUpdateManager->Update();
			Sleep(1);
		}
	}
	
	void NFServerCtrl::WSAGetLastErrorToTextMessage(char *pMsg)
	{
		LPVOID lpMsgBuf;
		auto errorCode = WSAGetLastError();

		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&lpMsgBuf, 0, NULL);
		
		//printf("[%s] %s", pMsg, (char *)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
}