
//#include "NFMemPool.h"
#include "NFPacketPool.h"
//#include <Nave/NFLog.h>

namespace NaveNetLib {

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ������					 					//
	// [2]PARAMETER :												//
	// [3]RETURN :													//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	NFPacketPool::NFPacketPool()
	{
		m_hLogFile = NULL;											// ���� �ڵ� �ʱ�ȭ 
		m_pAcptPool = nullptr;
		m_pRecvPool = nullptr;
		m_pProcPool = nullptr;											// ���� �ʱ�ȭ 

		m_nMaxPacketSize = 0;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : �Ҹ���  - �޸� �Ҵ� ���� 					//
	// [2]PARAMETER :												//
	// [3]RETURN :													//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	NFPacketPool::~NFPacketPool()
	{
		Release();
	}

	void NFPacketPool::Release()
	{
		LogClose();													// �α� ���� �ڵ� �ݱ� 

		m_nMaxPacketSize = 0;

		if (m_pAcptPool) {
			delete m_pAcptPool;
		}

		if (m_pRecvPool) {
			delete m_pRecvPool;
		}

		if (m_pProcPool) {
			delete m_pProcPool;
		}

		m_pAcptPool = nullptr;
		m_pRecvPool = nullptr;
		m_pProcPool = nullptr;											// ���� �ʱ�ȭ 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : �ʱ�ȭ �Լ�									//
	// [2]PARAMETER : lpszFileName - ���� �̸�, ������ NULL			//
	// [3]RETURN :	true - ����ó�� false - ���� 					//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::Create(int nMaxUser, char* lpszFileName, int nMaxPacketSize, int nAcptBuffCnt, int nRecvBuffCnt, int nSendBuffCnt)
	{
		// ������(�̸�)�� ���� �ϸ�..
		if (lpszFileName) {
			LogOpen(lpszFileName);					// ���� ���� 
		}

		m_nMaxPacketSize = nMaxPacketSize;

		// �� ��Ŷ�� �޸� �Ҵ� 
		// Winsock ��ü���� ���� �޸� ���� 64�� ���Ѵ� 
		// winsock���� ����ϴ� OVERLAPPED �����ϼ���.. 
		m_pAcptPool = new NFMemPool<OVERLAPPEDPLUS>(nMaxUser*nAcptBuffCnt, 8);
		m_pRecvPool = new NFMemPool<OVERLAPPEDPLUS>(nMaxUser*nRecvBuffCnt, m_nMaxPacketSize+64);
		m_pProcPool = new NFMemPool<OVERLAPPEDPLUS>(nMaxUser*nSendBuffCnt, m_nMaxPacketSize+64);

		// ���� ���� 
		//LogWrite("acpt=%08X, recv=%08X, proc=%08X", m_pAcptPool, m_pRecvPool, m_pProcPool);

		// �ϳ��� �޸� �Ҵ��� �ȵǾ��ٸ� 
		if (!m_pAcptPool || !m_pRecvPool || !m_pProcPool) {
			return false;											// false  
		}

		m_pAcptPool->SetLogTitle(L"ACPT");
		m_pRecvPool->SetLogTitle(L"RECV");
		m_pProcPool->SetLogTitle(L"SEND");

		return true;												// true  
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ���� �ڵ� ����								//
	// [2]PARAMETER : lpszFileName - ���� �̸�						//
	// [3]RETURN :	true - ����ó�� false - ���� 					//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::LogOpen( char *lpszFileName )
	{
		// �̸��� ������ �ʾҴٸ� 
		if (m_hLogFile || lpszFileName == NULL) {
			return false;		// false ���� 
		}

		auto error = fopen_s(&m_hLogFile, lpszFileName, "ab" );					// ó�� ���� ���� 

		// ���� �˻� 
		if (error != 0) {
			return false;
		}

		return true;												// true ���� - ���� ó�� 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ���Ͽ� �����Ͽ� ����						//
	// [2]PARAMETER : lpszFileName - ���� �̸� ���ϱ� 				//
	// [3]RETURN :	true - ����ó�� false - ���� 					//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::LogWrite( char *lpszFmt, ... )
	{
		if (NULL == m_hLogFile) {
			return false;
		}

		va_list		argptr;
		CHAR		szOutStr[1024];
		va_start(argptr, lpszFmt);
		vsprintf(szOutStr, lpszFmt, argptr);
		va_end(argptr);
		
		INT nBytesWritten = fprintf( m_hLogFile, "%s\r\n", szOutStr );// LOG ���� 
		fflush( m_hLogFile );

		return true;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : ���µ� ���� �ݱ� 							//
	// [2]PARAMETER : void											//
	// [3]RETURN :	void											//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	void NFPacketPool::LogClose()
	{
		// ���� �ڵ��� ���� �Ѵٸ�,
		if (m_hLogFile) {
			fclose(m_hLogFile);						// ���� �ڵ� �ݱ� 
		}

		m_hLogFile = NULL;											// ���� �ڵ� �ʱ�ȭ 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Accept Packet �Ҵ� 							//
	// [2]PARAMETER : newolp - ���� ���� 							//
	// [3]RETURN :	true - ����ó�� 								//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::AllocAcptPacket( LPOVERLAPPEDPLUS &newolp )
	{
		LPOVERLAPPEDPLUS olp = NULL;								// ����ü ��ü ���� 
		
		bool bRet = false;

		if (!m_pAcptPool) {
			return bRet;												// ��ȯ 
		}

		olp = m_pAcptPool->Alloc();									// Accept Pool���� �Ҵ� ���� 

		LogWrite( "AlocAcptPacket(%08X)", (UINT64)olp );				// logging

		// �Ҵ��� ����� �Ǿ��ٸ� 
		if(olp)
		{
			// �Ҵ� �޸� ó�� �κ� 
			newolp = olp;											// ���� ���� 
			newolp->wbuf.buf = (CHAR*)(olp+1);						// ���� ���� 
			bRet = true;											// ��ȯ�� - true�� �ٲ� 
		}

		return bRet;												// ��ȯ 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Receive Packet �Ҵ� 						//
	// [2]PARAMETER : newolp - ���� ���� 							//
	// [3]RETURN :	true - ����ó�� 								//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::AllocRecvPacket( LPOVERLAPPEDPLUS &newolp )
	{
		LPOVERLAPPEDPLUS olp = NULL;								// ����ü ��ü ���� 
		bool bRet = false;

		if (!m_pRecvPool) {
			return bRet;												// ��ȯ 
		}

		olp = m_pRecvPool->Alloc();									// Receive Pool���� �Ҵ� ����

		//LogWrite( "AlocRecvPacket(%08X)", (UINT64)olp );				// logging

		// �Ҵ��� ����� �Ǿ��ٸ� 
		if(olp)
		{
			// �Ҵ� �޸� ó�� �κ�
			newolp = olp;											// ���� ���� 
			newolp->wbuf.buf = (char*)(olp+1);						// ���� ���� 
			bRet   = true;											// ��ȯ�� - true�� �ٲ�
		}
		else {
			//LOG_ERROR((L"NFPacketPool::AllocRecvPacket Pool Alloc Faild.."));
		}

		return bRet;												// ��ȯ
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Process Packet �Ҵ� 						//
	// [2]PARAMETER : newolp - ���� ���� 							//
	// [3]RETURN :	true - ����ó�� 								//
	// [4]DATE : 2000�� 10�� 7��									//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::AllocSendPacket(LPOVERLAPPEDPLUS &newolp)
	{
		LPOVERLAPPEDPLUS olp = NULL;								// ����ü ��ü ���� 
		bool bRet = false;

		if (!m_pProcPool) {
			return bRet;												// ��ȯ 
		}

		olp = m_pProcPool->Alloc();									// Accept Pool���� �Ҵ� ���� 

		//LogWrite( "AlocAcptPacket(%08X)", (UINT64)olp );				// logging

		// �Ҵ��� ����� �Ǿ��ٸ� 
		if(olp)
		{
			// �Ҵ� �޸� ó�� �κ� 
			newolp = olp;											// ���� ���� 
			newolp->wbuf.buf = (char*)(olp+1);						// ���� ���� 
			bRet   = true;											// ��ȯ�� - true�� �ٲ� 
		}
		return bRet;												// ��ȯ 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Process Buffer �Ҵ� 						//
	// [2]PARAMETER : newolp - ���� ���� 							//
	// [3]RETURN :	true - ����ó�� 								//
	// [4]DATE : 2000�� 10�� 7��									//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::AllocProcBuffer(char* &newbuf)
	{
		LPOVERLAPPEDPLUS olp = NULL;								// ����ü ��ü ���� 
		bool bRet = false;

		if (!m_pProcPool) {
			return bRet;												// ��ȯ 
		}

		olp = m_pProcPool->Alloc();									// Accept Pool���� �Ҵ� ���� 

		LogWrite( "AllocProcBuffer(%08X)", (unsigned __int64)olp );				// logging

		// �Ҵ��� ����� �Ǿ��ٸ� 
		if(olp)
		{
			// �Ҵ� �޸� ó�� �κ� 
			newbuf = (char*)(olp+1);								// ���� ���� 
			bRet   = true;											// ��ȯ�� - true�� �ٲ� 
		}
		
		return bRet;
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Accept �Ҵ� ��Ŷ ����  						//
	// [2]PARAMETER : olp - ������ ��Ŷ 							//
	// [3]RETURN :	true - ���� ó�� 								//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::FreeAcptPacket( LPOVERLAPPEDPLUS olp )
	{
		LogWrite( "FreeAcptPacket(%08X)", (unsigned __int64)olp );				// Logging

		if (!m_pAcptPool) {
			return false;												// ��ȯ 
		}

		return m_pAcptPool->Free(olp);								// ��Ŷ ���� �� ��� ��ȯ 
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Receive �Ҵ� ��Ŷ ����  					//
	// [2]PARAMETER : olp - ������ ��Ŷ 							//
	// [3]RETURN :	true - ���� ó�� 								//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::FreeRecvPacket( LPOVERLAPPEDPLUS olp )
	{
		LogWrite( "FreeRecvPacket(%08X)", (unsigned __int64)olp );				// Logging

		if (!m_pRecvPool) {
			return false;												// ��ȯ 
		}

		return m_pRecvPool->Free(olp);							// ��Ŷ ���� �� ��� ��ȯ						
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Process �Ҵ� ��Ŷ ����  					//
	// [2]PARAMETER : olp - ������ ��Ŷ 							//
	// [3]RETURN :	true - ���� ó�� 								//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::FreeSendPacket(LPOVERLAPPEDPLUS olp)
	{
		LogWrite( "FreeProcPacket(%08X)", (unsigned __int64)olp );				// Logging

		if (!m_pProcPool) {
			return false;												// ��ȯ 
		}

		return m_pProcPool->Free(olp);								// ��Ŷ ���� �� ��� ��ȯ
	}

	//////////////////////////////////////////////////////////////////
	// [1]DESCRIPTION : Process �Ҵ� ����  ����  					//
	// [2]PARAMETER : olp - ������ ��Ŷ 							//
	// [3]RETURN :	true - ���� ó�� 								//
	// [4]DATE : 2000�� 9�� 4��										//
	//////////////////////////////////////////////////////////////////
	bool NFPacketPool::FreeProcBuffer(char* buffer)
	{
		LPOVERLAPPEDPLUS olp;
		olp = ((LPOVERLAPPEDPLUS)buffer) - 1;
		LogWrite( "FreeProcBuffer(%08X)", (unsigned __int64)olp );				// Logging

		if (!m_pProcPool) {
			return false;												// ��ȯ 
		}

		return m_pProcPool->Free(olp);								// ��Ŷ ���� �� ��� ��ȯ
	}

}