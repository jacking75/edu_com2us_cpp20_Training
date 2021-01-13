/** 
 *  @file  		NFPacketPool.h
 *  @brief 		Packet Pool ������ü, �̰����� Buffer�� �Ҵ��ϰų� �����Ѵ�.
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#include "NaveServer.h"
#include "NFMemPool.h"

namespace NaveNetLib {

	//////////////////////////////////////////////////////////////////////////////////////////////
	// NFPacketPool Class
	//////////////////////////////////////////////////////////////////////////////////////////////
	class NFPacketPool  
	{
	//---------------------- Member Functions ----------------------//
	public:
		NFPacketPool();												// Contructor 
		virtual ~NFPacketPool();										// Destructor 

		bool Create(int nMaxUse			= DEF_MAXUSER,				// �ִ� ����
					char* lpszFileName	= NULL,						// �α� ���� �̸�
					int nMaxPacketSize	= DEF_MAXPACKETSIZE,
					int nAcptBuffCnt    = 1,
					int nRecvBuffCnt    = 8,
					int nSendBuffCnt    = 16);						// Create Packet Pool

		void Release();

		// Operations
		bool AllocAcptPacket(LPOVERLAPPEDPLUS &newolp);				// Accept ��Ŷ ���� 
		bool AllocRecvPacket(LPOVERLAPPEDPLUS &newolp);				// Receive ��Ŷ ���� 
		bool AllocSendPacket(LPOVERLAPPEDPLUS &newolp);				// Send Buffer ���� 
		bool AllocProcBuffer(char* &newbuf);						// Send Buffer ����

		bool FreeAcptPacket(LPOVERLAPPEDPLUS olp);					// Accept ��Ŷ ���� 
		bool FreeRecvPacket(LPOVERLAPPEDPLUS olp);					// Receive ��Ŷ ���� 
		bool FreeSendPacket(LPOVERLAPPEDPLUS olp);					// Send Buffer ����
		bool FreeProcBuffer(char* buffer);							// Send Buffer ����

	private:
		bool				LogOpen(char *lpszFileName);			// �α� ���� ���� 
		bool				LogWrite(char *lpszFmt, ...);			// �α� ���� ����� 
		void				LogClose();								// �α� ���� �ݱ� 

	//---------------------- Member Variables ----------------------//
	private:
		FILE*				m_hLogFile;								// �α� ������ ����� ���� ���� �ڵ� ��ü 

		//TODO: ������ ��ü Ǯ�� ��ü�Ѵ�.
		NFMemPool			<OVERLAPPEDPLUS>* m_pAcptPool;			// Accept Packet
		NFMemPool			<OVERLAPPEDPLUS>* m_pRecvPool;			// Receive Packet
		NFMemPool			<OVERLAPPEDPLUS>* m_pProcPool;			// Process Packet

		int					m_nMaxPacketSize;						// ��Ŷ ������ ���Ѱ�
	};

}