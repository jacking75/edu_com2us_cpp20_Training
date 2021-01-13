/** 
 *  @file		NFIOBuffer.h
 *  @brief		I/O Buffering, Packet I/O Buffering
 *  @remarks	
 *  @author		������(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once

namespace NaveNetLib { 

	class NFPacket;

	/**
	 @class		
	 @author	������(edith2580@gmail.com)
	 @date		2009-03-03
	 @brief		
	 @warning   
	*/
	/** 
	 *  @class		NFIOBuffer
	 *  @brief		I/O Buffering Ŭ����
	 *  @remarks	I/O Buffering �� ���� Ŭ����. Sock�� �̿��� ��Ŷ�� �ְ� ������ ��Ŷ��				\r\n
	 *  			���Ŀ��ų� ���ҵǾ� �ö� �� ��Ŷ�� ���� �� ó���Ҷ� ���ȴ�.(IOBuffer��			\r\n
	 *  			��ӹ��� PacektIOBuffer Ŭ������ ó���ϰ�..											\r\n
	 *  																								\r\n
	 *  			�⺻������ PacketBuffer�� ����Ǵ� ��Ʈ��(?)�� ������ ������ ����.					\r\n
	 *  			---------------------------------------------------------------------------------------	\r\n
	 *  			| 2byte | 2byte | CheckSum(4Byte) | Header.Siz-Headere ��ŭ�� ���� Packet �뷮 Header |	\r\n
	 *  			---------------------------------------------------------------------------------------	\r\n
	 *  			IOBufferŬ������ ���� ��Ŷ�� �ϳ��� �迭�� ���������� �־� �� ��Ŷ��				\r\n
	 *  			�����Ѵ�.  2Byte�� 65535������ ���̸� ó���Ѵ�.										\r\n
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-04
	 */
	class NFIOBuffer
	{
	public:
		/// NFIOBuffer ������
		NFIOBuffer();
		/// NFIOBuffer �Ҹ���
		virtual ~NFIOBuffer();

	public:
		/// ���� ������ ũ�� ���.
		void	CalcBuffered();

		/**
		 * @brief	���� �����͸� ���ɴϴ�.
		 * @return  ���� ������
		 */
		inline char* GetBuffer() { return	m_cBuf; }
		/**
		 * @brief	Head Pos�� �����մϴ�.
		 * @param Head Head Pos��
		 */
		inline void SetHead(int Head) { m_iHead = Head; CalcBuffered(); }
		/**
		 * @brief	Head Pos�� �����ɴϴ�.
		 * @return	Head Pos ��
		 */
		inline int	GetHead() { return m_iHead; }
		/**
		 * @brief	Tail Pos�� �����մϴ�.
		 * @param Tail Tail Pos��
		 */
		inline void SetTail(int Tail) { m_iTail = Tail; }
		/**
		 * @brief	Tail Pos�� �����ɴϴ�.
		 * @return	Tail Pos ��
		 */
		inline int	GetTail() { return m_iTail; }
		/**
		 * @brief	������ ��ü ũ�⸦ ���ɴϴ�.
		 * @return  ������ ��ü ũ��
		 */
		inline int	GetBufSize() { return m_iBufSize; }

		/**
		 * @brief	������� ������ ���̸� ���ɴϴ�.
		 * @return	������� ������ ����
		 */
		inline int GetBufferUsed() { return m_iBuffered; }
		/**
		 * @brief	����ִ� ������ ���̸� ���ɴϴ�.
		 * @return	����ִ� ������ ����
		 */
		inline int GetEmptyBuffer() { return m_iBufSize - m_iBuffered; }

		/**
		 * @brief	���۸� �ʱ�ȭ�մϴ�.
		 */
		void	InitIOBuf();

		/**
		 * @brief	���۸� �߰��մϴ�.
		 * @param Buffer	����
		 * @param Size		ũ��
		 * @return	�߰��� ����
		 */
		int		Append(const char* Buffer, int Size);

		/**
		 * @brief	���۸� �н��ϴ�.
		 * @param Buffer	����
		 * @param Size		���� ũ��
		 * @return	�о���� ����
		 */
		int		GetData(char* Buffer, int Size);

		/**
		 * @brief	���۱��� üũ�մϴ�.
		 * @param Size	üũ�� ����
		 * @return	üũ�� ���� ������ Size���� ����
		 */
		int		CheckData(int Size);
		
		/**
		 * @brief	���۸� �����մϴ�.
		 * @param BufSize ������ ���� ũ��
		 */
		void	NewIOBuf(int BufSize);

		/**
		 * @brief	���۸� �����մϴ�.
		 */
		void	DeleteIOBuf();

	private:
		/// ���� ������ ������ġ.
		int		m_iHead;
		/// ���� ������ ��.
		int		m_iTail;
		/// ������ ������ ������
		int		m_iBufSize;
		/// ������ ������ ������.
		char*	m_cBuf;

		/// ���� ������ ������ �̰��� ������ Over Flow..
		int		m_iBuffered;
	};

	/** 
	 *  @class        NFPacketIOBuffer
	 *  @brief        Packet I/O Buffering Ŭ����
	 *  @remarks      NFPacket�� �ڵ�ȭ �Ǿ��ִ� Ŭ����		\r\n
	 *	   			  ������ Ŭ���̾�Ʈ�� ��ſ��� ���ȴ�.
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-04
	 */
	class NFPacketIOBuffer : public NFIOBuffer
	{
	public:
		/// NFPacketIOBuffer ������
		NFPacketIOBuffer();
		/// NFPacketIOBuffer �Ҹ���
		virtual ~NFPacketIOBuffer();

	public:
		/// ���۸� Lock �մϴ�.
		void	Lock();

		/// ���۸� UnLock �մϴ�.
		void	UnLock();

		// �Ѱ��з��� ��Ŷ�� ���´�.
		int		GetPacket(NFPacket* Packet);

		// ��Ŷüũ
		bool	CheckPacket();

	private:
		/// Lock �� UnLock���� ����� ���� 
		int		m_iLockHead;
	};


}