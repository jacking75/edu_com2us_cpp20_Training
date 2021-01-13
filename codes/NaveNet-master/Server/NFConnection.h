/** 
 *  @file  		NFConnection.h
 *  @brief 		�ϳ��� Connection�� �����ϴ� ��ü�̴�.
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#include <mutex>

#include "NaveServer.h"
#include "NFMemPool.h"
//#include "NFPacketPool.h"
#include "NFIOBuffer.h"


namespace NaveNetLib {

	class NFPacketPool;
	class NFUpdateManager;

	/** 
	 *  @class        NFConnection
	 *  @brief        �ϳ��� ���ϰ�ü�� ��Ÿ���� 1���� Ŭ���̾�Ʈ�� ��Ÿ����.
	 *  @remarks      
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-08-28
	 */
	class NFConnection  
	{
	public:
		NFConnection();
		virtual ~NFConnection();

	public:
		// �� ��ü ���� 
		/**
		 * @brief	��� ���� ���� �� ��ü �ʱ�ȭ
		 * @param dwIndex		��ü ���� ID
		 * @param hIOCP			IOCP Handle
		 * @param listener		Listen Socket
		 * @param pPacketPool	��Ŷ Pool ������
		 * @param nMaxBuf		�ִ� ����ũ��
		 * @return ��������
		 */
		virtual bool		Create( DWORD			dwIndex,
									HANDLE			hIOCP,
									SOCKET			listener,
			NFPacketPool*	pPacketPool,
									INT				nMaxBuf);

		/**
		 * @brief	��Ŷ�� ������Ʈó�� �մϴ�. 
		 * @param Packet ������Ʈ �� ��Ŷ����
		 */
		virtual void		UpdatePacket(NFPacket& Packet);

		/**
		 * @brief	���� ���� ���� 
		 * @param bForce true�� ���Ͽ� ���� ������ ����
		 */
		void				Disconnect( bool bForce = false );		

		/**
		 * @brief	���� �������� �����մϴ�.
		 */
		virtual void		Clear() { return; };

		/**
		 * @brief	��ü�� Ŭ���̾�Ʈ ���� ���� �� �ʱ�ȭ
		 * @param bForce true�� ���Ͽ� ���� ������ ����
		 * @return  ��������
		 */
		virtual bool		Close_Open( bool bForce = false );

		/**
		 * @brief	IOCP ó�� �ڵ鸵
		 * @param lpOverlapPlus	ó���� ��Ŷ
		 * @return 
		 */
		virtual bool		DoIo( LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager);

		/**
		 * @brief	��Ŷ ����
		 * @param pPackte ���� ��Ŷ ������
		 * @param Len ���� ��Ŷ ũ��
		 * @return  ��������
		 */
		virtual bool		SendPost( CHAR* pPackte, INT Len );	

		/**
		 * @brief	��Ŷ ����
		 * @param Packet ���� ��Ŷ ����ü
		 * @return  ��������
		 */
		virtual bool		SendPost( NFPacket&	Packet);

		/**
		 * @brief	�������� IP��� 
		 * @param iIP ������ 4�� �迭
		 * @return 
		 */
		bool				GetClientIP( INT* iIP );					 
		/**
		 * @brief	�������� IP��� 
		 * @param szIP ������
		 * @return 
		 */
		bool				GetClientIP( CHAR* szIP );				
		/**
		 * @brief	�������� IP��� 
		 * @param szIP �����ڵ� ������
		 * @return 
		 */
		bool				GetClientIP( wchar_t* szIP );				
		/**
		 * @brief	�������� IP��� 
		 * @param addr sockaddr_in ����
		 * @return 
		 */
		bool				GetClientIP( sockaddr_in& addr );			

		/**
		 * @brief	���ᰳü�� ���� �ε����� �����մϴ�.
		 * @param iIndex ���ᰳü�� ���� �ε���
		 */
		inline void			SetIndex(INT iIndex) { m_dwIndex = iIndex; };

		/**
		 * @brief	���ᰳü�� ���� �ε��� ���
		 * @return  �ε�����
		 */
		inline DWORD		GetIndex() { return m_dwIndex; }

		/**
		 * @brief	���� Tick ���
		 * @return  ���� Tick ī��Ʈ
		 */
		LONG				GetRecvTickCnt();

		/**
		 * @brief	���� ��ü�� Ȱ��ȭ ���� ���
		 * @return  Ȱ��ȭ ����
		 */
		bool				IsConnect();

		/**
		 * @brief	���� ��ü�� ���¸� �����մϴ�.
		 * @param eState	���� ��ü�� ���� ���� 
		 */
		void				SetConnectFlag(CONNECT_EVENT eState, NFUpdateManager* pUpdateManager);

		/**
		 * @brief	���޵� Overlapped��ü�� �̿��� ���� ��ü�� �����Ų�� �ʱ�ȭ ��ŵ�ϴ�.
		 * @param lpOverlapPlus Overlapped ��ü
		 * @param bForce   true�� ���Ͽ� ���� ������ ����
		 */
		void				SetClose_Open(LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager, bool bForce=false );

		//------------------------ �� �� �� �� -------------------------//
	protected:

		/**
		 * @brief	��ü �ʱ�ȭ, ���� ���� ����					
		 * @return  ��������
		 */
		virtual bool		Open();

		/**
		 * @brief	Socket�� IOCP ���ε� �۾�
		 * @param lpOverlapPlus ���ε� ��ų Overlapped ����ü
		 * @return 
		 */
		bool				BindIOCP( LPOVERLAPPEDPLUS lpOverlapPlus );

		/**
		 * @brief	Accept��Ŷ �Ҵ�
		 * @return  �Ҵ�� Overlapped ����ü 
		 */
		LPOVERLAPPEDPLUS	PrepareAcptPacket();

		/**
		 * @brief	IOCP�� �̿��� Recv Overlapped ����
		 * @param buflen ���� ũ��
		 * @return  �Ҵ�� Overlapped ����ü 
		 */
		LPOVERLAPPEDPLUS	PrepareRecvPacket(UINT buflen);

		/**
		 * @brief	IOCP�� �̿��� Send Overlapped ����
		 * @param *psrcbuf ���� ������ ������
		 * @param srclen   ���� ũ��
		 * @return  �Ҵ�� Overlapped ����ü 
		 */
		LPOVERLAPPEDPLUS	PrepareSendPacket(CHAR *psrcbuf, UINT srclen);

		/**
		 * @brief	Accept ��Ŷ ����  							
		 * @param lpOverlapPlus ������ Overlapped����ü
		 * @return  ��������
		 */
		bool				ReleaseAcptPacket(LPOVERLAPPEDPLUS lpOverlapPlus);

		/**
		 * @brief	Receive ��Ŷ ����  							
		 * @param lpOverlapPlus ������ Overlapped����ü
		 * @return  ��������
		 */
		bool				ReleaseRecvPacket(LPOVERLAPPEDPLUS lpOverlapPlus);

		/**
		 * @brief	Send ��Ŷ ����
		 * @param lpOverlapPlus ������ Overlapped����ü
		 * @return  ��������
		 */
		bool				ReleaseSendPacket(LPOVERLAPPEDPLUS lpOverlapPlus);

		/**
		 * @brief	���� Receive ó��
		 * @param buflen  ���� ���� ������
		 * @return  ��������
		 */
		bool				RecvPost(UINT buflen=0);

		/**
		 * @brief	�α׿� �޽����� ����մϴ�.
		 * @param MsgIndex �޽��� ���̵�
		 */
		virtual void		ShowMessage(INT MsgIndex);

		/**
		 * @brief	 �����ڿ��� Connect �̺�Ʈ �߻�
		 * @param bConnect Connect ��������
		 */
		virtual void		OnConnect(bool bConnect) { };

		/// Disconnect �̺�Ʈ �߻�
		virtual	void		OnDisconnect() { };

		// ���� ó�� ���� �Լ� 
		/**
		 * @brief	�޼��� ���� �� Receive ��ȣ �߻�, Socket����
		 * @param lpOverlapPlus Overlapped ����ü
		 * @return  Recv ó�� ���� ���� 
		 */
		virtual bool		DispatchPacket( LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager);

		/**
		 * @brief	���� ��Ŷ�� ó���ϴ� �κ��̴�
		 * @param Packet ó���� ��Ŷ ����ü
		 */
		virtual void		DispatchPacket( NFPacket& Packet ) 
		{
			// ��ӹ޾Ƽ� ��Ŷ�� ó���Ҷ��� �Ʒ��� ���� ���� ������ ����ؼ� ����Ѵ�.
			//Nave::Sync::CSSync Live(&m_Sync);
		};

	protected:
		/// �ִ� ���� ������ 
		INT					m_nMaxBuf = 0;							

		/// �ִ� ��Ŷ ������
		INT					m_nMaxPacketSize = 0;						

		/// Ŭ���̾�Ʈ ���� ���� 
		SOCKET				m_Socket = NULL;								
		
		/// Listener ���� 
		SOCKET				m_sckListener = NULL;							

		/// ���� ������ ������ �ִ� ��ü 
		sockaddr_in			m_Local;	

		/// Ŭ���̾�Ʈ ������ ������ �ִ� ��ü 
		sockaddr_in			m_Peer;									
		
		/// �� Ŭ���̾�Ʈ ���� ��ü�� ��ȣ 
		INT					m_dwIndex;								

		/// ���� ����Ʈ �˻� ���� 
		INT					m_nBytesSent;							

		/// IOCP �ڵ� 	
		HANDLE				m_hIOCP = NULL;								

		//�� Ŭ������ IOCP���� ��Ŷ�� �ְ� ������ Socket�� ���� ����Ǵ� ���۸� �����Ѵ�.
		/// ��ŶǮ ������ 
		NaveNetLib::NFPacketPool*		m_pPacketPool = nullptr;

		/// �� ��ü�� Ŭ���̾�Ʈ�� ���� �˻�
		bool				m_bIsConnect = false;							
		
		/// ���� Tick Count 
		LONG				m_uRecvTickCnt = 0;							

		/// Ŀ�ؼ� ���� ����
		CONNECT_EVENT		m_eConnectFlag = CONNECT_NONE;

		/// ���� ���� �ʱ�ȭ ���� ����
		bool				m_bForce = false;								

		/// Sync ��ü
		std::mutex		m_Lock; //TODO:�����. ������� �ʴ� ��

		// checksum ó�� ///////////////////////////////////////////// 
		// CPacketIOBuffer�� Recv���� ��Ŷ ����(�̶� ��ŶǮ�� ���ۿ� �ִ�)�� �����ϴ� IOBuffer �̴�.
		// IOBuffer�� ����ϴ� ������ Recv ���� ��Ŷ�� ������ ��Ŷ�� �ƴҶ� ó���ϱ� ���� �ϳ���
		// �����̴�.
		
		/// Recv�� IOBuffer
		NFPacketIOBuffer		m_RecvIO;								
		
		// CPacket�� �ϳ��� ��Ŷ�� �����ϴ� Ŭ������ IOBuffer ���� ��Ŷ�� ���� �����Ҷ� ���ȴ�.
		// Packet���� ��Ŷ ���۰� �迭�� ���ǵǾ� �ִ�. (memcpy�� ����ϱ� ������ �����ε尡 �߻��Ѵ�.)
		/// Recv�� �ϱ����� ����ϴ� �ӽ� ��Ŷ ����
		NFPacket				m_RecvPacket;
	};

}