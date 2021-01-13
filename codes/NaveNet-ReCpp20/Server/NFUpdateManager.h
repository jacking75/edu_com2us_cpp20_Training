/** 
 *  @file  		NFUpdateManager.h
 *  @brief 		�Ѱ��� �ϼ��� ��Ŷ�� �޾����� �ش���Ŷ�� ������Ʈ �ϴ� ����ť
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#include <mutex>

#include "NFPacket.h"
#include "NFConnection.h"

namespace NaveNetLib {

	class NFUpdateManager
	{
	public:
		NFUpdateManager(void);
		~NFUpdateManager(void) = default;

	public:
		void Add(NFConnection* pUser, NFPacket* pPacket);
		void Update();
				
		
	private:
		enum {
			// �ִ� 1024 ���� ��Ŷ�� ������ �� �ִ�.
			MaxQueCount = 1024,
		};

		// ����ť ���.
		INT						m_Head;
		INT						m_Tail;
		NFConnection*			m_Conn[MaxQueCount];
		NFPacket				m_Packet[MaxQueCount];

		/// Sync ��ü
		std::mutex				m_Lock;		
	};

}