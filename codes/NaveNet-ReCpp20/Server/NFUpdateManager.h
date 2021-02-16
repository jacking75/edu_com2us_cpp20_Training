/** 
 *  @file  		NFUpdateManager.h
 *  @brief 		한개의 완성된 패킷을 받았을때 해당패킷을 업데이트 하는 원형큐
 *  @remarks 
 *  @author  	강동명(edith2580@gmail.com)
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
		static const int MaxQueCount = 1024; // 최대 1024 개의 패킷을 저장할 수 있다.

		// 원형큐 대용.
		INT						m_Head = 0;
		INT						m_Tail = 0;
		NFConnection*			m_Conn[MaxQueCount];
		NFPacket				m_Packet[MaxQueCount];

		/// Sync 객체
		std::mutex				m_Lock;		
	};

}