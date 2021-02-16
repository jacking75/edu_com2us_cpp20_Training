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
		NFUpdateManager(void) = default;
		~NFUpdateManager(void) = default;

	public:
		void Add(NFConnection* pUser, NFPacket* pPacket);
		void Update();
				
		
	private:		
		static const int MaxQueCount = 1024; // �ִ� 1024 ���� ��Ŷ�� ������ �� �ִ�.

		// ����ť ���.
		INT						m_Head = 0;
		INT						m_Tail = 0;
		NFConnection*			m_Conn[MaxQueCount];
		NFPacket				m_Packet[MaxQueCount];

		/// Sync ��ü
		std::mutex				m_Lock;		
	};

}