#include <memory.h>
#include "NFIOBuffer.h"
#include "NFPacket.h"

namespace NaveNetLib { 

	// IOBuffer�� �⺻ũ�� ��Ŷ 1024����ŭ ó������
	const int IOBUF_DEF_SIZE = DEF_MAXPACKETSIZE*128;

	NFIOBuffer::NFIOBuffer() : m_iHead(0), m_iTail(0), m_iBufSize(0), m_cBuf(0), m_iBuffered(0)
	{
	}

	NFIOBuffer::~NFIOBuffer()
	{
		DeleteIOBuf();
	}

	void NFIOBuffer::InitIOBuf()
	{
		m_iHead = m_iTail = 0;
		m_iBuffered = 0;

		if(m_cBuf)
			memset(m_cBuf, 0, sizeof(m_cBuf));
	}

	void NFIOBuffer::NewIOBuf(int BufSize)
	{
		if (BufSize <= 0) {
			BufSize = IOBUF_DEF_SIZE;
		}

		m_cBuf = new char[BufSize];
		if(m_cBuf == NULL)
		{
	//		throw "NFIOBuffer::NewIOBuf : Memory allocation failure!";
			return;
		}
		
		m_iBufSize = BufSize;
		
		InitIOBuf();
	}

	void NFIOBuffer::DeleteIOBuf()
	{
		if(m_cBuf)
		{
			delete []m_cBuf;
			m_cBuf = NULL;
		}

		m_iBufSize = 0;
		m_iHead = m_iTail = 0;
		m_iBuffered = 0;
	}

	// Size ��ŭ Buffer�� ������ ����.
	int NFIOBuffer::Append(const char* Buffer, int Size)
	{
		// �����÷ο찡 �ȴ�. 
		if(m_iBuffered + Size >= m_iBufSize)
		{
	//		throw "NFIOBuffer::Append : Buffer overflow";
			return -1;
		}

		auto aSize = 0;
		auto Added = 0;

		// ��� Size�� �߰��Ҷ����� ó���Ѵ�.
		while(Size > 0)
		{
			if (Size > m_iBufSize - m_iTail) {
				aSize = m_iBufSize - m_iTail;
			}
			else {
				aSize = Size;
			}

			if(aSize)
			{
				memcpy(m_cBuf+m_iTail, Buffer, aSize);

				Added += aSize;
				Size -= aSize;
				Buffer += aSize;
				m_iTail += aSize;
				
				if (m_iTail >= m_iBufSize) {
					m_iTail -= m_iBufSize;
				}
			}
		}

		CalcBuffered();

		return Added;
	}

	// Size��ŭ ����Ÿ�� �о� Buffer�� ����.
	int	NFIOBuffer::GetData(char* Buffer, int Size)
	{
		// ���ִ� ����Ÿ ���� ���� ������ ���ִ� ����Ÿ�� �а� �Ѵ�.
		if (GetBufferUsed() < Size) {
			Size = GetBufferUsed();
		}

		if (Size <= 0) {
			return 0;
		}

		// �߷�������. ó���Ѵ�.
		if(m_iHead+Size >= m_iBufSize)
		{
			// ���⿡ ���� ���װ� �ִ�.
			auto Size1 = m_iBufSize - m_iHead;
			memcpy(Buffer, m_cBuf+m_iHead, Size1);
			memcpy(Buffer+Size1, m_cBuf, Size-Size1);
		}
		else	// ���߷� ������.
		{
			memcpy(Buffer, m_cBuf+m_iHead, Size);
		}

		m_iHead += Size;
		if (m_iHead >= m_iBufSize) {
			m_iHead -= m_iBufSize;
		}

		CalcBuffered();
		return Size;
	}

	int NFIOBuffer::CheckData(int Size)
	{
		// ���ִ� ����Ÿ ���� ���� ������ ���ִ� ����Ÿ�� �а� �Ѵ�.
		if (GetBufferUsed() < Size) {
			Size = GetBufferUsed();
		}

		if (Size <= 0) {
			return 0;
		}

		m_iHead += Size;
		if (m_iHead >= m_iBufSize) {
			m_iHead -= m_iBufSize;
		}

		CalcBuffered();
		return Size;
	}


	void NFIOBuffer::CalcBuffered()
	{
		if (m_iHead > m_iTail) {
			m_iBuffered = m_iBufSize - m_iHead + m_iTail;
		}
		else {
			m_iBuffered = m_iTail - m_iHead;
		}
	}


	NFPacketIOBuffer::NFPacketIOBuffer() : m_iLockHead(0)
	{
		// �ʱ�ȭ.
		NFIOBuffer::NFIOBuffer();
	}

	NFPacketIOBuffer::~NFPacketIOBuffer()
	{
		DeleteIOBuf();
	}

	void NFPacketIOBuffer::Lock()
	{
		m_iLockHead = GetHead();
	}

	void NFPacketIOBuffer::UnLock()
	{
		SetHead(m_iLockHead);
	}

	bool NFPacketIOBuffer::CheckPacket()
	{
		PACKETHEADER	header;
		auto HeaderSize = (int)sizeof(PACKETHEADER);

		// Header
		if(GetBufferUsed() < HeaderSize)
		{
			// ������ �ƴ϶� ����Ÿ�� ���°Ŵ�.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < P_SIZE";
			return false;
		}

		if(HeaderSize != GetData((char*)&header, HeaderSize))
		{
			// ����� ������� �ʱ�ȭ �Ѵ�.
	//		throw "NFPacketIOBuffer::GetPacket : Packet Header Size != P_SIZE";
			return false;
		}

		int PacketSize = header.Size-HeaderSize;
		// ����� ����� ���µ� ���۰� ����� ������ Ȯ���Ѵ�.
		if(GetBufferUsed() < PacketSize)
		{
			// ����� ������� �ʱ�ȭ �Ѵ�.
			// ������ �ƴ϶� ����Ÿ�� ���°Ŵ�.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < PacketLen";
			return false;
		}

		
		int GetSize = CheckData(PacketSize);
		
		if(GetSize != PacketSize)
		{
			// ����� ������� �ʱ�ȭ �Ѵ�.
	//		throw "NFPacketIOBuffer::GetPacket : Packet Data Read Faild, PacketSize != GetSize";
			return false;
		}

		return true;
	}

	int NFPacketIOBuffer::GetPacket(NFPacket* Packet)
	{
		int OldHead = GetHead();

		int HeaderSize = HEADERSIZE;

		// Header
		if(GetBufferUsed() < HeaderSize)
		{
			// ������ �ƴ϶� ����Ÿ�� ���°Ŵ�.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < P_SIZE";
			return -1;
		}

		if(HeaderSize != GetData((char*)&Packet->m_Header, HeaderSize))
		{
			// ����� ������� �ʱ�ȭ �Ѵ�.
			SetHead(OldHead);
	//		throw "NFPacketIOBuffer::GetPacket : Packet Header Size != P_SIZE";
			return -2;
		}

		int PacketSize = Packet->m_Header.Size-HeaderSize;
		// ����� ����� ���µ� ���۰� ����� ������ Ȯ���Ѵ�.
		if(GetBufferUsed() < PacketSize)
		{
			// ����� ������� �ʱ�ȭ �Ѵ�.
			SetHead(OldHead);
			// ������ �ƴ϶� ����Ÿ�� ���°Ŵ�.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < PacketLen";
			return -3;
		}

		// ���� ����Ÿ�� ����� ó���ȵƴ�.
		int GetSize = GetData(Packet->m_Packet, PacketSize);
		if(GetSize != PacketSize)
		{
			// ����� ������� �ʱ�ȭ �Ѵ�.
			SetHead(OldHead);
	//		throw "NFPacketIOBuffer::GetPacket : Packet Data Read Faild, PacketSize != GetSize";
			return -4;
		}

		// ��Ŷ�� ����� �о�鿴��.
		return 1;
	}

}