/** 
 *  @file		NFPacket.h
 *  @brief		��Ŷ Ŭ����
 *  @remarks	
 *  @author		������(edith2580@gmail.com)
 *  @date		2009-04-02
 */

/*
2017. 05.25 ����� ����
*/

#pragma once

#ifdef _WIN32
	#pragma pack( push ,1 )
#else    
	#pragma pack(1)
#endif

namespace NaveClientNetLib {

	using USHORT = unsigned short;
	using UINT = unsigned int;

	const int		DEF_BUFSIZE = 1024;						/// Mem Pool and Socket Buffer size
	const int		DEF_PACKETSIZE = 1016;						/// Packet Size 
	const int		DEF_MAXPACKETSIZE = 1024;						/// Max Packet Size ( Header + Packet )

	const char		CONNECT_CHECK_DATA[] = "NFG";						/// CheckSum ��Ŷ (ó������ Server�� Connect ���� ���޵ȴ�)
	const int		CONNECT_CHECK_SIZE =	4;							/// �ʱ� ���� ���� ��Ŷ ������ ** WARINING **

	/// ��Ŷ ��� ����ü
	typedef struct PACKETHEADER
	{
		USHORT	Size;		/// ���� ��Ŷ ũ��
		USHORT	Command;	/// ��Ŷ�� ����
		UINT	CheckSum;	/// üũ��
	}*LPPACKETHEADER;

	const int HEADERSIZE = 8;			/// ��Ŷ ����� ������

	/** 
	 *  @class        NFPacket
	 *  @brief        �Ѱ��� ��Ŷ�� ���� �ϱ� ���� Ŭ���� ��ü
	 *  @remarks     ��Ŷ�� ����										\r\n
																		\r\n
					Header( Size, Command) + CheckSum + Data			\r\n
					------------------------------------------------------------0-------------------\r\n
					| 2byte | 2byte | CheckSum(4Byte) | Header.Size-Header ��ŭ�� ���� Packet �뷮 |\r\n
					--------------------------------------------------------------00----------------\r\n
																		\r\n
					��)													\r\n
					#define USER_LOGIN	1								\r\n
																		\r\n
					typedef struct stLogin								\r\n
					{													\r\n
						CHAR	UserID[16];								\r\n
						CHAR	UserPass[12];							\r\n
					}LOGIN, *LPLOGIN;									\r\n
																		\r\n
					// Header ����										\r\n
					Packet.m_Header.Size = sizeof(LOGIN);				\r\n
					Packet.m_Header.Command = USER_LOGIN;				\r\n
																		\r\n
					// ����											\r\n
					static LPLOGIN login = (LPLOGIN)Packet.m_Packet;	\r\n
					login->UserID;										\r\n
					login->UserPass;									\r\n

	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFPacket 
	{
	public:
		/// NFPacket ������
		NFPacket();
		
		/// NFPacket �Ҹ���
		~NFPacket() = default;

		/// ��Ŷ �ʱ�ȭ
		void Init();

		/**
		 * @brief	����� Command ���
		 * @return	Command ����
		 */
		inline USHORT GetCommand() const 
		{
			return m_Header.Command;
		}

		/**
		 * @brief	����� Command�� �����Ѵ�.
		 * @param shCommand Command ��
		 */
		inline void SetCommand(USHORT shCommand)
		{
			m_Header.Command = shCommand;
		}

		/**
		 * @brief	��Ŷ ������ ���ϱ�
		 * @return  ��Ŷ ������
		 */
		inline int GetSize()
		{
			return m_Header.Size-HEADERSIZE;
		}

		/**
		 * @brief	��Ŷ�� ����� �����Ѵ�.
		 * @param shDataSize 
		 */
		inline void SetSize(USHORT shDataSize)
		{
			// Size = Header + DataSize;
			m_Header.Size = HEADERSIZE + shDataSize;
		}	

		/// ��Ŷ�� ��ȣȭ�Ѵ�.
		UINT EncryptPacket(const bool isEncrypt=false);

		/// ��Ŷ�� ���ڵ��Ѵ�.
		void DecryptPacket();

		/// CheckSum ������ ���� ������ üũ���� �������� �˻��Ѵ�.
		bool IsAliveChecksum(const bool isEncrypt = false);


	public:
		/// ��Ŷ�� ��� ����
		PACKETHEADER		m_Header;

		/// ���� ��Ŷ�� ����
		char				m_Packet[DEF_PACKETSIZE];
					
	};

}

#pragma pack()
