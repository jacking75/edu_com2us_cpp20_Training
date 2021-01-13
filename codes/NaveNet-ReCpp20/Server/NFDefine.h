/** 
 *  @file  		NFDefine.h
 *  @brief 		Packet�� ���õ� �⺻ Define �� ��Ʈ���� ����
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

namespace NaveNetLib {

	// ��Ŷ ó���� UpdateQue�� �̿��� ó���Ұ������� ���� Define
	// �̰� Ȱ��ȭ ��Ű�� ��Ŷ�� ó���Ҷ� Receive�����尡 �ƴ� 
	// Process�����忡�� ��Ŷ�� ������Ʈ �Ѵ�.

	// UPDATEQUE ����� Work�����忡�� Packet�� �ϼ��Ǹ� PacketQue�� �̿��� 
	// ��Ŷ�� ó���Ѵ�. (MMO�ÿ� �ʿ�)
	#define ENABLE_UPDATEQUE


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [1] User Define
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	#define		DEF_MAXUSER				50	 						// Maximum connections for Test

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [2] Server Declarations
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	#define		IOCP_SHUTDOWN			((DWORD) -1L)				// Closing Server Message
	#define		DEF_SERVER_PORT			25000						// PORT Number ( Client�� ���� �ؾ��� )

	enum ESHOW_MSGTYPE
	{
		ACCEPT_FAILED,
		CLOSE_SOCKET,
		DEAD_CONNECTION,
		CONNECT_SUCCESS,
		CONNECT_FAILED,
		DISPATCH_FAILED,
		DOIOSWITCH_FAILED,
		// accept
		ALLOCACPT_FAILED,
		PREPAREACPT_FAILED,
		
		// send, recv
		PREPARERECVSIZE_FAILED,
		PREPARESENDSIZE_FAILED,
		
		ALLOCRECV_FAILED,
		ALLOCSEND_FAILED,
		
		PREPARERECV_FAILED,
		PREPARESEND_FAILED,

		RELEASEACPT_FAILED,
		RELEASERECV_FAILED,
		RELEASESEND_FAILED,

		BINDIOCP_FAILED,

		RECVPOST_FAILED,
		RECVPOSTPENDING_FAILED,

//		ALLOCPROC_FAILED,
		SENDPOST_FAILED,
		SENDPOSTPENDING_FAILED,
	};

	enum CONNECT_EVENT
	{
		CONNECT_NONE,
		CONNECT_true,
		CONNECT_false,
		DISCONNECT_TURE,
		CLOSEOPEN_true,
	};

	/// ������ ���� : Ŭ���̾�Ʈ �۵� ���¸� ���� �Ѵ� 
	typedef enum CONN_STATUS 
	{
		ClientIoUnknown,											/// Raw status
		ClientIoAccept,												/// accept status 
		ClientIoRead,												/// read status  
		ClientIoWrite,												/// write status
	}*pCONN_STATUS;

	/// Ȯ�� ������ ����ü : IOCPó���� ��� 
	typedef struct OVERLAPPEDPLUS {
		OVERLAPPED		overlapped;									/// OVERLAPPED struct
		SOCKET			sckListen;									/// listen socket handle
		SOCKET			sckClient;									/// send/recv socket handle
		CONN_STATUS		nConnState;									/// operation flag
		WSABUF			wbuf;										/// WSA buffer						
		DWORD			dwRemain;									/// 
		DWORD			dwBytes;									/// Processing Data Size
		DWORD			dwFlags;									/// 
		void*			pClientConn;								/// Processing Client 
	}*LPOVERLAPPEDPLUS;

}