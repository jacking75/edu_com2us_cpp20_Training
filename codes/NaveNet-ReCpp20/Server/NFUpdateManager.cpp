#include "NaveServer.h"
#include "NFUpdateManager.h"

namespace NaveNetLib {

//	INT				iMaxProcPacket = 0;
//	INT				iProcPacket = 0;		// 이 객체가 클라이언트와 연결 검사
//	DWORD			uProcTickCnt = 0;		// 수신 Tick Count 
		
	void NFUpdateManager::Update()
	{
		std::lock_guard<std::mutex> Sync(m_Lock);

		auto Tail = m_Tail;
/*
		if(timeGetTime() - uProcTickCnt > 1000)
		{
			InterlockedExchange((LONG*)&iMaxProcPacket,iProcPacket);
			InterlockedExchange((LONG*)&iProcPacket,0);
			InterlockedExchange((LONG*)&uProcTickCnt,timeGetTime());
		}
*/
		// 없다.
		if (m_Head == Tail) {
			return;
		}

		auto iCount = 0;

		// Tail이 앞에 있다.
		if(Tail < m_Head)
		{
			for(int i = m_Head; i < MaxQueCount; ++i)
			{
				if (m_Conn[i] == nullptr) {
					continue;
				}

				m_Conn[i]->UpdatePacket(m_Packet[i]);
				m_Conn[i] = nullptr;
				iCount++;
			}

			for(int i = 0; i < Tail; ++i)
			{
				if (m_Conn[i] == nullptr) 
				{
					continue;
				}

				m_Conn[i]->UpdatePacket(m_Packet[i]);
				m_Conn[i] = nullptr;
				iCount++;
			}
		}
		else // Tail이 뒤에 있다.
		{
			for(int i = m_Head; i < Tail; ++i)
			{
				if (m_Conn[i] == nullptr)
				{
					continue;
				}

				m_Conn[i]->UpdatePacket(m_Packet[i]);
				m_Conn[i] = nullptr;
				iCount++;
			}

		}

//		InterlockedExchange((LONG*)&iProcPacket,iProcPacket+iCount);

		m_Head = Tail;
	}

	void NFUpdateManager::Add(NFConnection* pUser, NFPacket* pPacket)
	{
		std::lock_guard<std::mutex> Sync(m_Lock);

		if (!pUser) 
		{
			return;
		}

		auto tail = m_Tail;
		m_Conn[tail] = pUser;
		
		if (pPacket) 
		{
			memcpy(&m_Packet[tail], pPacket, sizeof(NFPacket));
		}
		else 
		{
			m_Packet[tail].Init();
		}

		if ((tail + 1) >= MaxQueCount) 
		{
			m_Tail = 0;
		}
		else 
		{
			m_Tail = tail + 1;
		}

		/*if (m_Tail == m_Head)
		{
			NAVE_ERROR(("UpdateManager Overflow. (H:%d, T:%d)", m_Head, m_Tail));
		}*/
	}

}